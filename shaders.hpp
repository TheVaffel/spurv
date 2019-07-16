#ifndef __SPURV_SHADER
#define __SPURV_SHADER

namespace spurv {

  /*
   * SpurvShader - The object responsible for IO and compilation of the shader
   */
  
  template<SpurvShaderType type, typename... InputTypes>
  class SpurvShader {


    struct InputVariableEntry {
      int id;
      DSpurvType ds;
      int pointer_id;

      void* value_node;
      
      InputVariableEntry();
    };

    template<typename s_type>
    struct BuiltinEntry {
      ValueNode<s_type>* value_node;
      int pointer_id;
    };
    
    // We use this to reset the type declaration_states (stored for each type) after compilation
    std::vector<TypeDeclarationState*> defined_type_declaration_states;
    
    std::vector<InputVariableEntry> input_entries;
    std::vector<uint32_t> output_pointer_ids;
    std::vector<SpurvUniformBindingBase*> uniform_bindings;

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

    template<typename tt>
    void output_shader_header_output_variables(std::vector<uint32_t>& binary,
					       int n, ValueNode<tt>& val);
    template<typename tt, typename... NodeTypes>
    void output_shader_header_output_variables(std::vector<uint32_t>& binary, int n,
					       ValueNode<tt>& val, NodeTypes... args);

    template<typename tt>
    void output_shader_header_decorate_output_variables(std::vector<uint32_t>& binary, int n,
							ValueNode<tt>& val);
    template<typename tt, typename... NodeTypes>
    void output_shader_header_decorate_output_variables(std::vector<uint32_t>& binary, int n,
							ValueNode<tt>& val, NodeTypes... args);

    template<typename tt>
    void output_type_definitions(std::vector<uint32_t>& binary, ValueNode<tt>& val);

    template<typename tt, typename... NodeTypes>
    void output_type_definitions(std::vector<uint32_t>& binary, ValueNode<tt>& val,
				 NodeTypes... args);

    void output_main_function_begin(std::vector<uint32_t>& res);
    void output_main_function_end(std::vector<uint32_t>& res);
    
    template<typename tt>
    void output_output_definitions(std::vector<uint32_t>& res, int n, ValueNode<tt>& node);

    template<typename tt, typename... NodeType>
    void output_output_definitions(std::vector<uint32_t>& res, int n, ValueNode<tt>& node,
				   NodeType... args);

    template<int n>
    void output_input_pointers(std::vector<uint32_t>& res);
    
    template<int n, typename CurrType, typename... RestTypes>
    void output_input_pointers(std::vector<uint32_t>& res);

    template<typename tt, typename... NodeTypes>
    void output_output_pointers(std::vector<uint32_t>& res, int n, ValueNode<tt>& val, NodeTypes... args);

    template<typename tt>
    void output_output_pointers(std::vector<uint32_t>& res, int n, ValueNode<tt>& val);

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
    
  public:
    SpurvShader();

    template<BuiltinVariableIndex ind, typename tt>
    void setBuiltinOutput(ValueNode<tt>& val);

    // In SPIR-V, most bindings must (excluding more complex datatypes like images)
    // be given in a struct, thus we let the developer specify which member of the
    // struct that is desirable
    template<typename tt>
    ValueNode<tt>& getUniform(int set_no, int bind_no, int member_no);

    template<int n, typename First>
    auto& getInputVariable();
    
    template<int n, int c, typename First, typename... Rest>
    auto& getInputVariable();
    
    template<int n>
    auto& getInputVariable();

    template<typename... InnerTypes>
    SpurvUniformBinding<InnerTypes...>& getUniformBinding(int set_no, int binding_no);
    
    template<typename... NodeTypes>
    void compileToSpirv(std::vector<uint32_t>& res, NodeTypes&... args);
  };
  
};

#endif // __SPURV_SHADER
