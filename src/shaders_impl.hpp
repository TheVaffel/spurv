#ifndef __SPURV_SHADERS_IMPL
#define __SPURV_SHADERS_IMPL

#include "shaders.hpp"

namespace spurv {
  
  
  /*
   * Constructor(s?)
   */

  template<SShaderType type, typename... InputTypes>
  SShader<type, InputTypes...>::SShader() {
    builtin_vec4_0 = nullptr;
    builtin_float_0 = nullptr;
    builtin_arr_1_float_0 = nullptr;
    builtin_arr_1_float_1 = nullptr;
    builtin_uint32_0 = nullptr;
    builtin_uint32_1 = nullptr;

    input_entries = std::vector<InputVariableEntry>(sizeof...(InputTypes)); 
  }
  
  
  /*
   * Member functions of inner classes
   */

  template<SShaderType type, typename... InputTypes>
  SShader<type, InputTypes...>::InputVariableEntry::InputVariableEntry() {
    this->id = -1;
  }


  /*
   * Util functions
   */

  template<SShaderType type, typename... InputTypes>
  int SShader<type, InputTypes...>::get_num_defined_builtins() {
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
    if(this->builtin_uint32_0) {
      num++;
    }
    if(this->builtin_uint32_1) {
      num++;
    }

    return num;
  }
  
  template<SShaderType type, typename...InputTypes>
  void SShader<type, InputTypes...>::cleanup_declaration_states() {
    for(unsigned int i = 0; i < this->defined_type_declaration_states.size(); i++) {
      this->defined_type_declaration_states[i]->is_defined  = false;
      this->defined_type_declaration_states[i]->id = -1;
    }
  }

  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::cleanup_decoration_states() {
    for(unsigned int i = 0; i < this->decoration_states.size(); i++) {
      *this->decoration_states[i] = false;
    }
  }
  
  
  /*
   * Output member functions - functions writing binaries to vector
   */
  
  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::output_shader_header_output_variables(std::vector<uint32_t>& binary,
									       int n) {
    return;
    } 
  
  template<SShaderType type, typename... InputTypes>
  template<typename in1, typename... NodeTypes>
  void SShader<type, InputTypes...>::output_shader_header_output_variables(std::vector<uint32_t>& binary,
									   int n, in1&& arg0, NodeTypes&&... args) {

    // Just double check that this is an actual useful type

    using t_in1 = typename std::remove_reference<in1>::type;
    using inner_type = typename SValueWrapper::unwrapped_type<t_in1>::type;
    static_assert(SValueWrapper::does_wrap<t_in1, inner_type>::value);
    binary.push_back(this->output_pointer_ids[n]);

    if constexpr(sizeof...(NodeTypes) > 0) {
	output_shader_header_output_variables(binary, n + 1, args...);
      }
  }

  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::output_shader_header_decorate_output_variables(std::vector<uint32_t>& bin, int n) {
    return;
  }									 
  
  template<SShaderType type, typename... InputTypes>
  template<typename in1, typename... NodeTypes>
  void SShader<type, InputTypes...>::output_shader_header_decorate_output_variables(std::vector<uint32_t>& bin, int n,
									 in1&& arg0, NodeTypes&&... args) {
    bin.push_back((4 << 16) | 71);
    bin.push_back(this->output_pointer_ids[n]);
    bin.push_back(30);
    bin.push_back(n);

    output_shader_header_decorate_output_variables(bin, n + 1, args...); 
  }


  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::output_shader_header_decorate_tree(std::vector<uint32_t>& bin) {
    return;
  }
  
