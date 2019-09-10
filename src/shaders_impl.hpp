#ifndef __SPURV_SHADERS_IMPL
#define __SPURV_SHADERS_IMPL

#include "shaders.hpp"
#include "variable_registry.hpp"

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

    return num;
  }
  
  template<SShaderType type, typename...InputTypes>
  void SShader<type, InputTypes...>::cleanup_declaration_states() {
    for(uint i = 0; i < this->defined_type_declaration_states.size(); i++) {
      this->defined_type_declaration_states[i]->is_defined  = false;
      this->defined_type_declaration_states[i]->id = -1;
    }
  }
  
  
  /*
   * Output member functions - functions writing binaries to vector
   */
  
  template<SShaderType type, typename... InputTypes>
  template<typename tt>
  void SShader<type, InputTypes...>::output_shader_header_output_variables(std::vector<uint32_t>& binary,
									       int n, SValue<tt>& arg0) {
    static_assert(is_spurv_type<tt>::value);
    binary.push_back(this->output_pointer_ids[n]);
  }
  
  template<SShaderType type, typename... InputTypes>
  template<typename tt, typename... NodeTypes>
  void SShader<type, InputTypes...>::output_shader_header_output_variables(std::vector<uint32_t>& binary,
									       int n, SValue<tt>& arg0, NodeTypes... args) {
    static_assert(is_spurv_type<tt>::value);
    binary.push_back(this->output_pointer_ids[n]);
    output_shader_header_output_variables(binary, n + 1, args...);
  }

  template<SShaderType type, typename... InputTypes>
  template<typename tt>
  void SShader<type, InputTypes...>::output_shader_header_decorate_output_variables(std::vector<uint32_t>& bin, int n,
									 SValue<tt>& arg0) {
    bin.push_back((4 << 16) | 71);
    bin.push_back(this->output_pointer_ids[n]);
    bin.push_back(30);
    bin.push_back(n);
  }									 
  
  template<SShaderType type, typename... InputTypes>
  template<typename tt, typename... NodeTypes>
  void SShader<type, InputTypes...>::output_shader_header_decorate_output_variables(std::vector<uint32_t>& bin, int n,
									 SValue<tt>& arg0, NodeTypes... args) {
    bin.push_back((4 << 16) | 71);
    bin.push_back(arg0->getID());
    bin.push_back(30);
    bin.push_back(n);

    output_shader_header_decorate_output_variables(bin, n + 1, args...); 
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
  template<typename tt>
  void SShader<type, InputTypes...>::output_output_tree_type_definitions(std::vector<uint32_t>& bin,
									 SValue<tt>& val) {
    
    val.ensure_type_defined(bin, this->defined_type_declaration_states);
  }

  
  template<SShaderType type, typename... InputTypes>
  template<typename tt, typename... NodeTypes>
  void SShader<type, InputTypes...>::output_output_tree_type_definitions(std::vector<uint32_t>& bin,
									 SValue<tt>& val,
									 NodeTypes... args) {
    val.ensure_type_defined(bin, this->defined_type_declaration_states);

    this->output_type_definitions(bin, args...);
  }


  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::output_shader_header_begin(std::vector<uint32_t>& bin) {
    // capability Shader
    SUtils::add(bin, (2 << 16) | 17);
    SUtils::add(bin, 1);

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

    std::string entry_point_name = "main";

    if constexpr(type == SHADER_VERTEX) {
	SUtils::add(bin, 0); // Vertex
      } else if constexpr(type == SHADER_FRAGMENT) {
	SUtils::add(bin, 4); // Fragment
      } else {
      printf("Shader type not yet accounted for in output_shader_header_begin\n");
      exit(-1);
    }

    this->entry_point_id = SUtils::getNewID();
    SUtils::add(bin, this->entry_point_id);
    SUtils::add(bin, entry_point_name);

    for(uint i = 0; i < this->input_entries.size(); i++) {
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
    if constexpr(type == SHADER_FRAGMENT) {
	// OpExecutionMode <entry_point_id> OriginUpperLeft
	SUtils::add(bin, (3 << 16) | 16);
	SUtils::add(bin, this->entry_point_id);
	SUtils::add(bin, 7);
      }
  }
  
  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::output_shader_header_decorate_begin(std::vector<uint32_t>& bin) {
    if constexpr(type == SHADER_VERTEX) {
	if(this->builtin_vec4_0) {
	  // Decorate <builtin> Builtin Position
	  SUtils::add(bin, (4 << 16) | 71);
	  SUtils::add(bin, this->builtin_vec4_0->pointer_id);
	  SUtils::add(bin, 11);
	  SUtils::add(bin, 0);
	}
    
	if(this->builtin_float_0) {
	  SUtils::add(bin, (4 << 16) | 71);
	  SUtils::add(bin, this->builtin_float_0->pointer_id);
	  SUtils::add(bin, 11);
	  SUtils::add(bin, 1);
	}
	if(this->builtin_arr_1_float_0) {
	  SUtils::add(bin, (4 << 16) | 71);
	  SUtils::add(bin, this->builtin_arr_1_float_0->pointer_id);
	  SUtils::add(bin, 11);
	  SUtils::add(bin, 3);
	}
	if(this->builtin_arr_1_float_1) {
	  SUtils::add(bin, (4 << 16) | 71);
	  SUtils::add(bin, this->builtin_arr_1_float_1->pointer_id);
	  SUtils::add(bin, 11);
	  SUtils::add(bin, 4);
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
      SUtils::add(bin, (4 << 16) | 71);
      SUtils::add(bin, input_entries[i].pointer_id);
      SUtils::add(bin, 30);
      SUtils::add(bin, i);
    }

    // Put decorations on uniforms
    for(uint i = 0; i < uniform_bindings.size(); i++) {
      
      uniform_bindings[i]->decorateType(bin);

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
    SType<KIND_VOID>::ensure_defined(res, this->defined_type_declaration_states);

    int void_function_type = SUtils::getNewID();
    
    // OpTypeFunction <result_id> <result type> <result_id> 
    SUtils::add(res, (3 << 16) | 33);
    SUtils::add(res, void_function_type);
    SUtils::add(res, SType<KIND_VOID>::getID());

    // OpFunction <result type> <result_id> <function_control> <function_type>
    SUtils::add(res, (5 << 16) | 54);
    SUtils::add(res, SType<KIND_VOID>::getID());
    SUtils::add(res, entry_point_id);
    SUtils::add(res, 0);
    SUtils::add(res, void_function_type);

    // OpLabel <result_id>
    SUtils::add(res, (2 << 16) | 248);
    SUtils::add(res, SUtils::getNewID());
    
  }

  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::output_main_function_variables(std::vector<uint32_t>& res) {
    std::vector<VariableEntry>* entries = SVariableRegistry::getVector();

    for(VariableEntry& entr : *entries) {
      // OpVariables <result_type> <result_id> Function
      SUtils::add(res, (4 << 16) | 59);
      SUtils::add(res, entr.type_id);
      SUtils::add(res, entr.variable_id);
      SUtils::add(res, 7);
    }
  }

  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::output_main_function_end(std::vector<uint32_t>& res) {
    // OpReturn
    SUtils::add(res, (1 << 16) | 253);

    // OpFunctionEnd
    SUtils::add(res, (1 << 16) | 56);
  }

  template<SShaderType type, typename... InputTypes>
  template<typename tt, typename... NodeTypes>
  void SShader<type, InputTypes...>::output_output_definitions(std::vector<uint32_t>& res, int n,
								   SValue<tt>& node, NodeTypes... args) {
    node.ensure_defined(res);

    // OpStore
    SUtils::add(res, (3 << 16) | 62);
    SUtils::add(res, this->output_pointer_ids[n]);
    SUtils::add(res, node.getID());

    this->output_output_definitions(res, n + 1, args...);
  }

  template<SShaderType type, typename... InputTypes>
  template<typename tt>
  void SShader<type, InputTypes...>::output_output_definitions(std::vector<uint32_t>& res, int n,
								   SValue<tt>& node) {
    node.ensure_defined(res);

    // OpStore
    SUtils::add(res, (3 << 16) | 62);
    SUtils::add(res, this->output_pointer_ids[n]);
    SUtils::add(res, node.getID());
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
  template<typename tt, typename... NodeTypes>
  void SShader<type, InputTypes...>::output_output_pointers(std::vector<uint32_t>& res, int n,
								SValue<tt>& val, NodeTypes... args) {
    SPointer<STORAGE_OUTPUT, tt>::ensure_defined(res, this->defined_type_declaration_states);

    // OpVariable...
    SUtils::add(res, (4 << 16) | 59);
    SUtils::add(res, SPointer<STORAGE_OUTPUT, tt>::id);
    SUtils::add(res, this->output_pointer_ids[n]);
    SUtils::add(res, STORAGE_OUTPUT);

    this->output_output_pointers(res, n + 1, args...);
  }

  template<SShaderType type, typename... InputTypes>
  template<typename tt>
  void SShader<type, InputTypes...>::output_output_pointers(std::vector<uint32_t>& res, int n, SValue<tt>& val) {
    SPointer<STORAGE_OUTPUT, tt>::ensure_defined(res, this->defined_type_declaration_states);

    // OpVariable...
    SUtils::add(res, (4 << 16) | 59);
    SUtils::add(res, SPointer<STORAGE_OUTPUT, tt>::getID());
    SUtils::add(res, this->output_pointer_ids[n]);
    SUtils::add(res, STORAGE_OUTPUT);
  }

  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::output_uniform_pointers(std::vector<uint32_t>& res) {
    for(uint i = 0; i < this->uniform_bindings.size(); i++) {
      this->uniform_bindings[i]->definePointer(res, this->defined_type_declaration_states);
    }
  }

  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::output_used_builtin_pointers(std::vector<uint32_t>& res){
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
  }
  
  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::output_builtin_output_definitions(std::vector<uint32_t>& res) {
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

  
  /*
   * Public member function
   */

  template<SShaderType type, typename... InputTypes>
  template<SBuiltinVariable ind, typename tt>
  void SShader<type, InputTypes...>::setBuiltin(SValue<tt>& val) {
    if constexpr(type == SHADER_VERTEX) {
	if constexpr(ind == BUILTIN_POSITION) {
        
	    static_assert(std::is_same<tt, vec4_s>::value, "Position must be vec4_s");
	    if(this->builtin_vec4_0) {
	      printf("Cannot set builtin multiple times");
	      exit(-1);
	    }
	    this->builtin_vec4_0 = SUtils::allocate<BuiltinEntry<vec4_s> >();
	    this->builtin_vec4_0->value_node = &val;
	    this->builtin_vec4_0->pointer_id = SUtils::getNewID();
	  } else if constexpr(ind == BUILTIN_POINT_SIZE) {
	    static_assert(std::is_same<tt, float_s>::value, "PointSize must be float_s");
	    
	    if(this->builtin_float_0) {
	      printf("Cannot set builtin multiple times");
	      exit(-1);
	    }
	    this->builtin_float_0 = SUtils::allocate< BuiltinEntry<float_s> >();
	    this->builtin_float_0->value_node = &val;
	    this->builtin_float_0->pointer_id = SUtils::getNewID();
	  } else if constexpr(ind == BUILTIN_CLIP_DISTANCE) {
	    static_assert(std::is_same<tt, arr_1_float_s>::value, "Clip Distance must be arr_1_float_s");
	    
	    if(this->builtin_arr_1_float_0) {
	      printf("Cannot set builtin multiple times");
	      exit(-1);
	    }
	    this->builtin_arr_1_float_0 = SUtils::allocate<BuiltinEntry<arr_1_float_s> >();
	    this->builtin_arr_1_float_0->value_node = &val;
	    this->builtin_arr_1_float_0->pointer_id = SUtils::getNewID();
	  } else if constexpr(ind == BUILTIN_CULL_DISTANCE) {
	    static_assert(std::is_same<tt, arr_1_float_s>::value, "Cull Distance must be arr_1_float_s");
	    
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

    if(input_entries[n].ds.kind == KIND_INVALID) {
      
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

  template<typename... InnerTypes>
  constexpr bool isUniformConstantType() {
    return sizeof...(InnerTypes) == 1 &&
      is_spurv_texture_type<typename SUtils::NthType<0, InnerTypes...>::type>::value;
  }

  template<SShaderType type, typename... InputTypes>
  template<typename... InnerTypes>
  typename std::conditional<isUniformConstantType<InnerTypes...>(),
			    SUniformConstant<typename SUtils::NthType<0, InnerTypes...>::type>,
			    SUniformBinding<InnerTypes...> >::type&
  SShader<type, InputTypes...>::uniformBinding(int set_no, int binding_no) {

    // Check if this is just a uniform constant, not a struct
    if constexpr(isUniformConstantType<InnerTypes...>()) {

	using Type = typename SUtils::NthType<0, InnerTypes...>::type;
	
	for(uint i = 0; i < this->uniform_bindings.size(); i++) {
	  if(this->uniform_bindings[i]->getSetNo() == set_no &&
	     this->uniform_bindings[i]->getBindingNo() == binding_no) {
	    return *(SUniformConstant<Type>*)uniform_bindings[i];
	  }
	}

	SUniformConstant<Type>* pp = SUtils::allocate<SUniformConstant<Type> >(set_no, binding_no);

	this->uniform_bindings.push_back((SUniformBindingBase*)pp);
	return *pp;
	
      } else {
    
      // O(n^2).. But hopefully nobody cares
      for(uint i = 0; i < this->uniform_bindings.size(); i++) {
	if(this->uniform_bindings[i]->getSetNo() == set_no &&
	   this->uniform_bindings[i]->getBindingNo() == binding_no) {
	  return *(SUniformBinding<InnerTypes...>*)uniform_bindings[i];
	}
      }

      SUniformBinding<InnerTypes...>* pp = SUtils::allocate<SUniformBinding<InnerTypes...> >(set_no, binding_no);

      this->uniform_bindings.push_back((SUniformBindingBase*)pp);
      return *pp;
    }
    
  }

  template<SShaderType type, typename... InputTypes>
  template<typename... NodeTypes>
  void SShader<type, InputTypes...>::compile(std::vector<uint32_t>& res, NodeTypes&... args) {

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

    this->output_output_tree_type_definitions(res, args...);
    this->output_builtin_tree_type_definitions(res);
    
    this->output_input_pointers<0, InputTypes...>(res);
    this->output_output_pointers(res, 0, args...);
    this->output_uniform_pointers(res);
    this->output_used_builtin_pointers(res);

    this->output_main_function_begin(res);
    this->output_main_function_variables(res);
    
    this->output_output_definitions(res, 0, args...);
    this->output_builtin_output_definitions(res);

    this->output_main_function_end(res);

    res[this->id_max_bound_index] = SUtils::getCurrentID();

    this->cleanup_declaration_states();
    
    SUtils::resetID();
    SUtils::clearAllocations();
    SConstantRegistry::resetRegistry();
  }
};
#endif // __SPURV_SHADERS_IMPL
