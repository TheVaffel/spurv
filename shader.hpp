#ifndef __SPURV_SHADER
#define __SPURV_SHADER

namespace spurv {

  /*
   * Constructor(s?)
   */

  template<SpurvShaderType type, typename... InputTypes>
  SpurvShader<type, InputTypes...>::SpurvShader() {
    builtin_vec4_0 = nullptr;
    builtin_float_0 = nullptr;
    builtin_arr_1_float_0 = nullptr;
    builtin_arr_1_float_1 = nullptr;

    input_entries = std::vector<InputVariableEntry>(sizeof...(InputTypes)); 
  }
  
  
  /*
   * Member functions of inner classes
   */

  template<SpurvShaderType type, typename... InputTypes>
  SpurvShader<type, InputTypes...>::InputVariableEntry::InputVariableEntry() {
    this->id = -1;
  }


  /*
   * Util functions
   */

  template<SpurvShaderType type, typename... InputTypes>
  int SpurvShader<type, InputTypes...>::get_num_defined_builtins() {
    int num = 0;
    if(this->builtin_vec4_0) {
      num++;
    }
    if(this->builtin_float_0) {
      num++;
    }
    if(this->builtin_arr_1_float_0) {
      num++;
    }
    if(this->builtin_arr_1_float_1) {
      num++;
    }

    return num;
  }
  
  template<SpurvShaderType type, typename...InputTypes>
  void SpurvShader<type, InputTypes...>::cleanup_declaration_states() {
    for(uint i = 0; i < this->defined_type_declaration_states.size(); i++) {
      this->defined_type_declaration_states[i]->is_defined  = false;
      this->defined_type_declaration_states[i]->id = -1;
    }
  }
  
  
  /*
   * Output member functions - functions writing binaries to vector
   */
  
  template<SpurvShaderType type, typename... InputTypes>
  template<typename tt>
  void SpurvShader<type, InputTypes...>::output_shader_header_output_variables(std::vector<uint32_t>& binary,
									       int n, ValueNode<tt>& arg0) {
    static_assert(is_spurv_type<tt>::value);
    binary.push_back(this->output_pointer_ids[n]);
  }
  
  template<SpurvShaderType type, typename... InputTypes>
  template<typename tt, typename... NodeTypes>
  void SpurvShader<type, InputTypes...>::output_shader_header_output_variables(std::vector<uint32_t>& binary,
									       int n, ValueNode<tt>& arg0, NodeTypes... args) {
    static_assert(is_spurv_type<tt>::value);
    binary.push_back(this->output_pointer_ids[n]);
    output_shader_header_output_variables(binary, n + 1, args...);
  }

  template<SpurvShaderType type, typename... InputTypes>
  template<typename tt>
  void SpurvShader<type, InputTypes...>::output_shader_header_decorate_output_variables(std::vector<uint32_t>& bin, int n,
									 ValueNode<tt>& arg0) {
    bin.push_back((4 << 16) | 71);
    bin.push_back(this->output_pointer_ids[n]);
    bin.push_back(30);
    bin.push_back(n);
  }									 
  
  template<SpurvShaderType type, typename... InputTypes>
  template<typename tt, typename... NodeTypes>
  void SpurvShader<type, InputTypes...>::output_shader_header_decorate_output_variables(std::vector<uint32_t>& bin, int n,
									 ValueNode<tt>& arg0, NodeTypes... args) {
    bin.push_back((4 << 16) | 71);
    bin.push_back(arg0->getID());
    bin.push_back(30);
    bin.push_back(n);

    output_shader_header_decorate_output_variables(bin, n + 1, args...); 
  }

  template<SpurvShaderType type, typename... InputTypes>
  void SpurvShader<type, InputTypes...>::output_used_builtin_ids(std::vector<uint32_t>& bin) {
    if(this->builtin_vec4_0) {
      bin.push_back(this->builtin_vec4_0->pointer_id);
    }
    if(this->builtin_float_0) {
      bin.push_back(this->builtin_float_0->pointer_id);
    }
    if(this->builtin_arr_1_float_0) {
      bin.push_back(this->builtin_arr_1_float_0->pointer_id);
    }
    if(this->builtin_arr_1_float_1) {
      bin.push_back(this->builtin_arr_1_float_1->pointer_id);
    }
  }

