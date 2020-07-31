#ifndef __SPURV_SHADERS_IMPL
#define __SPURV_SHADERS_IMPL

#include "shaders.hpp"

namespace spurv {
  
  
  /*
   * Constructor(s?)
   */

  template<SShaderType type, typename... InputTypes>
  SShader<type, InputTypes...>::SShader() {
    builtin_vec4_out_0 = nullptr;
    builtin_float_out_0 = nullptr;
    builtin_arr_1_float_out_0 = nullptr;
    builtin_arr_1_float_out_1 = nullptr;
    builtin_uint32_in_0 = nullptr;
    builtin_uint32_in_1 = nullptr;
    builtin_vec4_in_0 = nullptr;

    input_entries = std::vector<InputVariableBase*>(sizeof...(InputTypes), nullptr); 
  }


  /*
   * Util functions
   */

  template<SShaderType type, typename... InputTypes>
  int SShader<type, InputTypes...>::get_num_defined_builtins() {
    int num = 0;
    if(this->builtin_vec4_out_0) {
      num++;
    }
    if(this->builtin_float_out_0) {
      num++;
    }
    if(this->builtin_arr_1_float_out_0) {
      num++;
    }
    if(this->builtin_arr_1_float_out_1) {
      num++;
    }
    if(this->builtin_uint32_in_0) {
      num++;
    }
    if(this->builtin_uint32_in_1) {
      num++;
    }
    if(this->builtin_vec4_in_0) {
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
    if(this->builtin_vec4_out_0) {
      bin.push_back(this->builtin_vec4_out_0->getPointerID());
    }
    if(this->builtin_float_out_0) {
      bin.push_back(this->builtin_float_out_0->getPointerID());
    }
    if(this->builtin_arr_1_float_out_0) {
      bin.push_back(this->builtin_arr_1_float_out_0->getPointerID());
    }
    if(this->builtin_arr_1_float_out_1) {
      bin.push_back(this->builtin_arr_1_float_out_1->getPointerID());
    }
    if(this->builtin_uint32_in_0) {
      bin.push_back(this->builtin_uint32_in_0->getPointerID());
    }
    if(this->builtin_uint32_in_1) {
      bin.push_back(this->builtin_uint32_in_1->getPointerID());
    }
    if(this->builtin_vec4_in_0) {
      bin.push_back(this->builtin_vec4_in_0->getPointerID());
    }
  }

  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::output_builtin_tree_type_definitions(std::vector<uint32_t>& bin) {
    if(this->builtin_vec4_out_0) {
      this->builtin_vec4_out_0->pointer->ensure_type_defined(bin, this->defined_type_declaration_states);
    }
    if(this->builtin_float_out_0) {
      this->builtin_float_out_0->pointer->ensure_type_defined(bin, this->defined_type_declaration_states);
    }
    if(this->builtin_arr_1_float_out_0) {
      this->builtin_arr_1_float_out_0->pointer->ensure_type_defined(bin, this->defined_type_declaration_states);
    }
    if(this->builtin_arr_1_float_out_1) {
      this->builtin_arr_1_float_out_1->pointer->ensure_type_defined(bin, this->defined_type_declaration_states);
    }
    if(this->builtin_uint32_in_0) {
      this->builtin_uint32_in_0->pointer->ensure_type_defined(bin, this->defined_type_declaration_states);
    }
    if(this->builtin_uint32_in_1) {
      this->builtin_uint32_in_1->pointer->ensure_type_defined(bin, this->defined_type_declaration_states);
    }
    if(this->builtin_vec4_in_0) {
      this->builtin_vec4_in_0->pointer->ensure_type_defined(bin, this->defined_type_declaration_states);
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
    SUtils::setGLSLID(this->glsl_id);
    
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
      if (this->input_entries[i]->getPointerID() == -1) {
	printf("Input variable not initialized");
	exit(-1);
      }
      SUtils::add(bin, this->input_entries[i]->getPointerID());
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
	if(this->builtin_vec4_out_0) {
	  // Decorate <builtin> Builtin Position
	  SUtils::add(bin, (4 << 16) | 71);
	  SUtils::add(bin, this->builtin_vec4_out_0->getPointerID());
	  SUtils::add(bin, 11);
	  SUtils::add(bin, 0);
	}

	// Decorate <builtin> Builtin PointSize
	if(this->builtin_float_out_0) {
	  SUtils::add(bin, (4 << 16) | 71);
	  SUtils::add(bin, this->builtin_float_out_0->getPointerID());
	  SUtils::add(bin, 11);
	  SUtils::add(bin, 1);
	}

	// Clip Distance
	if(this->builtin_arr_1_float_out_0) {
	  SUtils::add(bin, (4 << 16) | 71);
	  SUtils::add(bin, this->builtin_arr_1_float_out_0->getPointerID());
	  SUtils::add(bin, 11);
	  SUtils::add(bin, 3);
	}

	// Cull Distance
	if(this->builtin_arr_1_float_out_1) {
	  SUtils::add(bin, (4 << 16) | 71);
	  SUtils::add(bin, this->builtin_arr_1_float_out_1->getPointerID());
	  SUtils::add(bin, 11);
	  SUtils::add(bin, 4);
	}
	
	// Instance Index
	if(this->builtin_uint32_in_0) {
	  SUtils::add(bin, (4 << 16) | 71);
	  SUtils::add(bin, this->builtin_uint32_in_0->getPointerID());
	  SUtils::add(bin, 11);
	  SUtils::add(bin, 43); // InstanceIndex
	}
	
	// Vertex Index
	if(this->builtin_uint32_in_1) {
	  SUtils::add(bin, (4 << 16) | 71);
	  SUtils::add(bin, this->builtin_uint32_in_1->getPointerID());
	  SUtils::add(bin, 11);
	  SUtils::add(bin, 42); // VertexIndex
	}


      } else if constexpr(type == SShaderType::SHADER_FRAGMENT) {
	if(this->builtin_vec4_in_0) {
	  // Frag Coord
	  SUtils::add(bin, (4 << 16) | 71);
	  SUtils::add(bin, this->builtin_vec4_in_0->getPointerID());
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
      SUtils::add(bin, input_entries[i]->getPointerID());
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
  void SShader<type, InputTypes...>::output_used_builtin_pointers(std::vector<uint32_t>& res){
    if constexpr(type == SShaderType::SHADER_VERTEX) {
	if(this->builtin_vec4_out_0) {
	  this->builtin_vec4_out_0->pointer->ensure_defined(res);
	}
	if(this->builtin_float_out_0) {
	  this->builtin_float_out_0->pointer->ensure_defined(res);
	}
	if(this->builtin_arr_1_float_out_0) {
	  this->builtin_arr_1_float_out_0->pointer->ensure_defined(res);
	}
	if(this->builtin_arr_1_float_out_1) {
	  this->builtin_arr_1_float_out_1->pointer->ensure_defined(res);
	}
	if(this->builtin_uint32_in_0) {
	  this->builtin_uint32_in_0->pointer->ensure_defined(res);
	}
	if(this->builtin_uint32_in_1) {
	  this->builtin_uint32_in_1->pointer->ensure_defined(res);
	}
      } else if constexpr (type == SShaderType::SHADER_FRAGMENT) {
	if(this->builtin_vec4_in_0) {
	  this->builtin_vec4_in_0->pointer->ensure_defined(res);
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

	    if(!this->builtin_uint32_in_0) {
	      this->builtin_uint32_in_0 = SUtils::allocate<BuiltinEntry<uint32_s, SStorageClass::STORAGE_INPUT> >();
	    }
	    return this->builtin_uint32_in_0->pointer->load();
	    
	  } else if constexpr(ind == BUILTIN_VERTEX_INDEX) {
	    
	    static_assert(std::is_same<tt, uint32_s>::value, "Vertex shader vertex ID must be unsigned 32-bit integer");
	    if(!this->builtin_uint32_in_1) {
	      this->builtin_uint32_in_1 = SUtils::allocate<BuiltinEntry<uint32_s, SStorageClass::STORAGE_INPUT> >();
	    }
	    return this->builtin_uint32_in_1->pointer->load();
	  } else {
	  printf("Builtin does not support read in vertex shader\n");
	  exit(-1);
	}
      } else if constexpr(type == SShaderType::SHADER_FRAGMENT) {
	if constexpr(ind == BUILTIN_FRAG_COORD) {
	    static_assert(std::is_same<tt, vec4_s>::value, "Fragment shader fragment coordinate must have type vec4");
	    
	    if(!this->builtin_vec4_in_0) {
	      this->builtin_vec4_in_0 = SUtils::allocate<BuiltinEntry<vec4_s, SStorageClass::STORAGE_INPUT> >();
	    }
	    return this->builtin_vec4_in_0->pointer->load();
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
        
	    if(this->builtin_vec4_out_0) {
	      printf("Cannot set builtin multiple times");
	      exit(-1);
	    }
	    this->builtin_vec4_out_0 = SUtils::allocate<BuiltinEntry<vec4_s, SStorageClass::STORAGE_OUTPUT> >();
	    this->builtin_vec4_out_0->pointer->store(val);
	    
	  } else if constexpr(ind == BUILTIN_POINT_SIZE) {
	    
	    if(this->builtin_float_out_0) {
	      printf("Cannot set builtin multiple times");
	      exit(-1);
	    }
	    
	    this->builtin_float_out_0 = SUtils::allocate<BuiltinEntry<float_s, SStorageClass::STORAGE_OUTPUT> >();
	    this->builtin_float_out_0->pointer->store(val);
	    
	  } else if constexpr(ind == BUILTIN_CLIP_DISTANCE) {
	    
	    if(this->builtin_arr_1_float_out_0) {
	      printf("Cannot set builtin multiple times");
	      exit(-1);
	    }
	    
	    this->builtin_arr_1_float_out_0 = SUtils::allocate<BuiltinEntry<arr_1_float_s, SStorageClass::STORAGE_OUTPUT> >();
	    this->builtin_arr_1_float_out_0->pointer->store(val);
	  } else if constexpr(ind == BUILTIN_CULL_DISTANCE) {
	    
	    if(this->builtin_arr_1_float_out_1) {
	      printf("Cannot set builtin multiple times");
	      exit(-1);
	    }
	    this->builtin_arr_1_float_out_1 = SUtils::allocate<BuiltinEntry<arr_1_float_s, SStorageClass::STORAGE_OUTPUT> >();
	    this->builtin_arr_1_float_out_1->pointer->store(val);
	  } else {
	  printf("This builtin has not yet been defined in setBuiltinOutput");
	  exit(-1);
	}
      } else {
      printf("This shader type is not yet supported in setBuiltinOutput\n");
      exit(-1);
    }
  }
  
  /* template<SShaderType type, typename... InputTypes>
  template<int n>
  auto& SShader<type, InputTypes...>::input() {
    return this->input<n, 0, InputTypes...>();
    } */

  /* template<SShaderType type, typename... InputTypes>
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
    } */

  template<SShaderType type, typename... InputTypes>
  template<int n>
  SValue<typename SUtils::NthType<n, InputTypes...>::type>& SShader<type, InputTypes...>::input() {
    
    using itt = typename SUtils::NthType<n, InputTypes...>::type;
    
    if(input_entries[n] == nullptr) {
      input_entries[n] = SUtils::allocate<InputVariableEntry<itt> >(n);
    }
    
    InputVariableBase* in = input_entries[n];
    
    return *(SValue<itt>*)(in->getValue());
  }
  
  /* template<SShaderType type, typename... InputTypes>
  template<int n, int curr, typename First, typename... Rest>
  auto& SShader<type, InputTypes...>::input() {
    using input_type = First;
    static_assert(is_spurv_type<input_type>::value, "Supplied input values must be SType");

    if constexpr(n > curr) {
	return input<n, curr + 1, Rest...>();
      } else {
      return input<n, First>();
    }
    } */

  template<SShaderType type, typename... InputTypes>
  SUniformBindingBase* SShader<type, InputTypes...>::find_binding(int set, int binding) {
    for(unsigned int i = 0; i < this->uniform_bindings.size(); i++) {
      if(this->uniform_bindings[i]->getSetNo() == set &&
	 this->uniform_bindings[i]->getBindingNo() == binding) {

	// Disallow accessing a binding multiple times, as we currently don't have a way
	// for checking whether the type is equal in the two accesses
	printf("[spurv::SShader::find_binding()] Accessing the same binding multiple times is currently disallowed\n");
	exit(-1);
	
	return uniform_bindings[i];
      }
    }

    return nullptr;
  }

  template<SShaderType type, typename... InputTypes>
  template<typename BindingType>
  SUniformBindingBase* SShader<type, InputTypes...>::construct_binding(int set, int binding) {
    SUniformBindingBase* sb = find_binding(set, binding);
    
    if(sb != nullptr) {
      return sb;
    }

    BindingType* pp = SUtils::allocate<BindingType>(set, binding);
    this->uniform_bindings.push_back(pp);
    return pp;
  }

  template<SShaderType type, typename... InputTypes>
  template<typename tt>
  SPointerVar<tt, SStorageClass::STORAGE_UNIFORM_CONSTANT>&
  SShader<type, InputTypes...>::uniformConstant(int set_no, int binding_no) {
    SUniformBindingBase* sb = construct_binding<SUniformConstant<tt> >(set_no, binding_no);
    return *(SPointerVar<tt, SStorageClass::STORAGE_UNIFORM_CONSTANT>*)sb->getPointer();
  }

  template<SShaderType type, typename... InputTypes>
  template<typename... InnerTypes>
  // SUniformBinding<InnerTypes...>&
  SPointerVar<SStruct<InnerTypes...>, SStorageClass::STORAGE_UNIFORM >&
  SShader<type, InputTypes...>::uniformBinding(int set_no, int binding_no) {
    SUniformBindingBase* sb = construct_binding<SUniformBinding<InnerTypes...> >(set_no, binding_no);
    return  *(SPointerVar<SStruct<InnerTypes...>, SStorageClass::STORAGE_UNIFORM>*)sb->getPointer();
  }

  template<SShaderType type, typename... InputTypes>
  template<typename... InnerTypes>
  // SStorageBuffer<InnerTypes...>&
  SPointerVar<SStruct<InnerTypes...>, SStorageClass::STORAGE_STORAGE_BUFFER >&
  SShader<type, InputTypes...>::storageBuffer(int set_no, int binding_no) {
    this->extensions.insert(SExtension::EXTENSION_STORAGE_BUFFER);

    SUniformBindingBase* sb = construct_binding<SStorageBuffer<InnerTypes...> >(set_no, binding_no);
    return *(SPointerVar<SStruct<InnerTypes...>, SStorageClass::STORAGE_STORAGE_BUFFER >*)sb->getPointer();
  }

  template<SShaderType type, typename... InputTypes>
  template<typename tt>
  SLocal<tt>& SShader<type, InputTypes...>::local() {
    return *SUtils::allocate<SLocal<tt> >();
  }

  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::ifThen(SValue<bool_s>& condition) {
    SIfThen* it = SUtils::allocate<SIfThen>(&condition);
    this->block_stack.push_back(it);
    SEventRegistry::addIf(it);
  }

  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::elseThen() {
    if(this->block_stack.size() == 0) {
      printf("[spurv] Else not matched to an if\n");
      exit(-1);
    }
    if(this->block_stack[this->block_stack.size() - 1]->getControlType() != CONTROL_IF) {
      printf("[spurv] Else cannot be used here, there is another unclosed block\n");
      exit(-1);
    }

    if(((SIfThen*)this->block_stack[this->block_stack.size() - 1])->has_else()) {
      printf("[spurv] Tried to give to define a second else of for an if-statement\n");
      exit(-1);
    }

    ((SIfThen*)this->block_stack[this->block_stack.size() - 1])->add_else();

    SEventRegistry::addElse((SIfThen*)this->block_stack[this->block_stack.size() - 1]);
  }

  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::endIf() {
    if(this->block_stack.size() == 0) {
      printf("[spurv] EndIf not matched to an if\n");
      exit(-1);
    }
    if(this->block_stack[this->block_stack.size() - 1]->getControlType() != CONTROL_IF) {
      printf("[spurv] EndIfcannot be used here, there is another unclosed block\n");
      exit(-1);
    }

    SEventRegistry::addEndIf((SIfThen*)this->block_stack[this->block_stack.size() - 1]);
    this->block_stack.pop_back();
  }
  
  template<SShaderType type, typename... InputTypes>
  SValue<int_s>& SShader<type, InputTypes...>::forLoop(int arg0, int arg1) {
    SForLoop* fl = SUtils::allocate<SForLoop>(arg0, arg1);
    this->block_stack.push_back(fl);
    SEventRegistry::addForBegin(fl);

    return *fl->iterator_val;
  }

  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::endLoop() {
    if(this->block_stack.size() == 0) {
      printf("[spurv] Tried to end loop when no loop is active\n");
      exit(-1);
    }
    if(this->block_stack[this->block_stack.size() - 1]->getControlType() != CONTROL_FOR) {
      printf("[spurv] Cannot end for-loop, as there are other unclosed blocks\n");
      exit(-1);
    }
    
    SForLoop* fl = (SForLoop*)this->block_stack[this->block_stack.size() - 1];
    this->block_stack.pop_back();
    SEventRegistry::addForEnd(fl);
  }

  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::breakLoop() {
    SForLoop* fl = nullptr;
    
    for(int i = (int)(this->block_stack.size()) - 1; i >= 0; i--) {
      if(this->block_stack[i]->getControlType() == CONTROL_FOR) {
	fl = (SForLoop*)this->block_stack[i];
	break;
      }
    }

    if(fl == nullptr) {
      printf("[spurv] There is no active loop block in stack for break\n");
      exit(-1);
    }

    SEventRegistry::addBreak(fl);
  }

  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::continueLoop() {
    SForLoop* fl = nullptr;

    for(int i = (int)(this->block_stack.size()) - 1; i >= 0; i--) {
      if(this->block_stack[i]->getControlType() == CONTROL_FOR) {
	fl = (SForLoop*)this->block_stack[i];
	break;
      }
    }

    if(fl == nullptr) {
      printf("[spurv] There is no active loop block in stack for continue\n");
      exit(-1);
    }

    SEventRegistry::addContinue(fl);
  }

  template<SShaderType type, typename... InputTypes>
  template<typename... NodeTypes>
  void SShader<type, InputTypes...>::compile(std::vector<uint32_t>& res, NodeTypes&&... args) {

    if(this->block_stack.size()) {
      printf("[spurv] There were unfinished loops/if statements in shader\n");
      exit(-1);
    }

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
    
    this->output_output_pointers(res, 0, args...);
    this->output_used_builtin_pointers(res);

    this->output_main_function_begin(res);

    SVariableRegistry::write_variable_definitions(res);

    SEventRegistry::write_events(res);
    
    this->output_output_definitions(res, 0, args...);

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
