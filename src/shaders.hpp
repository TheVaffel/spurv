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
    
    /* struct InputVariableEntry {
      int id;
      DSType ds;
      int pointer_id;

      void* value_node;
      
      InputVariableEntry();
      }; */

    struct InputVariableBase {
      int num;
      
      template<typename t1>
      virtual InputVar<t1>& getVar() = 0;
      
      template<typename t1>
      virtual SValue<t1>& getValue() = 0;

      InputVariableBase(int num) {
	this->num = num;
      }
      
      int getNum() {
	return this->num;
      }
    };

    template<typename tt>
    struct InputVariableEntry : public InputVariableBase {
      InputVar<tt>* input_var;
      SValue<tt>* val;

      InputVariableEntry(int n) : InputVariableBase(n) {
	this->input_var = SUtils::allocate<InputVar<tt> >(n);
	this->val = nullptr;
      }
      
      template<typename t1>
      virtual InputVar<t1>* getVar() {
	static_assert(std::is_same<t1, tt>::value, "[spurv::InputVariableEntry::getVar] No correspondence between types");
	return *input_var;
      }

      template<typename t1>
      virtual SValue<t1>* getVal() {
	static_assert(std::is_same<t1, tt>::value, "[spurv::InputVariableEntry::getValue] No correspondence between types");
	if(this->val == nullptr) {
	  this->val = input_var->load();
	}
	return this->val;
      }
      
    };

    template<typename s_type>
    struct BuiltinEntry {
      SValue<s_type>* value_node;
      int pointer_id;
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

    template<int n>
    void output_input_pointers(std::vector<uint32_t>& res);
    
    template<int n, typename CurrType, typename... RestTypes>
    void output_input_pointers(std::vector<uint32_t>& res);

    void output_output_pointers(std::vector<uint32_t>& res, int n);
    
    template<typename in1, typename... NodeTypes>
    void output_output_pointers(std::vector<uint32_t>& res, int n, in1&& val, NodeTypes&&... args);

    void output_uniform_pointers(std::vector<uint32_t>& res);
    
    void output_used_builtin_pointers(std::vector<uint32_t>& res);
    
    void output_builtin_output_definitions(std::vector<uint32_t>& res);
    
    int get_num_defined_builtins();
    

    // Builtin outputs
    BuiltinEntry<vec4_s>* builtin_vec4_0; // Vertex: Position
    BuiltinEntry<float_s>* builtin_float_0; // Vertex: PointSize
    BuiltinEntry<arr_1_float_s>* builtin_arr_1_float_0; // Vertex: Clip Distance
    BuiltinEntry<arr_1_float_s>* builtin_arr_1_float_1; // Vertex: Cull Distance
    BuiltinEntry<uint32_s>* builtin_uint32_0; // Vertex: VertexId
    BuiltinEntry<uint32_s>* builtin_uint32_1; // Vertex: InstanceId

    void cleanup_declaration_states();
    void cleanup_decoration_states();

    SUniformBindingBase* find_binding(int set_no, int binding_no);
    template<typename BindingType>
    BindingType& construct_binding(int set_no, int binding_no);
    
  public:
    SShader();
    
    template<SBuiltinVariable ind>
    SValue<typename BuiltinToType<ind>::type >& getBuiltin();
    
    template<SBuiltinVariable ind, typename tt>
    void setBuiltin(SValue<tt>& val);
    
    template<int n, typename First>
    auto& input();
    
    template<int n, int c, typename First, typename... Rest>
    auto& input();
    
    template<int n>
    auto& input();

    
    template<typename tt>
    SValue<tt>& uniformConstant(int set_no, int binding_no);
    
    template<typename... InnerTypes>
    SUniformBinding<InnerTypes...>& uniformBinding(int set_no, int binding_no);

    template<typename... InnerTypes>
    SStorageBuffer<InnerTypes...>& storageBuffer(int set_no, int binding_no);
    
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
