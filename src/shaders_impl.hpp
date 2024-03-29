#ifndef __SPURV_SHADERS_IMPL
#define __SPURV_SHADERS_IMPL

#include "shaders.hpp"

namespace spurv {


  /*
   * Constructor(s?)
   */

  template<SShaderType type, typename... InputTypes>
  SShader<type, InputTypes...>::SShader() {

    input_entries = std::vector<InputVariableBase*>(sizeof...(InputTypes), nullptr);
  }


  /*
   * Util functions
   */

  template<SShaderType type, typename... InputTypes>
  int SShader<type, InputTypes...>::get_num_defined_builtins() {
    return this->builtin_entries.size();
  }

  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::create_output_variables() {
    return;
  }

  template<SShaderType type, typename... InputTypes>
  template<typename tt1, typename... NodeTypes>
  void SShader<type, InputTypes...>::create_output_variables(SValue<tt1>& val, NodeTypes&&... rest) {
    OutputVariableEntry<tt1>* var = SUtils::allocate<OutputVariableEntry<tt1> >(this->output_entries.size());
    this->output_entries.push_back(var);
    var->template store<tt1>(val);

    if constexpr(sizeof...(rest) > 0) {
	this->create_output_variables(rest...);
      }
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
    SUtils::add(binary, this->output_entries[n]->getPointerID());

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
    bin.push_back(this->output_entries[n]->getPointerID());
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

    output_shader_header_decorate_tree(bin, args...);
  }


  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::output_used_builtin_ids(std::vector<uint32_t>& bin) {
    for(unsigned int i = 0; i < this->builtin_entries.size(); i++) {
      SUtils::add(bin, this->builtin_entries[i]->getPointerID());
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
      } else if constexpr(type == SShaderType::SHADER_COMPUTE) {
	SUtils::add(bin, 5); // GLCompute
      } else {
      printf("Shader type not yet accounted for in output_shader_header_begin\n");
      exit(-1);
    }

    this->entry_point_id = SUtils::getNewID();
    SUtils::add(bin, this->entry_point_id);
    SUtils::add(bin, entry_point_name);

    for(unsigned int i = 0; i < this->input_entries.size(); i++) {

      if (this->input_entries[i] == nullptr) {
        printf("[spurv] Input variable %d not used in shader, This is considered illegal for now.\n", i);
        exit(-1);
      }

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
      } else if constexpr(type == SShaderType::SHADER_COMPUTE) {
	// OpExecutionMode <entry_point_id> LocalSize <local_x_size> <local_y_size> <local_z_size>
	SUtils::add(bin, (6 << 16) | 16);
	SUtils::add(bin, this->entry_point_id);
	SUtils::add(bin, 17); // LocalSize
	SUtils::add(bin, 32); // Default, TODO: Make changable by user
	SUtils::add(bin, 1); // --"--
	SUtils::add(bin, 1); // --"--
      }


    int strl = SUtils::stringWordLength(entry_point_name);

    // OpName <main_id> "main"
    SUtils::add(bin, ((2 + strl) << 16) | 5);
    SUtils::add(bin, this->entry_point_id);
    SUtils::add(bin, entry_point_name);

  }

