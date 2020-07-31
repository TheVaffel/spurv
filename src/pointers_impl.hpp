#ifndef __SPURV_POINTERS_IMPL
#define __SPURV_POINTERS_IMPL

#include "pointers.hpp"

namespace spurv {

  /*
   * SPointerTypeBase member functions
   */

  template<typename tt>
  template<typename t1>
  void SPointerTypeBase<tt>::store(t1&& val) {
    static_assert(SValueWrapper::does_wrap<t1, tt>::value,
		  "[spurv::SPointerVar::store] Cannot convert store value to desired value");
    SStoreEvent<tt> *ev = SEventRegistry::addStore<tt>(this);
    ev->val_p = &SValueWrapper::unwrap_to<t1, tt>(val);
  }

  
  /*
   * SPointerVar member functions
   */
  

  template<typename tt, SStorageClass storage>
  SValue<tt>& SPointerVar<tt, storage>::load() {
    SLoadEvent<tt> *ev = SEventRegistry::addLoad<tt>(this->id);
    SLoadedVal<tt, storage> *val = SUtils::allocate<SLoadedVal<tt, storage> >(this);
    ev->val_p = val;

    return *val;
  }

  template<typename tt, SStorageClass storage>
  void SPointerVar<tt, storage>::define(std::vector<uint32_t>& res) {
    // OpVariable
    SUtils::add(res, (4 << 16) | 59);
    SUtils::add(res, SPointer<storage, tt>::getID());
    SUtils::add(res, this->id);
    SUtils::add(res, storage);
  }

  template<typename tt, SStorageClass storage>
  void SPointerVar<tt, storage>::ensure_type_defined(std::vector<uint32_t>& res,
					std::vector<SDeclarationState*>& declaration_states) {
    SPointer<storage, tt>::ensure_defined(res, declaration_states);

    // If not a local variable, declare pointer in type-declaration section
    if constexpr(storage != SStorageClass::STORAGE_FUNCTION) {
	this->ensure_defined(res);
      }
  }

  template<typename tt, SStorageClass storage>
  int SPointerVar<tt, storage>::getChainLength() {
    return 1; // Count this (the base pointer) as the first link
  }

  template<typename tt, SStorageClass storage>
  void SPointerVar<tt, storage>::outputChainNumber(std::vector<uint32_t>& res) {
    SUtils::add(res, this->id); // Add this ID as base pointer
  }

  template<typename tt, SStorageClass storage>
  template<int n>
  SAccessChain<typename member_access_result<tt, n>::type, storage>& SPointerVar<tt, storage>::member() {
    using rtype = typename member_access_result<tt, n>::type;

    return *SUtils::allocate<SAccessChain<rtype, storage> >(this, n);
  }

  template<typename tt, SStorageClass storage>
  template<typename tind>
  SAccessChain<typename index_access_result<tt>::type, storage>& SPointerVar<tt, storage>::operator[](tind&& ind) {
    using rtype = typename index_access_result<tt>::type;
    
    return *SUtils::allocate<SAccessChain<rtype, storage> >(this, ind);
  }


  /*
   * SAccessChain member functions
   */

  template<typename tt, SStorageClass storage>
  template<typename tind>
  SAccessChain<tt, storage>::SAccessChain(SPointerBase* base, tind&& index) {
    this->acb = base;
    this->index_value = &SValueWrapper::unwrap_to<tind, int_s>(index);
  }

  template<typename tt, SStorageClass storage>
  void SAccessChain<tt, storage>::define(std::vector<uint32_t>& res) {

    this->acb->ensure_defined(res);

    int chain_length = this->getChainLength();
    // OpAccessChain
    SUtils::add(res, ((3 + chain_length) << 16) | 65); // Can be substituted for opcode 66 / OpInBoundsAccessChain?
    SUtils::add(res, SPointer<storage, tt>::getID());
    SUtils::add(res, this->id);

    this->outputChainNumber(res);
  }

  template<typename tt, SStorageClass storage>
  int SAccessChain<tt, storage>::getChainLength() {
    return 1 + this->acb->getChainLength();
  }

  template<typename tt, SStorageClass storage>
  void SAccessChain<tt, storage>::outputChainNumber(std::vector<uint32_t>& res) {
    this->acb->outputChainNumber(res);
    SUtils::add(res, this->index_value->getID());
  }

  template<typename tt, SStorageClass storage>
  void SAccessChain<tt, storage>::ensure_type_defined(std::vector<uint32_t>& res,
						      std::vector<SDeclarationState*>& declaration_states) {
    this->acb->ensure_type_defined(res, declaration_states);
    this->index_value->ensure_type_defined(res, declaration_states);
    
    SPointer<storage, tt>::ensure_defined(res, declaration_states);
  }

  /*
   * SLoadedVal member functions
   */

  template<typename tt, SStorageClass storage>
  SLoadedVal<tt, storage>::SLoadedVal(SPointerVar<tt, storage>* pointer) : pointer(pointer) {
  }

  template<typename tt, SStorageClass storage>
  void SLoadedVal<tt, storage>::define(std::vector<uint32_t>& res) {
    this->pointer->ensure_defined(res);

    // OpLoad
    SUtils::add(res, (4 << 16) | 61);
    SUtils::add(res, tt::getID());
    SUtils::add(res, this->id);
    SUtils::add(res, this->pointer->getID());
  }

  template<typename tt, SStorageClass storage>
  void SLoadedVal<tt, storage>::ensure_type_defined(std::vector<uint32_t>& res,
						    std::vector<SDeclarationState*>& declaration_states) {
    
    this->pointer->ensure_type_defined(res, declaration_states);
    
  }
  
};

#endif // __SPURV_POINTERS_IMPL
