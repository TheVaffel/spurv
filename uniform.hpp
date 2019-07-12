#ifndef __SPURV_UNIFORM
#define __SPURV_UNIfORM

#include <declarations.hpp>

namespace spurv { 

  /*
   * SpurvUniformBinding member functions
   */

  template<typename... InnerTypes>
  SpurvUniformBinding<InnerTypes...>::SpurvUniformBinding(int sn, int bn) : SpurvUniformBindingBase(sn, bn), value_pointers(sizeof...(InnerTypes), nullptr) {
    this->pointer_id = Utils::getNewID();
  }
  
  template<typename... InnerTypes>
  template<int n>
  ValueNode<typename Utils::NthType<n, InnerTypes...>::type >& SpurvUniformBinding<InnerTypes...>::getBinding() {
    static_assert(n < sizeof...(InnerTypes), "n too high!");
    static_assert(n >= 0, "n must be non-negative!");

    if (value_pointers[n] == nullptr) {
      int pointer_id = Utils::getNewID();
      UniformVar< typename Utils::NthType<n, InnerTypes...>::type> *uniform =
	new UniformVar< typename Utils::NthType<n, InnerTypes...>::type>(this->set_no, this->binding_no,
									 n, pointer_id, this->pointer_id);
      this->value_pointers.push_back((void*)uniform);
      return *uniform;
    } else {
      return *(ValueNode<typename Utils::NthType<n, InnerTypes...>::type>*)value_pointers[n];
    }
  }

  template<typename... InnerTypes>
  void SpurvUniformBinding<InnerTypes...>::definePointer(std::vector<uint32_t>& bin,
							 std::vector<TypeDeclarationState*>& declaration_states) {
    SpurvPointer<SPURV_STORAGE_UNIFORM, SpurvStruct<InnerTypes...> >::ensure_defined(bin, declaration_states);

    // this->pointer_id = Utils::getNewID();
    
    // OpVariable...
    Utils::add(bin, (4 << 16) | 59);
    Utils::add(bin, SpurvPointer<SPURV_STORAGE_UNIFORM, SpurvStruct<InnerTypes...> >::getID());
    Utils::add(bin, pointer_id);
    Utils::add(bin, SPURV_STORAGE_UNIFORM);
  }

  template<typename... InnerTypes>
  void SpurvUniformBinding<InnerTypes...>::decorateType(std::vector<uint32_t>& bin) {
    SpurvStruct<InnerTypes...>::ensure_decorated(bin);
  }

};
#endif // __SPURV_UTIFORM