  template<SpurvShaderType type, typename... InputTypes>
  void SpurvShader<type, InputTypes...>::output_preamble(std::vector<uint32_t>& binary) {
    binary.push_back(0x07230203); // Magic number
    binary.push_back(0x00010000); // Version number (1.0.0)
    binary.push_back(0x124);      // Generator's magic number (not officially registered)
    this->id_max_bound_index = binary.size();
    binary.push_back(0); // We'll set this later
    binary.push_back(0x0); // For instruction schema (whatever that means)
  }

  template<SpurvShaderType type, typename... InputTypes>
  template<typename tt>
  void SpurvShader<type, InputTypes...>::output_type_definitions(std::vector<uint32_t>& bin, ValueNode<tt>& val) {
    
    printf("Ensuring type %s\n", typeid(tt).name());
    val.ensure_type_defined(bin, this->defined_type_declaration_states);
  }

  
  template<SpurvShaderType type, typename... InputTypes>
  template<typename tt, typename... NodeTypes>
  void SpurvShader<type, InputTypes...>::output_type_definitions(std::vector<uint32_t>& bin, ValueNode<tt>& val, NodeTypes... args) {
    printf("Ensuring type %s\n", typeid(tt).name());
    val.ensure_type_defined(bin, this->defined_type_declaration_states);

    this->output_type_definitions(bin, args...);
  }


  template<SpurvShaderType type, typename... InputTypes>
  void SpurvShader<type, InputTypes...>::output_shader_header_begin(std::vector<uint32_t>& bin) {
    // capability Shader
    Utils::add(bin, (2 << 16) | 17);
    Utils::add(bin, 1);

    // GLSL = ext_inst_import "GLSL.std.450"
    std::string glsl_import_str = "GLSL.std.450";
    int length = Utils::stringWordLength(glsl_import_str);
    Utils::add(bin, ((2 + length) << 16) | 11);
    
    this->glsl_id = Utils::getNewID();
    Utils::add(bin, this->glsl_id);
    Utils::add(bin, glsl_import_str);

    // memory_model Logical GLSL450
    Utils::add(bin, (3 << 16) | 14);
    Utils::add(bin, 0);
    Utils::add(bin, 1);

    // entry_point vertex_shader main "main" input/output_variables
    this->entry_point_declaration_size_index = bin.size(); // Add size afterwards
    Utils::add(bin, 15);

    std::string entry_point_name = "main";

    if constexpr(type == SPURV_SHADER_VERTEX) {
	Utils::add(bin, 0); // Vertex
      } else if constexpr(type == SPURV_SHADER_FRAGMENT) {
	Utils::add(bin, 4); // Fragment
      } else {
      printf("Shader type not yet accounted for in output_shader_header_begin\n");
      exit(-1);
    }

    this->entry_point_id = Utils::getNewID();
    Utils::add(bin, this->entry_point_id);
    Utils::add(bin, entry_point_name);

    for(uint i = 0; i < this->input_entries.size(); i++) {
      if (this->input_entries[i].id == -1) {
	printf("Input variable not initialized");
	exit(-1);
      }
      Utils::add(bin, this->input_entries[i].pointer_id);
    }

    output_used_builtin_ids(bin);
  }

  template<SpurvShaderType type, typename... InputTypes>
  void SpurvShader<type, InputTypes...>::output_shader_header_end(std::vector<uint32_t>& bin) {
    if constexpr(type == SPURV_SHADER_FRAGMENT) {
	// OpExecutionMode <entry_point_id> OriginUpperLeft
	Utils::add(bin, (3 << 16) | 16);
	Utils::add(bin, this->entry_point_id);
	Utils::add(bin, 7);
      }
  }
  