  template<SShaderType type, typename... InputTypes>
  template<typename in1, typename... NodeTypes>
  void SShader<type, InputTypes...>::output_shader_header_decorate_tree(std::vector<uint32_t>& bin,
									in1&& arg0, NodeTypes&&... args) {
    using t_in1 = typename std::remove_reference<in1>::type;
    if constexpr(is_spurv_value<t_in1>::value) {
	arg0.ensure_type_decorated(bin,
				   this->decoration_states);
      }
  }
  

  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::output_used_builtin_ids(std::vector<uint32_t>& bin) {
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
    if(this->builtin_uint32_0) {
      bin.push_back(this->builtin_uint32_0->pointer_id);
    }
    if(this->builtin_uint32_1) {
      bin.push_back(this->builtin_uint32_1->pointer_id);
    }
  }

  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::output_builtin_tree_type_definitions(std::vector<uint32_t>& bin) {
    if(this->builtin_vec4_0) {
      this->builtin_vec4_0->value_node->ensure_type_defined(bin, this->defined_type_declaration_states);
    }
    if(this->builtin_float_0) {
      this->builtin_float_0->value_node->ensure_type_defined(bin, this->defined_type_declaration_states);
    }
    if(this->builtin_arr_1_float_0) {
      this->builtin_arr_1_float_0->value_node->ensure_type_defined(bin, this->defined_type_declaration_states);
    }
    if(this->builtin_arr_1_float_1) {
      this->builtin_arr_1_float_1->value_node->ensure_type_defined(bin, this->defined_type_declaration_states);
    }
    if(this->builtin_uint32_0) {
      this->builtin_uint32_0->value_node->ensure_type_defined(bin, this->defined_type_declaration_states);
    }
    if(this->builtin_uint32_1) {
      this->builtin_uint32_1->value_node->ensure_type_defined(bin, this->defined_type_declaration_states);
    }
  }

  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::output_preamble(std::vector<uint32_t>& binary) {
    binary.push_back(0x07230203); // Magic number
    binary.push_back(0x00010000); // Version number (1.0.0)
    binary.push_back(0x124);      // Generator's magic number (not officially registered)
    this->id_max_bound_index = binary.size();
    binary.push_back(0); // We'll set this later
    binary.push_back(0x0); // For instruction schema (whatever that means)
  }

  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::output_output_tree_type_definitions(std::vector<uint32_t>& bin) {
    return;
  }

  
  template<SShaderType type, typename... InputTypes>
  template<typename in1, typename... NodeTypes>
  void SShader<type, InputTypes...>::output_output_tree_type_definitions(std::vector<uint32_t>& bin,
									 SValue<in1>& val,
									 NodeTypes&&... args) {
    val.ensure_type_defined(bin, this->defined_type_declaration_states);

    this->output_output_tree_type_definitions(bin, args...);
  }

  
  static const std::string entry_point_name = "main";

  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::output_shader_header_begin(std::vector<uint32_t>& bin) {
    // capability Shader
    SUtils::add(bin, (2 << 16) | 17);
    SUtils::add(bin, 1);

    for(SExtension ext : this->extensions) {
      // OpExtensions
      int num_words = SUtils::stringWordLength(shaderExtensions[ext]);
      SUtils::add(bin, ((1 + num_words) << 16) | 10);
      SUtils::add(bin, shaderExtensions[ext]);
    }

    // GLSL = ext_inst_import "GLSL.std.450"
    std::string glsl_import_str = "GLSL.std.450";
    int length = SUtils::stringWordLength(glsl_import_str);
    SUtils::add(bin, ((2 + length) << 16) | 11);
    
    this->glsl_id = SUtils::getNewID();
    SUtils::add(bin, this->glsl_id);
    SUtils::add(bin, glsl_import_str);

    // memory_model Logical GLSL450
    SUtils::add(bin, (3 << 16) | 14);
    SUtils::add(bin, 0);
    SUtils::add(bin, 1);
    
    // entry_point vertex_shader main "main" input/output_variables
    this->entry_point_declaration_size_index = bin.size(); // Add size afterwards
    SUtils::add(bin, 15);

    if constexpr(type == SShaderType::SHADER_VERTEX) {
	SUtils::add(bin, 0); // Vertex
      } else if constexpr(type == SShaderType::SHADER_FRAGMENT) {
	SUtils::add(bin, 4); // Fragment
      } else {
      printf("Shader type not yet accounted for in output_shader_header_begin\n");
      exit(-1);
    }

    this->entry_point_id = SUtils::getNewID();
    SUtils::add(bin, this->entry_point_id);
    SUtils::add(bin, entry_point_name);

    for(unsigned int i = 0; i < this->input_entries.size(); i++) {
      if (this->input_entries[i].id == -1) {
	printf("Input variable not initialized");
	exit(-1);
      }
      SUtils::add(bin, this->input_entries[i].pointer_id);
    }

    output_used_builtin_ids(bin);

  }

  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::output_shader_header_end(std::vector<uint32_t>& bin) {

    if constexpr(type == SShaderType::SHADER_FRAGMENT) {
	// OpExecutionMode <entry_point_id> OriginUpperLeft
	SUtils::add(bin, (3 << 16) | 16);
	SUtils::add(bin, this->entry_point_id);
	SUtils::add(bin, 7);
      }

    
    int strl = SUtils::stringWordLength(entry_point_name);
    
    // OpName <main_id> "main"
    SUtils::add(bin, ((2 + strl) << 16) | 5);
    SUtils::add(bin, this->entry_point_id);
    SUtils::add(bin, entry_point_name);
    
  }
  
  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::output_shader_header_decorate_begin(std::vector<uint32_t>& bin) {
    
    if constexpr(type == SShaderType::SHADER_VERTEX) {
	if(this->builtin_vec4_0) {
	  // Decorate <builtin> Builtin Position
	  SUtils::add(bin, (4 << 16) | 71);
	  SUtils::add(bin, this->builtin_vec4_0->pointer_id);
	  SUtils::add(bin, 11);
	  SUtils::add(bin, 0);
	}

	// Decorate <builtin> Builtin PointSize
	if(this->builtin_float_0) {
	  SUtils::add(bin, (4 << 16) | 71);
	  SUtils::add(bin, this->builtin_float_0->pointer_id);
	  SUtils::add(bin, 11);
	  SUtils::add(bin, 1);
	}

	// Clip Distance
	if(this->builtin_arr_1_float_0) {
	  SUtils::add(bin, (4 << 16) | 71);
	  SUtils::add(bin, this->builtin_arr_1_float_0->pointer_id);
	  SUtils::add(bin, 11);
	  SUtils::add(bin, 3);
	}

	// Cull Distance
	if(this->builtin_arr_1_float_1) {
	  SUtils::add(bin, (4 << 16) | 71);
	  SUtils::add(bin, this->builtin_arr_1_float_1->pointer_id);
	  SUtils::add(bin, 11);
	  SUtils::add(bin, 4);
	}
	
	// Instance Index
	if(this->builtin_uint32_0) {
	  SUtils::add(bin, (4 << 16) | 71);
	  SUtils::add(bin, this->builtin_uint32_0->pointer_id);
	  SUtils::add(bin, 11);
	  SUtils::add(bin, 43); // InstanceIndex
	}
	
	// Vertex Index
	if(this->builtin_uint32_1) {
	  SUtils::add(bin, (4 << 16) | 71);
	  SUtils::add(bin, this->builtin_uint32_1->pointer_id);
	  SUtils::add(bin, 11);
	  SUtils::add(bin, 42); // VertexIndex
	}


      } else if constexpr(type == SShaderType::SHADER_FRAGMENT) {
	if(this->builtin_vec4_0) {
	  // Frag Coord
	  SUtils::add(bin, (4 << 16) | 71);
	  SUtils::add(bin, this->builtin_vec4_0->pointer_id);
	  SUtils::add(bin, 11);
	  SUtils::add(bin, 15); // FragCoord
	}
      } else {
	if(this->builtin_vec4_0 || this->builtin_float_0 ||
	   this->builtin_arr_1_float_0 || this->builtin_arr_1_float_1 ||
	   this->builtin_uint32_0 || this->builtin_uint32_1) {
	  printf("Builtin variables for other than Vertex and Fragment shader must still be implemented\n");
	  exit(-1);
	}
    }
    
    // Decorate <input_var> Location <index>
    for(unsigned int i = 0; i < input_entries.size(); i++) {
      SUtils::add(bin, (4 << 16) | 71);
      SUtils::add(bin, input_entries[i].pointer_id);
      SUtils::add(bin, 30);
      SUtils::add(bin, i);
    }

    // Put decorations on uniforms
    for(unsigned int i = 0; i < uniform_bindings.size(); i++) {

      // This could be handled through the recursive tree decoration
      // but it's safer to do it here as well
      uniform_bindings[i]->decorateType(bin,
					this->decoration_states); 

      // Decorate <uniform_binding> DescriptorSet <num>
      SUtils::add(bin, (4 << 16) | 71);
      SUtils::add(bin, uniform_bindings[i]->getPointerID());
      SUtils::add(bin, 34);
      SUtils::add(bin, uniform_bindings[i]->getSetNo());

      // Decorate <uniform_binding> Binding <num>
      SUtils::add(bin, (4 << 16) | 71);
      SUtils::add(bin, uniform_bindings[i]->getPointerID());
      SUtils::add(bin, 33);
      SUtils::add(bin, uniform_bindings[i]->getBindingNo());

    }
  }
  

  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::output_main_function_begin(std::vector<uint32_t>& res) {
    SType<STypeKind::KIND_VOID>::ensure_defined(res, this->defined_type_declaration_states);

    int void_function_type = SUtils::getNewID();
    
    // OpTypeFunction <result_id> <result type> <result_id> 
    SUtils::add(res, (3 << 16) | 33);
    SUtils::add(res, void_function_type);
    SUtils::add(res, SType<STypeKind::KIND_VOID>::getID());

    // OpFunction <result type> <result_id> <function_control> <function_type>
    SUtils::add(res, (5 << 16) | 54);
    SUtils::add(res, SType<STypeKind::KIND_VOID>::getID());
    SUtils::add(res, entry_point_id);
    SUtils::add(res, 0);
    SUtils::add(res, void_function_type);

    // OpLabel <result_id>
    SUtils::add(res, (2 << 16) | 248);
    SUtils::add(res, SUtils::getNewID());
    
  }

  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::output_main_function_end(std::vector<uint32_t>& res) {
    // OpReturn
    SUtils::add(res, (1 << 16) | 253);

    // OpFunctionEnd
    SUtils::add(res, (1 << 16) | 56);
  }

  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::output_output_definitions(std::vector<uint32_t>& res, int n) {
    return;
  }
  
