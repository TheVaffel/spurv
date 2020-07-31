#ifndef __SPURV_SHADER
#define __SPURV_SHADER

#include "control_flow.hpp"

#include <set>
#include <stack>

namespace spurv {

  /*
   * BuiltinToType: Convert a builtin index to its type
   */
  
  template<SBuiltinVariable>
  struct BuiltinToType {};

  template<>
  struct BuiltinToType<BUILTIN_POSITION> { using type = vec4_s; };

  template<>
  struct BuiltinToType<BUILTIN_POINT_SIZE> { using type = float_s; };

  template<>
  struct BuiltinToType<BUILTIN_CLIP_DISTANCE> { using type = arr_1_float_s; };

  template<>
  struct BuiltinToType<BUILTIN_CULL_DISTANCE> { using type = arr_1_float_s; };

  template<>
  struct BuiltinToType<BUILTIN_INSTANCE_INDEX> { using type = uint32_s; };

  template<>
  struct BuiltinToType<BUILTIN_VERTEX_INDEX> { using type = uint32_s; };

  template<>
  struct BuiltinToType<BUILTIN_FRAG_COORD> { using type = vec4_s; };
  
  
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

    struct InputVariableBase {
    public: 
      int num;

      // I actually can't see any other way out than having these return void pointers
      virtual void* getVar() = 0;
      
      virtual void* getValue() = 0;

      InputVariableBase(int num) {
	this->num = num;
      }
      
      int getNum() {
	return this->num;
      }

      virtual int getPointerID() = 0;
    };

    template<typename tt>
    struct InputVariableEntry : public InputVariableBase {
      InputVar<tt>* input_var;
      SValue<tt>* val;

      InputVariableEntry(int n) : InputVariableBase(n) {
	this->input_var = SUtils::allocate<InputVar<tt> >(n);
	this->val = &this->input_var->load();
      }

      virtual void* getVar() {
	return (void*)input_var;
      }

      virtual void* getValue() {
	return (void*)val;
      }

      virtual int getPointerID() {
	return input_var->getID();
      }
    };

    template<typename s_type, SStorageClass storage>
    struct BuiltinEntry {
      SPointerVar<s_type, storage>* pointer;

      BuiltinEntry() {
	this->pointer = SUtils::allocate<SPointerVar<s_type, storage> >();
      }

      int getPointerID() {
	return this->pointer->getID();
      }
    };
    
    // We use this to reset the type declaration_states (stored for each type) after compilation
    std::vector<SDeclarationState*> defined_type_declaration_states;
    std::vector<bool*> decoration_states;
    
    std::vector<InputVariableBase*> input_entries;
    std::vector<uint32_t> output_pointer_ids;
    std::vector<SUniformBindingBase*> uniform_bindings;

    // Keeps track of the most recent loop, so we know which one to close
    std::vector<SControlStructureBase*> block_stack;

    std::set<SExtension> extensions;

    int glsl_id;
    int entry_point_id;
    int entry_point_declaration_size_index;
    int id_max_bound_index; // Will be constant, but oh well
    int builtin_pointer_id;
    
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

    void output_builtin_tree_type_definitions(std::vector<uint32_t>& binary);

    void output_main_function_begin(std::vector<uint32_t>& res);
    
    void output_main_function_end(std::vector<uint32_t>& res);
    
    void output_output_definitions(std::vector<uint32_t>& res, int n);

    template<typename in1, typename... NodeType>
    void output_output_definitions(std::vector<uint32_t>& res, int n, in1&& node,
				   NodeType&&... args);    
    void output_output_pointers(std::vector<uint32_t>& res, int n);
    
    template<typename in1, typename... NodeTypes>
    void output_output_pointers(std::vector<uint32_t>& res, int n, in1&& val, NodeTypes&&... args);

    
    void output_used_builtin_pointers(std::vector<uint32_t>& res);
    
    int get_num_defined_builtins();
    

    // Builtins
    BuiltinEntry<vec4_s, SStorageClass::STORAGE_OUTPUT>* builtin_vec4_out_0; // Vertex: Position
    BuiltinEntry<float_s, SStorageClass::STORAGE_OUTPUT>* builtin_float_out_0; // Vertex: PointSize
    BuiltinEntry<arr_1_float_s, SStorageClass::STORAGE_OUTPUT>* builtin_arr_1_float_out_0; // Vertex: Clip Distance
    BuiltinEntry<arr_1_float_s, SStorageClass::STORAGE_OUTPUT>* builtin_arr_1_float_out_1; // Vertex: Cull Distance
    
    BuiltinEntry<uint32_s, SStorageClass::STORAGE_INPUT>* builtin_uint32_in_0; // Vertex: VertexId
    BuiltinEntry<uint32_s, SStorageClass::STORAGE_INPUT>* builtin_uint32_in_1; // Vertex: InstanceId
    BuiltinEntry<vec4_s, SStorageClass::STORAGE_INPUT>* builtin_vec4_in_0; // Fragment: FragCoord

    void cleanup_declaration_states();
    void cleanup_decoration_states();

    SUniformBindingBase* find_binding(int set_no, int binding_no);
    template<typename BindingType>
    SUniformBindingBase* construct_binding(int set_no, int binding_no);
    
  public:
    SShader();
    
    template<SBuiltinVariable ind>
    SValue<typename BuiltinToType<ind>::type >& getBuiltin();
    
    template<SBuiltinVariable ind, typename tt>
    void setBuiltin(SValue<tt>& val);

    template<int n>
    SValue<typename SUtils::NthType<n, InputTypes...>::type>& input();
    
    template<typename tt>
    SPointerVar<tt, SStorageClass::STORAGE_UNIFORM_CONSTANT>& uniformConstant(int set_no, int binding_no);
    
    template<typename... InnerTypes>
    SPointerVar<SStruct<InnerTypes...>, SStorageClass::STORAGE_UNIFORM >& uniformBinding(int set_no, int binding_no);

    template<typename... InnerTypes>
    SPointerVar<SStruct<InnerTypes...>, SStorageClass::STORAGE_STORAGE_BUFFER>& storageBuffer(int set_no, int binding_no);
    
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