  template<SpurvShaderType type, typename... InputTypes>
  void SpurvShader<type, InputTypes...>::output_shader_header_decorate_begin(std::vector<uint32_t>& bin) {
    if constexpr(type == SPURV_SHADER_VERTEX) {
	if(this->builtin_vec4_0) {
	  // Decorate <builtin> Builtin Position
	  Utils::add(bin, (4 << 16) | 71);
	  Utils::add(bin, this->builtin_vec4_0->pointer_id);
	  Utils::add(bin, 11);
	  Utils::add(bin, 0);
	}
    
	if(this->builtin_float_0) {
	  Utils::add(bin, (4 << 16) | 71);
	  Utils::add(bin, this->builtin_float_0->pointer_id);
	  Utils::add(bin, 11);
	  Utils::add(bin, 1);
	}
	if(this->builtin_arr_1_float_0) {
	  Utils::add(bin, (4 << 16) | 71);
	  Utils::add(bin, this->builtin_arr_1_float_0->pointer_id);
	  Utils::add(bin, 11);
	  Utils::add(bin, 3);
	}
	if(this->builtin_arr_1_float_1) {
	  Utils::add(bin, (4 << 16) | 71);
	  Utils::add(bin, this->builtin_arr_1_float_1->pointer_id);
	  Utils::add(bin, 11);
	  Utils::add(bin, 4);
	}
      } else {
      if(this->builtin_vec4_0 || this->builtin_float_0 ||
	 this->builtin_arr_1_float_0 || this->builtin_arr_1_float_1) {
	printf("Builtin variables for other than Vertex shader must still be implemented\n");
	exit(-1);
      }
    }

    // Decorate <input_var> Location <index>
    for(uint i = 0; i < input_entries.size(); i++) {
      Utils::add(bin, (4 << 16) | 71);
      Utils::add(bin, input_entries[i].pointer_id);
      Utils::add(bin, 30);
      Utils::add(bin, i);
    }

    // Put decorations on uniforms
    for(uint i = 0; i < uniform_bindings.size(); i++) {
      
      uniform_bindings[i]->decorateType(bin);

      // Decorate <uniform_binding> DescriptorSet <num>
      Utils::add(bin, (4 << 16) | 71);
      Utils::add(bin, uniform_bindings[i]->getPointerID());
      Utils::add(bin, 34);
      Utils::add(bin, uniform_bindings[i]->getSetNo());

      // Decorate <uniform_binding> Binding <num>
      Utils::add(bin, (4 << 16) | 71);
      Utils::add(bin, uniform_bindings[i]->getPointerID());
      Utils::add(bin, 33);
      Utils::add(bin, uniform_bindings[i]->getBindingNo());

    }
  }

  template<SpurvShaderType type, typename... InputTypes>
  void SpurvShader<type, InputTypes...>::output_main_function_begin(std::vector<uint32_t>& res) {
    SpurvType<SPURV_TYPE_VOID>::ensure_defined(res, this->defined_type_declaration_states);

    int void_function_type = Utils::getNewID();
    
    // OpTypeFunction <result_id> <result type> <result_id> 
    Utils::add(res, (3 << 16) | 33);
    Utils::add(res, void_function_type);
    Utils::add(res, SpurvType<SPURV_TYPE_VOID>::getID());

    // OpFunction <result type> <result_id> <function_control> <function_type>
    Utils::add(res, (5 << 16) | 54);
    Utils::add(res, SpurvType<SPURV_TYPE_VOID>::getID());
    Utils::add(res, entry_point_id);
    Utils::add(res, 0);
    Utils::add(res, void_function_type);

    // OpLabel <result_id>
    Utils::add(res, (2 << 16) | 248);
    Utils::add(res, Utils::getNewID());
    
  }

  template<SpurvShaderType type, typename... InputTypes>
  void SpurvShader<type, InputTypes...>::output_main_function_end(std::vector<uint32_t>& res) {
    // OpReturn
    Utils::add(res, (1 << 16) | 253);

    // OpFunctionEnd
    Utils::add(res, (1 << 16) | 56);
  }

  template<SpurvShaderType type, typename... InputTypes>
  template<typename tt, typename... NodeTypes>
  void SpurvShader<type, InputTypes...>::output_output_definitions(std::vector<uint32_t>& res, int n,
								   ValueNode<tt>& node, NodeTypes... args) {
    node.ensure_defined(res);

    // OpStore
    Utils::add(res, (3 << 16) | 62);
    Utils::add(res, this->output_pointer_ids[n]);
    Utils::add(res, node.getID());

    this->output_output_definitions(res, n + 1, args...);
  }

  template<SpurvShaderType type, typename... InputTypes>
  template<typename tt>
  void SpurvShader<type, InputTypes...>::output_output_definitions(std::vector<uint32_t>& res, int n,
								   ValueNode<tt>& node) {
    node.ensure_defined(res);

    // OpStore
    Utils::add(res, (3 << 16) | 62);
    Utils::add(res, this->output_pointer_ids[n]);
    Utils::add(res, node.getID());
  }
  

  template<SpurvShaderType type, typename... InputTypes>
  template<int n>
  void SpurvShader<type, InputTypes...>::output_input_pointers(std::vector<uint32_t>& res) {
    return;
  }
      
