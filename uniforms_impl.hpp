#ifndef __SPURV_UNIFORMS_IMPL
#define __SPURV_UNIFORMS_IMPL

#include "uniforms.hpp"

namespace spurv {
    
  /*
   * SUniformBinding member functions
   */

  template<typename... InnerTypes>
  SUniformBinding<InnerTypes...>::SUniformBinding(int sn, int bn) : SUniformBindingBase(sn, bn), value_pointers(sizeof...(InnerTypes), nullptr) {
    this->pointer_id = SUtils::getNewID();
  }
  
  template<typename... InnerTypes>
  template<int n>
  SValue<typename SUtils::NthType<n, InnerTypes...>::type >& SUniformBinding<InnerTypes...>::member() {
    static_assert(n < sizeof...(InnerTypes), "n too high!");
    static_assert(n >= 0, "n must be non-negative!");

    if (value_pointers[n] == nullptr) {
      int pointer_id = SUtils::getNewID();
      SUniformVar< typename SUtils::NthType<n, InnerTypes...>::type> *uniform =
	new SUniformVar< typename SUtils::NthType<n, InnerTypes...>::type>(this->set_no, this->binding_no,
									 n, pointer_id, this->pointer_id);
      this->value_pointers.push_back((void*)uniform);
      return *uniform;
    } else {
      return *(SValue<typename SUtils::NthType<n, InnerTypes...>::type>*)value_pointers[n];
    }
  }

  template<typename... InnerTypes>
  void SUniformBinding<InnerTypes...>::definePointer(std::vector<uint32_t>& bin,
							 std::vector<SDeclarationState*>& declaration_states) {
    SPointer<STORAGE_UNIFORM, SStruct<InnerTypes...> >::ensure_defined(bin, declaration_states);

    // OpVariable...
    SUtils::add(bin, (4 << 16) | 59);
    SUtils::add(bin, SPointer<STORAGE_UNIFORM, SStruct<InnerTypes...> >::getID());
    SUtils::add(bin, pointer_id);
    SUtils::add(bin, STORAGE_UNIFORM);
  }

  template<typename... InnerTypes>
  void SUniformBinding<InnerTypes...>::decorateType(std::vector<uint32_t>& bin) {
    SStruct<InnerTypes...>::ensure_decorated(bin);
  }

};

#endif // __SPURV_UNIFORMS_IMPL
