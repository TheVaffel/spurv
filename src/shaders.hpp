#ifndef __SPURV_SHADER
#define __SPURV_SHADER

namespace spurv {

  /*
   * SShader - The object responsible for IO and compilation of the shader
   */
  
  template<SShaderType type, typename... InputTypes>
  class SShader {
    

    struct InputVariableEntry {
      int id;
      DSType ds;
      int pointer_id;

      void* value_node;
      
      InputVariableEntry();
    };

    template<typename s_type>
    struct BuiltinEntry {
      SValue<s_type>* value_node;
      int pointer_id;
    };
    
    // We use this to reset the type declaration_states (stored for each type) after compilation
    std::vector<SDeclarationState*> defined_type_declaration_states;
    std::vector<bool*> decoration_states;
    
    std::vector<InputVariableEntry> input_entries;
    std::vector<uint32_t> output_pointer_ids;
    std::vector<SUniformBindingBase*> uniform_bindings;

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

    void output_output_tree_type_definitions(std::vector<uint32_t>& binary);

    template<typename in1, typename... NodeTypes>
    void output_output_tree_type_definitions(std::vector<uint32_t>& binary, SValue<in1>& val,
					     NodeTypes&&... args);

    void output_builtin_tree_type_definitions(std::vector<uint32_t>& binary);

    void output_main_function_begin(std::vector<uint32_t>& res);
    void output_main_function_variables(std::vector<uint32_t>& res);
    
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

    void cleanup_declaration_states();
    void cleanup_decoration_states();
    
  public:
    SShader();

    template<SBuiltinVariable ind, typename tt>
    void setBuiltin(SValue<tt>& val);
    
    template<int n, typename First>
    auto& input();
    
    template<int n, int c, typename First, typename... Rest>
    auto& input();
    
    template<int n>
    auto& input();

    // Return an SUniformConstant if applicable, otherwise an SUniformBinding
    template<typename... InnerTypes>
    typename std::conditional<SUtils::isUniformConstantType<InnerTypes...>,
			      SUniformConstant<typename SUtils::NthType<0, InnerTypes...>::type>,
			      SUniformBinding<InnerTypes...> >::type& uniformBinding(int set_no, int binding_no);
    
    template<typename... NodeTypes>
    void compile(std::vector<uint32_t>& res, NodeTypes&&... args);
  };
  
};

#endif // __SPURV_SHADER
