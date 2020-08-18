#ifndef __SPURV_SHADER
#define __SPURV_SHADER

#include "control_flow.hpp"

#include <set>
#include <stack>

namespace spurv {

  /*
   * BuiltinInfo: Convert a builtin index to its type
   */
  
  template<SShaderType, SBuiltinVariable>
  struct BuiltinInfo {};

  template<>
  struct BuiltinInfo<SShaderType::SHADER_VERTEX, BUILTIN_POSITION>
  { using type = vec4_s; static const SStorageClass storage = SStorageClass::STORAGE_OUTPUT; };

  template<>
  struct BuiltinInfo<SShaderType::SHADER_VERTEX, BUILTIN_POINT_SIZE>
  { using type = float_s; static const SStorageClass storage = SStorageClass::STORAGE_OUTPUT; };

  template<>
  struct BuiltinInfo<SShaderType::SHADER_VERTEX, BUILTIN_CLIP_DISTANCE>
  { using type = arr_1_float_s; static const SStorageClass storage = SStorageClass::STORAGE_OUTPUT; };

  template<>
  struct BuiltinInfo<SShaderType::SHADER_VERTEX, BUILTIN_CULL_DISTANCE>
  { using type = arr_1_float_s; static const SStorageClass storage = SStorageClass::STORAGE_OUTPUT; };

  template<>
  struct BuiltinInfo<SShaderType::SHADER_VERTEX, BUILTIN_INSTANCE_INDEX>
  { using type = uint32_s; static const SStorageClass storage = SStorageClass::STORAGE_INPUT; };

  template<>
  struct BuiltinInfo<SShaderType::SHADER_VERTEX, BUILTIN_VERTEX_INDEX>
  { using type = uint32_s; static const SStorageClass storage = SStorageClass::STORAGE_INPUT; };

  template<>
  struct BuiltinInfo<SShaderType::SHADER_FRAGMENT, BUILTIN_FRAG_COORD>
  { using type = vec4_s; static const SStorageClass storage = SStorageClass::STORAGE_INPUT; };

  template<>
  struct BuiltinInfo<SShaderType::SHADER_COMPUTE, BUILTIN_GLOBAL_INVOCATION_ID>
  { using type = uvec3; static const SStorageClass storage = SStorageClass::STORAGE_INPUT; };

  
  /*
   * SShader - The object responsible for IO and compilation of the shader
   */
  
  template<SShaderType type, typename... InputTypes>
  class SShader {    
  protected:
    
    const std::string shaderExtensions[EXTENSION_END] =
      {
       "SPV_KHR_storage_buffer_storage_class"
      };


    
    /*
     * Inner structs describing input and output variables
     */

    // Base for all IO variables

    struct IOVariableBase {
      virtual DSType getPointerDSType() = 0;
      
      virtual void* getVar() = 0;
      virtual int getPointerID() = 0;

      template<typename tt>
      SPointerTypeBase<tt>* getVarTyped() {
	DSType dt = this->getPointerDSType();
	if(dt != tt::getDSType()) {
	  printf("[spurv::IOVariableBase::getVarTyped] Supplied type differed from true type\n");
	  exit(-1);
	}

	return (SPointerTypeBase<tt>*)this->getVar();
      }
    };

    // Base for input variables
    struct InputVariableBase : public IOVariableBase {
      int num;
      
      virtual void* getValue() = 0;

      template<typename tt>
      SValue<tt>* getValueTyped() {
	DSType dt = this->getPointerDSType();
	DSType ds;
	tt::getDSType(&ds);
	if(dt != ds) {
	  printf("[spurv::InputVariableBase::getValueTyped] Supplied type differed from true type\n");
	  exit(-1);
	}

	return (SValue<tt>*)this->getValue();
      }

      InputVariableBase(int num) {
	this->num = num;
      }
    };

    // Base for output variables (Ended up creating separate for output variable base)
    struct OutputVariableBase : public IOVariableBase {
      int num;
      
      template<typename tt, typename t1>
      void store(t1&& in1) {
	DSType dt = this->getPointerDSType();
	DSType ds;
	tt::getDSType(&ds);
	if(dt != ds) {
	  printf("[spurv::OutputVariableBase::store] Supplied type differed from true type\n");
	  exit(-1);
	}
	
	SOutputVar<tt>* variable = (SOutputVar<tt>*)this->getVar();
      
	variable->store(in1);
      }
    
      OutputVariableBase(int num) {
	this->num = num;
      }
    };
  
    // Templated input variables
    template<typename tt>
    struct InputVariableEntry : public InputVariableBase {
      InputVar<tt>* input_var;
      SValue<tt>* val;

      InputVariableEntry(int n) : InputVariableBase(n) {
	this->input_var = SUtils::allocate<InputVar<tt> >(n);
	this->val = &this->input_var->load();
      }

      virtual void* getVar() {
	return (void*)this->input_var;
      }
      
      virtual void* getValue() {
	return (void*)this->val;
      }

      virtual int getPointerID() {
	return input_var->getID();
      }

      virtual DSType getPointerDSType() {
	DSType dt;
	tt::getDSType(&dt);
	return dt;
      }
    };


    // Templated output variable entries
    template<typename tt>
    struct OutputVariableEntry : public OutputVariableBase {
      SOutputVar<tt>* output_var;

      OutputVariableEntry(int n) : OutputVariableBase(n) {
	this->output_var = SUtils::allocate<SOutputVar<tt> >(n);
      }

      virtual void* getVar() {
	return (void*)this->output_var;
      }

      virtual int getPointerID() {
	return this->output_var->getID();
      }