  template<SShaderType type, typename... InputTypes>
  template<typename in1, typename... NodeTypes>
  void SShader<type, InputTypes...>::output_output_definitions(std::vector<uint32_t>& res, int n,
							       in1&& node, NodeTypes&&... args) {
    node.ensure_defined(res);

    // OpStore
    SUtils::add(res, (3 << 16) | 62);
    SUtils::add(res, this->output_pointer_ids[n]);
    SUtils::add(res, node.getID());

    this->output_output_definitions(res, n + 1, args...);
  }
  

  template<SShaderType type, typename... InputTypes>
  template<int n>
  void SShader<type, InputTypes...>::output_input_pointers(std::vector<uint32_t>& res) {
    return;
  }
      
  template<SShaderType type, typename... InputTypes>
  template<int n, typename CurrInput, typename... RestInput>
  void SShader<type, InputTypes...>::output_input_pointers(std::vector<uint32_t>& res) {
    SPointer<STORAGE_INPUT, CurrInput>::ensure_defined(res, this->defined_type_declaration_states);

    // OpVariable ...
    SUtils::add(res, (4 << 16) | 59);
    SUtils::add(res, SPointer<STORAGE_INPUT, CurrInput>::getID());
    SUtils::add(res, this->input_entries[n].pointer_id);
    SUtils::add(res, STORAGE_INPUT);
      
    this->output_input_pointers<n + 1, RestInput...>(res);
  }

  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::output_output_pointers(std::vector<uint32_t>& res, int n) {
    return;
  }
  