  template<SpurvShaderType type, typename... InputTypes>
  template<int n, typename CurrInput, typename... RestInput>
  void SpurvShader<type, InputTypes...>::output_input_pointers(std::vector<uint32_t>& res) {
    SpurvPointer<SPURV_STORAGE_INPUT, CurrInput>::ensure_defined(res, this->defined_type_declaration_states);

    // OpVariable ...
    Utils::add(res, (4 << 16) | 59);
    Utils::add(res, SpurvPointer<SPURV_STORAGE_INPUT, CurrInput>::getID());
    Utils::add(res, this->input_entries[n].pointer_id);
    Utils::add(res, SPURV_STORAGE_INPUT);
      
    this->output_input_pointers<n + 1, RestInput...>(res);
  }

  template<SpurvShaderType type, typename... InputTypes>
  template<typename tt, typename... NodeTypes>
  void SpurvShader<type, InputTypes...>::output_output_pointers(std::vector<uint32_t>& res, int n,
								ValueNode<tt>& val, NodeTypes... args) {
    SpurvPointer<SPURV_STORAGE_OUTPUT, tt>::ensure_defined(res, this->defined_type_declaration_states);

    // OpVariable...
    Utils::add(res, (4 << 16) | 59);
    Utils::add(res, SpurvPointer<SPURV_STORAGE_OUTPUT, tt>::id);
    Utils::add(res, this->output_pointer_ids[n]);
    Utils::add(res, SPURV_STORAGE_OUTPUT);

    this->output_output_pointers(res, n + 1, args...);
  }

  template<SpurvShaderType type, typename... InputTypes>
  template<typename tt>
  void SpurvShader<type, InputTypes...>::output_output_pointers(std::vector<uint32_t>& res, int n, ValueNode<tt>& val) {
    SpurvPointer<SPURV_STORAGE_OUTPUT, tt>::ensure_defined(res, this->defined_type_declaration_states);

    // OpVariable...
    Utils::add(res, (4 << 16) | 59);
    Utils::add(res, SpurvPointer<SPURV_STORAGE_OUTPUT, tt>::getID());
    Utils::add(res, this->output_pointer_ids[n]);
    Utils::add(res, SPURV_STORAGE_OUTPUT);
  }

  template<SpurvShaderType type, typename... InputTypes>
  void SpurvShader<type, InputTypes...>::output_uniform_pointers(std::vector<uint32_t>& res) {
    for(uint i = 0; i < this->uniform_bindings.size(); i++) {
      this->uniform_bindings[i]->definePointer(res, this->defined_type_declaration_states);
    }
  }

  template<SpurvShaderType type, typename... InputTypes>
  void SpurvShader<type, InputTypes...>::output_used_builtin_pointers(std::vector<uint32_t>& res){
    if(this->builtin_vec4_0) {
      SpurvPointer<SPURV_STORAGE_OUTPUT, vec4_s>::ensure_defined(res, this->defined_type_declaration_states);
      
      // OpVariable
      Utils::add(res, (4 << 16) | 59);
      Utils::add(res, SpurvPointer<SPURV_STORAGE_OUTPUT, vec4_s>::getID());
      Utils::add(res, this->builtin_vec4_0->pointer_id);
      Utils::add(res, SPURV_STORAGE_OUTPUT);
    }
    if(this->builtin_float_0) {
      SpurvPointer<SPURV_STORAGE_OUTPUT, float_s>::ensure_defined(res, this->defined_type_declaration_states);
      
      // OpVariable
      Utils::add(res, (4 << 16) | 59);
      Utils::add(res, SpurvPointer<SPURV_STORAGE_OUTPUT, float_s>::getID());
      Utils::add(res, this->builtin_float_0->pointer_id);
      Utils::add(res, SPURV_STORAGE_OUTPUT);
    }
    if(this->builtin_arr_1_float_0) {
      SpurvPointer<SPURV_STORAGE_OUTPUT, arr_1_float_s>::ensure_defined(res, this->defined_type_declaration_states);
      
      // OpVariable
      Utils::add(res, (4 << 16) | 59);
      Utils::add(res, SpurvPointer<SPURV_STORAGE_OUTPUT, arr_1_float_s>::getID());
      Utils::add(res, this->builtin_arr_1_float_0->pointer_id);
      Utils::add(res, SPURV_STORAGE_OUTPUT);
    }
    if(this->builtin_arr_1_float_1) {
      SpurvPointer<SPURV_STORAGE_OUTPUT, arr_1_float_s>::ensure_defined(res, this->defined_type_declaration_states);
      
      // OpVariable
      Utils::add(res, (4 << 16) | 59);
      Utils::add(res, SpurvPointer<SPURV_STORAGE_OUTPUT, arr_1_float_s>::getID());
      Utils::add(res, this->builtin_arr_1_float_1->pointer_id);
      Utils::add(res, SPURV_STORAGE_OUTPUT);
    }
  }
  