  template<SShaderType type, typename... InputTypes>
  void SShader<type, InputTypes...>::output_shader_header_decorate_begin(std::vector<uint32_t>& bin) {

    for(unsigned int i = 0; i < builtin_entries.size(); i++) {
      builtin_entries[i]->decorate(bin);
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


  /*
   * Public member function
   */

  template<SShaderType type, typename... InputTypes>
  template<SBuiltinVariable ind>
  SValue< typename BuiltinInfo<type, ind>::type >& SShader<type, InputTypes...>::getBuiltin() {
    using tt = typename BuiltinInfo<type, ind>::type;
    constexpr SStorageClass stind = BuiltinInfo<type, ind>::storage;


    for(unsigned int i = 0; i < this->builtin_entries.size(); i++) {
      if(this->builtin_entries[i]->builtin_id == ind) {
	return ((BuiltinEntry<tt, stind>*)this->builtin_entries[i])->pointer->load();
      }
    }

    BuiltinEntry<tt, stind>* entry = SUtils::allocate<BuiltinEntry<tt, stind> >(ind);
    this->builtin_entries.push_back(entry);
    return entry->pointer->load();
  }

  template<SShaderType type, typename... InputTypes>
  template<SBuiltinVariable ind, typename tt>
  void SShader<type, InputTypes...>::setBuiltin(SValue<tt>& val) {
    static_assert(std::is_same<tt, typename BuiltinInfo<type, ind>::type>::value,
		  "[spurv] Compilation error: Mismatch type in builtin set");

    using res_type = typename BuiltinInfo<type, ind>::type;
    constexpr SStorageClass stind = BuiltinInfo<type, ind>::storage;

    BuiltinEntry<res_type, stind>* entry = nullptr;
    for(unsigned int i = 0; i < this->builtin_entries.size(); i++) {
      if(this->builtin_entries[i]->builtin_id == ind) {
	entry = (BuiltinEntry<res_type, stind>*)this->builtin_entries[i];
      }
    }

    if(entry == nullptr) {
      entry = SUtils::allocate<BuiltinEntry<res_type, stind> >(ind);
      this->builtin_entries.push_back(entry);
    }

    entry->pointer->store(val);
  }

  template<SShaderType type, typename... InputTypes>
  template<int n>
  SValue<typename SUtils::NthType<n, InputTypes...>::type>& SShader<type, InputTypes...>::input() {

    using itt = typename SUtils::NthType<n, InputTypes...>::type;

    if(input_entries[n] == nullptr) {
      input_entries[n] = SUtils::allocate<InputVariableEntry<itt> >(n);
    }

    InputVariableBase* in = input_entries[n];

    return *in->template getValueTyped<itt>();
  }

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
  SPointerVar<SStruct<SDecoration::BLOCK, InnerTypes...>, SStorageClass::STORAGE_UNIFORM >&
  SShader<type, InputTypes...>::uniformBinding(int set_no, int binding_no) {
    SUniformBindingBase* sb = construct_binding<SUniformBinding<InnerTypes...> >(set_no, binding_no);
    return  *(SPointerVar<SStruct<SDecoration::BLOCK, InnerTypes...>, SStorageClass::STORAGE_UNIFORM>*)sb->getPointer();
  }

  template<SShaderType type, typename... InputTypes>
  template<typename... InnerTypes>
  SPointerVar<SStruct<SDecoration::BLOCK, InnerTypes...>, SStorageClass::STORAGE_STORAGE_BUFFER >&
  SShader<type, InputTypes...>::storageBuffer(int set_no, int binding_no) {
    this->extensions.insert(SExtension::EXTENSION_STORAGE_BUFFER);

    SUniformBindingBase* sb = construct_binding<SStorageBuffer<InnerTypes...> >(set_no, binding_no);
    return *(SPointerVar<SStruct<SDecoration::BLOCK, InnerTypes...>, SStorageClass::STORAGE_STORAGE_BUFFER >*)sb->getPointer();
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

    this->create_output_variables(args...);

    this->output_preamble(res);
    this->output_shader_header_begin(res);

    constexpr int num_args = sizeof...(NodeTypes);

    this->output_shader_header_output_variables(res, 0, args...);

    this->output_shader_header_end(res);

    res[this->entry_point_declaration_size_index] |= ( 3 + 2 + this->input_entries.size() + num_args + get_num_defined_builtins()) << 16;


    this->output_shader_header_decorate_begin(res);
    this->output_shader_header_decorate_output_variables(res, 0, args...);
    this->output_shader_header_decorate_tree(res, args...);

    SEventRegistry::write_type_definitions(res,
					   this->defined_type_declaration_states);
    this->output_output_tree_type_definitions(res, args...);

    this->output_main_function_begin(res);

    SVariableRegistry::write_variable_definitions(res);

    SEventRegistry::write_events(res);

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