  template<SShaderType type, typename... InputTypes>
  template<typename in1, typename... NodeTypes>
  void SShader<type, InputTypes...>::output_output_pointers(std::vector<uint32_t>& res, int n,
							    in1&& val, NodeTypes&&... args) {

    using t_in1 = typename std::remove_reference<in1>::type;
    using inner_type = typename SValueWrapper::unwrapped_type<t_in1>::type;
    
    SPointer<STORAGE_OUTPUT, inner_type>::ensure_defined(res, this->defined_type_declaration_states);

    // OpVariable...
    SUtils::add(res, (4 << 16) | 59);
    SUtils::add(res, SPointer<STORAGE_OUTPUT, inner_type>::getID());
    SUtils::add(res, this->output_pointer_ids[n]);
    SUtils::add(res, STORAGE_OUTPUT);

    this->output_output_pointers(res, n + 1, args...);
  }

  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::output_uniform_pointers(std::vector<uint32_t>& res) {
    for(unsigned int i = 0; i < this->uniform_bindings.size(); i++) {
      this->uniform_bindings[i]->definePointer(res, this->defined_type_declaration_states);
    }
  }

  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::output_used_builtin_pointers(std::vector<uint32_t>& res){
    if constexpr(type == SShaderType::SHADER_VERTEX) {
	if(this->builtin_vec4_0) {
	  SPointer<STORAGE_OUTPUT, vec4_s>::ensure_defined(res, this->defined_type_declaration_states);
      
	  // OpVariable
	  SUtils::add(res, (4 << 16) | 59);
	  SUtils::add(res, SPointer<STORAGE_OUTPUT, vec4_s>::getID());
	  SUtils::add(res, this->builtin_vec4_0->pointer_id);
	  SUtils::add(res, STORAGE_OUTPUT);
	}
	if(this->builtin_float_0) {
	  SPointer<STORAGE_OUTPUT, float_s>::ensure_defined(res, this->defined_type_declaration_states);
      
	  // OpVariable
	  SUtils::add(res, (4 << 16) | 59);
	  SUtils::add(res, SPointer<STORAGE_OUTPUT, float_s>::getID());
	  SUtils::add(res, this->builtin_float_0->pointer_id);
	  SUtils::add(res, STORAGE_OUTPUT);
	}
	if(this->builtin_arr_1_float_0) {
	  SPointer<STORAGE_OUTPUT, arr_1_float_s>::ensure_defined(res, this->defined_type_declaration_states);
      
	  // OpVariable
	  SUtils::add(res, (4 << 16) | 59);
	  SUtils::add(res, SPointer<STORAGE_OUTPUT, arr_1_float_s>::getID());
	  SUtils::add(res, this->builtin_arr_1_float_0->pointer_id);
	  SUtils::add(res, STORAGE_OUTPUT);
	}
	if(this->builtin_arr_1_float_1) {
	  SPointer<STORAGE_OUTPUT, arr_1_float_s>::ensure_defined(res, this->defined_type_declaration_states);
      
	  // OpVariable
	  SUtils::add(res, (4 << 16) | 59);
	  SUtils::add(res, SPointer<STORAGE_OUTPUT, arr_1_float_s>::getID());
	  SUtils::add(res, this->builtin_arr_1_float_1->pointer_id);
	  SUtils::add(res, STORAGE_OUTPUT);
	}
	if(this->builtin_uint32_0) {
	  SPointer<STORAGE_INPUT, uint32_s>::ensure_defined(res, this->defined_type_declaration_states);

	  // OpVariable
	  SUtils::add(res, (4 << 16) | 59);
	  SUtils::add(res, SPointer<STORAGE_INPUT, uint32_s>::getID());
	  SUtils::add(res, this->builtin_uint32_0->pointer_id);
	  SUtils::add(res, STORAGE_INPUT);
	}
	if(this->builtin_uint32_1) {
	  SPointer<STORAGE_INPUT, uint32_s>::ensure_defined(res, this->defined_type_declaration_states);

	  // OpVariable
	  SUtils::add(res, (4 << 16) | 59);
	  SUtils::add(res, SPointer<STORAGE_INPUT, uint32_s>::getID());
	  SUtils::add(res, this->builtin_uint32_1->pointer_id);
	  SUtils::add(res, STORAGE_INPUT);
	}
      } else if constexpr (type == SShaderType::SHADER_FRAGMENT) {
	if(this->builtin_vec4_0) {
	  SPointer<STORAGE_INPUT, vec4_s>::ensure_defined(res, this->defined_type_declaration_states);

	  // OpVariable
	  SUtils::add(res, (4 << 16) | 59);
	  SUtils::add(res, SPointer<STORAGE_INPUT, vec4_s>::getID());
	  SUtils::add(res, this->builtin_vec4_0->pointer_id);
	  SUtils::add(res, STORAGE_INPUT);
	}
      }
  }
  
  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::output_builtin_output_definitions(std::vector<uint32_t>& res) {
    if constexpr(type == SShaderType::SHADER_VERTEX) {
	if(this->builtin_vec4_0) {
	  this->builtin_vec4_0->value_node->ensure_defined(res);
      
	  // OpStore
	  SUtils::add(res, (3 << 16) | 62);
	  SUtils::add(res, this->builtin_vec4_0->pointer_id);
	  SUtils::add(res, this->builtin_vec4_0->value_node->getID());
      
	}
	if(this->builtin_float_0) {
	  this->builtin_float_0->value_node->ensure_defined(res);
      
	  // OpStore
	  SUtils::add(res, (3 << 16) | 62);
	  SUtils::add(res, this->builtin_float_0->pointer_id);
	  SUtils::add(res, this->builtin_float_0->value_node->getID());
	}
	if(this->builtin_arr_1_float_0) {
	  this->builtin_arr_1_float_0->value_node->ensure_defined(res);
      
	  // OpStore
	  SUtils::add(res, (3 << 16) | 62);
	  SUtils::add(res, this->builtin_arr_1_float_0->pointer_id);
	  SUtils::add(res, this->builtin_arr_1_float_0->value_node->getID());
	}
	if(this->builtin_arr_1_float_1) {
	  this->builtin_arr_1_float_1->value_node->ensure_defined(res);
      
	  // OpStore
	  SUtils::add(res, (3 << 16) | 62);
	  SUtils::add(res, this->builtin_arr_1_float_1->pointer_id);
	  SUtils::add(res, this->builtin_arr_1_float_1->value_node->getID());
	}
      }
  }

  
  /*
   * Public member function
   */

