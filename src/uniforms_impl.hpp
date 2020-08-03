#ifndef __SPURV_UNIFORMS_IMPL
#define __SPURV_UNIFORMS_IMPL

#include "uniforms.hpp"

namespace spurv {
    
  /*
   * SStructBinding member functions
   */

  template<SStorageClass stind, typename... InnerTypes>
  SStructBinding<stind, InnerTypes...>::SStructBinding(int sn, int bn) : SUniformBindingBase(sn, bn) {
    this->pointer = SUtils::allocate<SPointerVar<SStruct<InnerTypes...>, stind > >();
  }

  template<SStorageClass stind, typename... InnerTypes>
  void SStructBinding<stind, InnerTypes...>::definePointer(std::vector<uint32_t>& bin,
							   std::vector<SDeclarationState*>& declaration_states) {
    this->pointer->ensure_type_defined(bin, declaration_states);
    this->pointer->ensure_defined(bin);

  }

  template<SStorageClass stind, typename... InnerTypes>
  void SStructBinding<stind, InnerTypes...>::decorateType(std::vector<uint32_t>& bin,
							  std::vector<bool*>& decoration_states) {
    if(!SStruct<InnerTypes...>::is_decorated) {
      SStruct<InnerTypes...>::ensure_decorated(bin, decoration_states);
      SStruct<InnerTypes...>::decorate_block(bin, decoration_states);
    }
  }

  
  template<SStorageClass stind, typename... InnerTypes>
  void* SStructBinding<stind, InnerTypes...>::getPointer() {
    return (void*)this->pointer;
  }
  
  template<SStorageClass stind, typename... InnerTypes>
  int SStructBinding<stind, InnerTypes...>::getPointerID() {
    return this->pointer->getID();
  }

  
  /*
   * SUniformBinding constructor
   */

  template<typename... Types>
  struct contains_runtime_array;

  template<>
  struct contains_runtime_array<> : std::false_type {};

  template<typename tt>
  struct contains_runtime_array<tt> : std::false_type {};

  template<typename First, typename... Rest>
  struct contains_runtime_array<First, Rest...> { static constexpr bool value = contains_runtime_array<First>::value ||
      contains_runtime_array<Rest...>::value; };

  template<SStorageClass storage, typename tt>
  struct contains_runtime_array<SRunArr<storage, tt> > : std::true_type {};
  
  template<typename... Types>
  struct contains_runtime_array<SStruct<Types...> > : contains_runtime_array<Types...> {};
  
  
  template<typename... InnerTypes>
  SUniformBinding<InnerTypes...>::SUniformBinding(int sn, int bn) : SStructBinding<STORAGE_UNIFORM, InnerTypes...>(sn, bn) {
    static_assert(!contains_runtime_array<SStruct<InnerTypes...> >::value,
		  "[spurv::SUniformBinding()] A uniform binding cannot contain a runtime-length array. Use storage buffer instead.");
  }


  /*
   * SStorageBuffer constructor
   */

  template<typename... InnerTypes>
  SStorageBuffer<InnerTypes...>::SStorageBuffer(int sn, int bn) : SStructBinding<STORAGE_STORAGE_BUFFER, InnerTypes...>(sn, bn) { }

  
  /*
   * SUniformConstant member functions
   */

  template<typename type>
  SUniformConstant<type>::SUniformConstant(int sn, int bn) : SUniformBindingBase(sn, bn) {
    this->pointer = SUtils::allocate<SPointerVar<type, SStorageClass::STORAGE_UNIFORM_CONSTANT> >();
  }

  template<typename type>
  void SUniformConstant<type>::definePointer(std::vector<uint32_t>& bin,
					     std::vector<SDeclarationState*>& declaration_states) {

    this->pointer->ensure_type_defined(bin, declaration_states);
    this->pointer->ensure_defined(bin);
  }

  template<typename type>
  void* SUniformConstant<type>::getPointer() {
    return (void*)this->pointer;
  }

  template<typename type>
  int SUniformConstant<type>::getPointerID() {
    return this->pointer->getID();
  }

};

#endif // __SPURV_UNIFORMS_IMPL