  template<SpurvShaderType type, typename... InputTypes>
  void SpurvShader<type, InputTypes...>::output_builtin_output_definitions(std::vector<uint32_t>& res) {
    if(this->builtin_vec4_0) {
      this->builtin_vec4_0->value_node->ensure_defined(res);
      
      // OpStore
      Utils::add(res, (3 << 16) | 62);
      Utils::add(res, this->builtin_vec4_0->pointer_id);
      Utils::add(res, this->builtin_vec4_0->value_node->getID());
      
    }
    if(this->builtin_float_0) {
      this->builtin_float_0->value_node->ensure_defined(res);
      
      // OpStore
      Utils::add(res, (3 << 16) | 62);
      Utils::add(res, this->builtin_float_0->pointer_id);
      Utils::add(res, this->builtin_float_0->value_node->getID());
    }
    if(this->builtin_arr_1_float_0) {
      this->builtin_arr_1_float_0->value_node->ensure_defined(res);
      
      // OpStore
      Utils::add(res, (3 << 16) | 62);
      Utils::add(res, this->builtin_arr_1_float_0->pointer_id);
      Utils::add(res, this->builtin_arr_1_float_0->value_node->getID());
    }
    if(this->builtin_arr_1_float_1) {
      this->builtin_arr_1_float_1->value_node->ensure_defined(res);
      
      // OpStore
      Utils::add(res, (3 << 16) | 62);
      Utils::add(res, this->builtin_arr_1_float_1->pointer_id);
      Utils::add(res, this->builtin_arr_1_float_1->value_node->getID());
    }
  }

  
  /*
   * Public member function
   */

  template<SpurvShaderType type, typename... InputTypes>
  template<BuiltinVariableIndex ind, typename tt>
  void SpurvShader<type, InputTypes...>::setBuiltinOutput(ValueNode<tt>& val) {
    if constexpr(type == SPURV_SHADER_VERTEX) {
	if constexpr(ind == BUILTIN_POSITION) {
        
	    static_assert(std::is_same<tt, vec4_s>::value, "Position must be vec4_s");
	    if(this->builtin_vec4_0) {
	      printf("Cannot set builtin multiple times");
	      exit(-1);
	    }
	    this->builtin_vec4_0 = new BuiltinEntry<vec4_s>;
	    this->builtin_vec4_0->value_node = &val;
	    this->builtin_vec4_0->pointer_id = Utils::getNewID();
	  } else if constexpr(ind == BUILTIN_POINT_SIZE) {
	    static_assert(std::is_same<tt, float_s>::value, "PointSize must be float_s");
	    
	    if(this->builtin_float_0) {
	      printf("Cannot set builtin multiple times");
	      exit(-1);
	    }
	    this->builtin_float_0 = new BuiltinEntry<float_s>;
	    this->builtin_float_0->value_node = &val;
	    this->builtin_float_0->pointer_id = Utils::getNewID();
	  } else if constexpr(ind == BUILTIN_CLIP_DISTANCE) {
	    static_assert(std::is_same<tt, arr_1_float_s>::value, "Clip Distance must be arr_1_float_s");
	    
	    if(this->builtin_arr_1_float_0) {
	      printf("Cannot set builtin multiple times");
	      exit(-1);
	    }
	    this->builtin_arr_1_float_0 = new BuiltinEntry<arr_1_float_s>;
	    this->builtin_arr_1_float_0->value_node = &val;
	    this->builtin_arr_1_float_0->pointer_id = Utils::getNewID();
	  } else if constexpr(ind == BUILTIN_CULL_DISTANCE) {
	    static_assert(std::is_same<tt, arr_1_float_s>::value, "Cull Distance must be arr_1_float_s");
	    
	    if(this->builtin_arr_1_float_1) {
	      printf("Cannot set builtin multiple times");
	      exit(-1);
	    }
	    this->builtin_arr_1_float_1 = new BuiltinEntry<arr_1_float_s>;
	    this->builtin_arr_1_float_1->value_node = &val;
	    this->builtin_arr_1_float_1->pointer_id = Utils::getNewID();
	  } else {
	  printf("This builtin has not yet been defined in setBuiltinOutput");
	  exit(-1);
	}
      } else {
      printf("This shader type is not yet supported in setBuiltinOutput\n");
      exit(-1);
    }
  }
  