  template<SShaderType type, typename... InputTypes>
  template<SBuiltinVariable ind>
  SValue< typename BuiltinToType<ind>::type >& SShader<type, InputTypes...>::getBuiltin() {
    using tt = typename BuiltinToType<ind>::type;
    
    if constexpr(type == SShaderType::SHADER_VERTEX) {
	if constexpr(ind == BUILTIN_INSTANCE_INDEX) {
	    static_assert(std::is_same<tt, uint32_s>::value, "Vertex shader instance ID must be unsigned 32-bit integer");

	    if(!this->builtin_uint32_0) {
	      int pointer_id = SUtils::getNewID();
	      this->builtin_uint32_0 = SUtils::allocate<BuiltinEntry<uint32_s> >();
	      this->builtin_uint32_0->value_node = SUtils::allocate<SPointerVar<uint32_s, STORAGE_INPUT> >(pointer_id);
	      this->builtin_uint32_0->pointer_id = pointer_id;
	    }
	    return *this->builtin_uint32_0->value_node;
	    
	  } else if constexpr(ind == BUILTIN_VERTEX_INDEX) {
	    
	    static_assert(std::is_same<tt, uint32_s>::value, "Vertex shader vertex ID must be unsigned 32-bit integer");
	    if(!this->builtin_uint32_1) {
	      int pointer_id = SUtils::getNewID();
	      this->builtin_uint32_1 = SUtils::allocate<BuiltinEntry<uint32_s> >();
	      this->builtin_uint32_1->value_node = SUtils::allocate<SPointerVar<uint32_s, STORAGE_INPUT> >(pointer_id);
	      this->builtin_uint32_1->pointer_id = pointer_id;
	    }
	    return *this->builtin_uint32_1->value_node;
	  } else {
	  printf("Builtin does not support read in vertex shader\n");
	  exit(-1);
	}
      } else if constexpr(type == SShaderType::SHADER_FRAGMENT) {
	if constexpr(ind == BUILTIN_FRAG_COORD) {
	    static_assert(std::is_same<tt, vec4_s>::value, "Fragment shader fragment coordinate must have type vec4");
	    
	    if(!this->builtin_vec4_0) {
	      int pointer_id = SUtils::getNewID();
	      this->builtin_vec4_0 = SUtils::allocate<BuiltinEntry<vec4_s> >();
	      this->builtin_vec4_0->value_node = SUtils::allocate<SPointerVar<vec4_s, STORAGE_INPUT> >(pointer_id);
	      this->builtin_vec4_0->pointer_id = pointer_id;
	    }
	    return *this->builtin_vec4_0->value_node;
	  } else {
	  printf("Builtin does not support read in fragment shader\n");
	  exit(-1);	
	}
      }
  }

