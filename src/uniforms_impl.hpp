#ifndef __SPURV_UNIFORMS_IMPL
#define __SPURV_UNIFORMS_IMPL

#include "uniforms.hpp"

namespace spurv {
    
  /*
   * SStructBinding member functions
   */

  template<SStorageClass stind, typename... InnerTypes>
  SStructBinding<stind, InnerTypes...>::SStructBinding(int sn, int bn) : SUniformBindingBase(sn, bn), value_pointers(sizeof...(InnerTypes), nullptr) {
  }
  
  template<SStorageClass stind, typename... InnerTypes>
  template<int n>
  SValue<typename SUtils::NthType<n, InnerTypes...>::type >& SStructBinding<stind, InnerTypes...>::member() {
    static_assert(n < sizeof...(InnerTypes), "n too high!");
    static_assert(n >= 0, "n must be non-negative!");

    if (value_pointers[n] == nullptr) {
      int pointer_id = SUtils::getNewID();
      int nn = n;
      SUniformVar<stind, typename SUtils::NthType<n, InnerTypes...>::type> *uniform =
	SUtils::allocate<SUniformVar<stind, typename SUtils::NthType<n, InnerTypes...>::type> >(this->set_no, this->binding_no, nn, pointer_id, this->pointer_id);

      this->value_pointers.push_back((void*)uniform);
      return *uniform;
    } else {
      return *(SValue<typename SUtils::NthType<n, InnerTypes...>::type>*)value_pointers[n];
    }
  }

  template<SStorageClass stind, typename... InnerTypes>
  void SStructBinding<stind, InnerTypes...>::definePointer(std::vector<uint32_t>& bin,
							   std::vector<SDeclarationState*>& declaration_states) {
    SPointer<stind, SStruct<InnerTypes...> >::ensure_defined(bin, declaration_states);

    // OpVariable...
    SUtils::add(bin, (4 << 16) | 59);
    SUtils::add(bin, SPointer<stind, SStruct<InnerTypes...> >::getID());
    SUtils::add(bin, pointer_id);
    SUtils::add(bin, stind);
  }

  template<SStorageClass stind, typename... InnerTypes>
  void SStructBinding<stind, InnerTypes...>::decorateType(std::vector<uint32_t>& bin,
						   std::vector<bool*>& decoration_states) {
    SStruct<InnerTypes...>::ensure_decorated(bin, decoration_states);
  }

  
  /*
   * SUniformBinding constructor
   */
  
  template<typename... InnerTypes>
  SUniformBinding<InnerTypes...>::SUniformBinding(int sn, int bn) : SStructBinding<STORAGE_UNIFORM, InnerTypes...>(sn, bn) { }


  /*
   * SStorageBuffer constructor
   */

  template<typename... InnerTypes>
  SStorageBuffer<InnerTypes...>::SStorageBuffer(int sn, int bn) : SStructBinding<STORAGE_STORAGE_BUFFER, InnerTypes...>(sn, bn) { }

  
  /*
   * SUniformConstant member functions
   */

  template<typename type>
  SUniformConstant<type>::SUniformConstant(int sn, int bn) : SUniformBindingBase(sn, bn) { }

  template<typename type>
  void SUniformConstant<type>::definePointer(std::vector<uint32_t>& bin,
					     std::vector<SDeclarationState*>& declaration_states) {
    SPointer<STORAGE_UNIFORM_CONSTANT, type>::ensure_defined(bin, declaration_states);
    
    // OpVariable
    SUtils::add(bin, (4 << 16) | 59);
    SUtils::add(bin, SPointer<STORAGE_UNIFORM_CONSTANT, type>::getID());
    SUtils::add(bin, pointer_id);
    SUtils::add(bin, STORAGE_UNIFORM_CONSTANT);
  }

  template<typename type>
  void SUniformConstant<type>::ensure_type_defined(std::vector<uint32_t>& res, std::vector<SDeclarationState*>& declaration_states) {
    type::ensure_defined(res, declaration_states);
    SPointer<STORAGE_UNIFORM_CONSTANT, type>::ensure_defined(res, declaration_states);
  }

  template<typename type>
  void SUniformConstant<type>::define(std::vector<uint32_t>& res) {
    // OpLoad
    SUtils::add(res, (4 << 16) | 61);
    SUtils::add(res, type::getID());
    SUtils::add(res, this->getID());
    SUtils::add(res, this->pointer_id);
  }

};

#endif // __SPURV_UNIFORMS_IMPL