  template<SpurvShaderType type, typename... InputTypes>
  template<int n>
  auto& SpurvShader<type, InputTypes...>::getInputVariable() {
    return this->getInputVariable<n, 0, InputTypes...>();
  }

  template<SpurvShaderType type, typename... InputTypes>
  template<int n, typename First>
   auto& SpurvShader<type, InputTypes...>::getInputVariable() {
    using input_type = First;
    static_assert(is_spurv_type<input_type>::value, "Supplied input values must be SpurvType");

    if(input_entries[n].ds.kind == SPURV_TYPE_INVALID) {
      
      input_type::getDSpurvType(&(input_entries[n].ds));
      input_entries[n].pointer_id = Utils::getNewID();
	
	
      InputVar<input_type> *iv = new InputVar<input_type>(n, input_entries[n].pointer_id);
	
      input_entries[n].id = iv->getID();
      input_entries[n].value_node = (void*)iv;
      return *iv;
    } else {
      return *(InputVar<input_type>*)input_entries[n].value_node; // Sketchy, but oh well
    }
  }
  
  template<SpurvShaderType type, typename... InputTypes>
  template<int n, int curr, typename First, typename... Rest>
  auto& SpurvShader<type, InputTypes...>::getInputVariable() {
    using input_type = First;
    static_assert(is_spurv_type<input_type>::value, "Supplied input values must be SpurvType");

    if constexpr(n > curr) {
	return getInputVariable<n, curr + 1, Rest...>();
      } else {
      return getInputVariable<n, First>();
    }
  }

  template<SpurvShaderType type, typename... InputTypes>
  template<typename... InnerTypes>
  SpurvUniformBinding<InnerTypes...>& SpurvShader<type, InputTypes...>::getUniformBinding(int set_no, int binding_no) {
    // O(n^2).. But hopefully nobody cares
    for(uint i = 0; i < this->uniform_bindings.size(); i++) {
      if(this->uniform_bindings[i]->getSetNo() == set_no &&
	 this->uniform_bindings[i]->getBindingNo() == binding_no) {
	return *(SpurvUniformBinding<InnerTypes...>*)uniform_bindings[i];
      }
    }

    SpurvUniformBinding<InnerTypes...>* pp = new SpurvUniformBinding<InnerTypes...>(set_no, binding_no);
    this->uniform_bindings.push_back((SpurvUniformBindingBase*)pp);
    return *pp;
  }

  template<SpurvShaderType type, typename... InputTypes>
  template<typename... NodeTypes>
  void SpurvShader<type, InputTypes...>::compileToSpirv(std::vector<uint32_t>& res, NodeTypes&... args) {

    this->output_preamble(res);
    this->output_shader_header_begin(res);
    
    constexpr int num_args = sizeof...(NodeTypes);
    
    for(int i = 0; i < num_args; i++) {
      this->output_pointer_ids.push_back(Utils::getNewID());
    }
    
    this->output_shader_header_output_variables(res, 0, args...);

    this->output_shader_header_end(res);

    res[this->entry_point_declaration_size_index] |= ( 3 + 2 + this->input_entries.size() + num_args + get_num_defined_builtins()) << 16;
    

    this->output_shader_header_decorate_begin(res);
    this->output_shader_header_decorate_output_variables(res, 0, args...);

    this->output_type_definitions(res, args...);

    this->output_input_pointers<0, InputTypes...>(res);
    this->output_output_pointers(res, 0, args...);
    this->output_uniform_pointers(res);
    this->output_used_builtin_pointers(res);

    this->output_main_function_begin(res);
        
    this->output_output_definitions(res, 0, args...);
    this->output_builtin_output_definitions(res);

    this->output_main_function_end(res);

    res[this->id_max_bound_index] = Utils::getCurrentID();

    this->cleanup_declaration_states();

    Utils::resetID();
    ConstantRegistry::resetRegistry();
  }
};

#endif // __SPURV_SHADER