  template<SShaderType type, typename... InputTypes>
  template<SBuiltinVariable ind, typename tt>
  void SShader<type, InputTypes...>::setBuiltin(SValue<tt>& val) {
    static_assert(std::is_same<tt, typename BuiltinToType<ind>::type>::value,
		  "[spurv] Compilation error: Mismatch type in builtin set");
    
    if constexpr(type == SShaderType::SHADER_VERTEX) {
	if constexpr(ind == BUILTIN_POSITION) {
        
	    if(this->builtin_vec4_0) {
	      printf("Cannot set builtin multiple times");
	      exit(-1);
	    }
	    this->builtin_vec4_0 = SUtils::allocate<BuiltinEntry<vec4_s> >();
	    this->builtin_vec4_0->value_node = &val;
	    this->builtin_vec4_0->pointer_id = SUtils::getNewID();
	    
	  } else if constexpr(ind == BUILTIN_POINT_SIZE) {
	    
	    if(this->builtin_float_0) {
	      printf("Cannot set builtin multiple times");
	      exit(-1);
	    }
	    this->builtin_float_0 = SUtils::allocate< BuiltinEntry<float_s> >();
	    this->builtin_float_0->value_node = &val;
	    this->builtin_float_0->pointer_id = SUtils::getNewID();
	    
	  } else if constexpr(ind == BUILTIN_CLIP_DISTANCE) {
	    
	    if(this->builtin_arr_1_float_0) {
	      printf("Cannot set builtin multiple times");
	      exit(-1);
	    }
	    this->builtin_arr_1_float_0 = SUtils::allocate<BuiltinEntry<arr_1_float_s> >();
	    this->builtin_arr_1_float_0->value_node = &val;
	    this->builtin_arr_1_float_0->pointer_id = SUtils::getNewID();
	    
	  } else if constexpr(ind == BUILTIN_CULL_DISTANCE) {
	    
	    if(this->builtin_arr_1_float_1) {
	      printf("Cannot set builtin multiple times");
	      exit(-1);
	    }
	    this->builtin_arr_1_float_1 = SUtils::allocate<BuiltinEntry<arr_1_float_s> >();
	    this->builtin_arr_1_float_1->value_node = &val;
	    this->builtin_arr_1_float_1->pointer_id = SUtils::getNewID();
	  } else {
	  printf("This builtin has not yet been defined in setBuiltinOutput");
	  exit(-1);
	}
      } else {
      printf("This shader type is not yet supported in setBuiltinOutput\n");
      exit(-1);
    }
  }
  