      virtual DSType getPointerDSType() {
	DSType dt;
	tt::getDSType(&dt);
	return dt;
      }
    };

    
    struct BuiltinEntryBase {
      SBuiltinVariable builtin_id;

      virtual int getPointerID() = 0;
      
      void decorate(std::vector<uint32_t>& bin) {
	// Decorate <builtin_id> Builtin <builtin_type>
	SUtils::add(bin, (4 << 16) | 71);
	SUtils::add(bin, this->getPointerID());
	SUtils::add(bin, 11);
	SUtils::add(bin, this->builtin_id);
      }
    };

    // Templated builtin entries
    template<typename s_type, SStorageClass storage>
    struct BuiltinEntry : public BuiltinEntryBase {
      SPointerVar<s_type, storage>* pointer;

      BuiltinEntry(SBuiltinVariable builtin_id) {
	this->builtin_id = builtin_id;
	this->pointer = SUtils::allocate<SPointerVar<s_type, storage> >();
      }

      virtual int getPointerID() {
	return this->pointer->getID();
      }
    };
    
    // We use this to reset the type declaration_states (stored for each type) after compilation
    std::vector<SDeclarationState*> defined_type_declaration_states;
    std::vector<bool*> decoration_states;
    
    std::vector<InputVariableBase*> input_entries;
    std::vector<OutputVariableBase*> output_entries;
    std::vector<BuiltinEntryBase*> builtin_entries;
    std::vector<SUniformBindingBase*> uniform_bindings;

    // Keeps track of the most recent loop, so we know which one to close
    std::vector<SControlStructureBase*> block_stack;

    std::set<SExtension> extensions;

    int glsl_id;
    int entry_point_id;
    int entry_point_declaration_size_index;
    int id_max_bound_index; // Will be constant, but oh well
    
    void output_preamble(std::vector<uint32_t>& binary);
    void output_shader_header_begin(std::vector<uint32_t>& binary);
    void output_shader_header_end(std::vector<uint32_t>& binary);
    void output_used_builtin_ids(std::vector<uint32_t>& bin);
    void output_shader_header_decorate_begin(std::vector<uint32_t>& bin);

    void output_shader_header_output_variables(std::vector<uint32_t>& binary,
					       int n);
    
    template<typename in1, typename... NodeTypes>
    void output_shader_header_output_variables(std::vector<uint32_t>& binary, int n,
					       in1&& val, NodeTypes&&... args);

    void output_shader_header_decorate_output_variables(std::vector<uint32_t>& binary, int n);
    template<typename in1, typename... NodeTypes>
    void output_shader_header_decorate_output_variables(std::vector<uint32_t>& binary, int n,
							in1&& val, NodeTypes&&... args);

    void output_shader_header_decorate_tree(std::vector<uint32_t>& binary);
    template<typename in1, typename... NodeTypes>
    void output_shader_header_decorate_tree(std::vector<uint32_t>& binary,
					    in1&& val,
					    NodeTypes&&... args);
    
    void output_output_tree_type_definitions(std::vector<uint32_t>& binary);

    template<typename in1, typename... NodeTypes>
    void output_output_tree_type_definitions(std::vector<uint32_t>& binary, SValue<in1>& val,
					     NodeTypes&&... args);

    void output_main_function_begin(std::vector<uint32_t>& res);
    
    void output_main_function_end(std::vector<uint32_t>& res);
    
    int get_num_defined_builtins();

    void create_output_variables();
    
    template<typename tt1, typename... NodeTypes>
    void create_output_variables(SValue<tt1>& val, NodeTypes&&... rest);
    
    void cleanup_declaration_states();
    void cleanup_decoration_states();

    SUniformBindingBase* find_binding(int set_no, int binding_no);
    template<typename BindingType>
    SUniformBindingBase* construct_binding(int set_no, int binding_no);
    
  public:
    SShader();
    
    template<SBuiltinVariable ind>
    SValue<typename BuiltinInfo<type, ind>::type >& getBuiltin();
    
    template<SBuiltinVariable ind, typename tt>
    void setBuiltin(SValue<tt>& val);

    template<int n>
    SValue<typename SUtils::NthType<n, InputTypes...>::type>& input();
    
    template<typename tt>
    SPointerVar<tt, SStorageClass::STORAGE_UNIFORM_CONSTANT>& uniformConstant(int set_no, int binding_no);
    
    template<typename... InnerTypes>
    SPointerVar<SStruct<SDecoration::BLOCK, InnerTypes...>, SStorageClass::STORAGE_UNIFORM >& uniformBinding(int set_no,
													     int binding_no);

    template<typename... InnerTypes>
    SPointerVar<SStruct<SDecoration::BLOCK, InnerTypes...>, SStorageClass::STORAGE_STORAGE_BUFFER>& storageBuffer(int set_no,
														  int binding_no);
    
    template<typename tt>
    SLocal<tt>& local();

    // One argument means 0 - arg0, two arguments means arg0 - arg1
    SValue<int_s>& forLoop(int arg0, int arg1 = 0);

    void endLoop();

    void ifThen(SValue<bool_s>& condition);
    void elseThen();
    void endIf();

    void breakLoop();
    void continueLoop();
    
    template<typename... NodeTypes>
    void compile(std::vector<uint32_t>& res, NodeTypes&&... args);

  };

  template<typename... InputTypes>
  class VertexShader : public SShader<SShaderType::SHADER_VERTEX, InputTypes...> { };

  template<typename... InputTypes>
  class FragmentShader : public SShader<SShaderType::SHADER_FRAGMENT, InputTypes...> { };
};


#endif // __SPURV_SHADER