  template<SShaderType type, typename... InputTypes>
  template<int n>
  auto& SShader<type, InputTypes...>::input() {
    return this->input<n, 0, InputTypes...>();
  }

  template<SShaderType type, typename... InputTypes>
  template<int n, typename First>
   auto& SShader<type, InputTypes...>::input() {
    using input_type = First;
    static_assert(is_spurv_type<input_type>::value, "Supplied input values must be SType");

    if(input_entries[n].ds.kind == STypeKind::KIND_INVALID) {
      
      input_type::getDSType(&(input_entries[n].ds));
      input_entries[n].pointer_id = SUtils::getNewID();
	

      int nn = n;
      InputVar<input_type> *iv = SUtils::allocate<InputVar<input_type> >(nn, input_entries[n].pointer_id);
	
      input_entries[n].id = iv->getID();
      input_entries[n].value_node = (void*)iv;
      return *iv;
    } else {
      return *(InputVar<input_type>*)input_entries[n].value_node; // Sketchy, but oh well
    }
  }
  
  template<SShaderType type, typename... InputTypes>
  template<int n, int curr, typename First, typename... Rest>
  auto& SShader<type, InputTypes...>::input() {
    using input_type = First;
    static_assert(is_spurv_type<input_type>::value, "Supplied input values must be SType");

    if constexpr(n > curr) {
	return input<n, curr + 1, Rest...>();
      } else {
      return input<n, First>();
    }
  }

  template<SShaderType type, typename... InputTypes>
  SUniformBindingBase* SShader<type, InputTypes...>::find_binding(int set, int binding) {
    for(unsigned int i = 0; i < this->uniform_bindings.size(); i++) {
      if(this->uniform_bindings[i]->getSetNo() == set &&
	 this->uniform_bindings[i]->getBindingNo() == binding) {
	return uniform_bindings[i];
      }
    }

    return nullptr;
  }

  template<SShaderType type, typename... InputTypes>
  template<typename BindingType>
  BindingType& SShader<type, InputTypes...>::construct_binding(int set, int binding) {
    SUniformBindingBase* sb = find_binding(set, binding);
    
    if(sb != nullptr) {
      return *(BindingType*)sb;
    }

    BindingType* pp = SUtils::allocate<BindingType>(set, binding);
    this->uniform_bindings.push_back((SUniformBindingBase*)pp);
    return *pp;
  }

  template<SShaderType type, typename... InputTypes>
  template<typename tt>
  SValue<tt>& SShader<type, InputTypes...>::uniformConstant(int set_no, int binding_no) {
    return construct_binding<SUniformConstant<tt> >(set_no, binding_no);
  }

  template<SShaderType type, typename... InputTypes>
  template<typename... InnerTypes>
  SUniformBinding<InnerTypes...>&
  SShader<type, InputTypes...>::uniformBinding(int set_no, int binding_no) {
    return construct_binding<SUniformBinding<InnerTypes...> >(set_no, binding_no);
  }

  template<SShaderType type, typename... InputTypes>
  template<typename... InnerTypes>
  SStorageBuffer<InnerTypes...>&
  SShader<type, InputTypes...>::storageBuffer(int set_no, int binding_no) {
    this->extensions.insert(SExtension::EXTENSION_STORAGE_BUFFER);
    
    return construct_binding<SStorageBuffer<InnerTypes...> >(set_no, binding_no);
  }

  template<SShaderType type, typename... InputTypes>
  template<typename tt>
  SLocal<tt>& SShader<type, InputTypes...>::local() {
    return *SUtils::allocate<SLocal<tt> >();
  }

  template<SShaderType type, typename... InputTypes>
  SValue<int_s>& SShader<type, InputTypes...>::forLoop(int arg0, int arg1) {
    SForLoop* fl = SUtils::allocate<SForLoop>(arg0, arg1);
    this->loop_stack.push(fl);
    SEventRegistry::addForBegin(fl);

    return *fl->iterator_val;
  }

  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::endLoop() {
    SForLoop* fl = this->loop_stack.top();
    this->loop_stack.pop();
    SEventRegistry::addForEnd(fl);
  }

  template<SShaderType type, typename... InputTypes>
  template<typename... NodeTypes>
  void SShader<type, InputTypes...>::compile(std::vector<uint32_t>& res, NodeTypes&&... args) {

    this->output_preamble(res);
    this->output_shader_header_begin(res);
    
    constexpr int num_args = sizeof...(NodeTypes);
    
    for(int i = 0; i < num_args; i++) {
      this->output_pointer_ids.push_back(SUtils::getNewID());
    }
    
    this->output_shader_header_output_variables(res, 0, args...);

    this->output_shader_header_end(res);

    res[this->entry_point_declaration_size_index] |= ( 3 + 2 + this->input_entries.size() + num_args + get_num_defined_builtins()) << 16;
    

    this->output_shader_header_decorate_begin(res);
    this->output_shader_header_decorate_output_variables(res, 0, args...);
    this->output_shader_header_decorate_tree(res, args...);

    SEventRegistry::write_type_definitions(res,
					   this->defined_type_declaration_states);
    this->output_output_tree_type_definitions(res, args...);
    this->output_builtin_tree_type_definitions(res);
    
    this->output_input_pointers<0, InputTypes...>(res);
    this->output_output_pointers(res, 0, args...);
    this->output_uniform_pointers(res);
    this->output_used_builtin_pointers(res);

    this->output_main_function_begin(res);

    SVariableRegistry::write_variable_definitions(res);

    SEventRegistry::write_events(res);
    
    this->output_output_definitions(res, 0, args...);
    this->output_builtin_output_definitions(res);

    this->output_main_function_end(res);

    res[this->id_max_bound_index] = SUtils::getCurrentID();

    this->cleanup_declaration_states();
    this->cleanup_decoration_states();
    
    SUtils::resetID();
    SUtils::clearAllocations();
    SConstantRegistry::resetRegistry();
    SEventRegistry::clear();
    SVariableRegistry::clear();
  }
};
#endif // __SPURV_SHADERS_IMPL
