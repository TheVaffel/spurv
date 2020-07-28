#ifndef __SPURV_POINTERS_IMPL
#define __SPURV_POINTERS_IMPL

#include "pointers.hpp"

namespace spurv {

  /*
   * SPointerBase member functions
   */

  SPointerBase::SPointerBase() {
    this->id = SUtils::getNewID();
    this->is_defined = false;
  }
  
  void SPointerBase::ensure_defined(std::vector<uint32_t>& res) {

    if(!this->is_defined) {
      this->define(res);
    }
    
    this->is_defined = true;
  }


  /*
   * SPointerBase member functions
   */
  
  template<typename tt>
  SValue<tt>& SPointerBase::load() {
    SLoadEvent<tt> *ev = SEventRegistry::addLoad<tt>(this->id);
    SLoadedVal<tt> *val = SUtils::allocate<SLoadedVal<tt> >(this);
    ev->val_p = val;

    return *val;
  }

  template<typename tt>
  template<typename t1>
  void SPointerBase::store(t1&& val) {
    static_assert(SValueWrapper::does_wrap<t1, tt>::value,
		  "[spurv::SPointerVar::store] Cannot convert store value to desired value");
    SStoreEvent<tt> *ev = SEventRegistry::addStore<tt>(this);
    ev->val_p = &SValueWrapper::unwrap_to<t1, tt>(val);
  }

  
  /*
   * SPointerVar member functions
   */

  template<typename tt, SStorageClass storage>
  void SPointerVar::define(std::vector<uint32_t>& res) {
    // OpVariable
    SUtils::add(res, (4 << 16) | 59);
    SUtils::add(res, SPointer<storage, tt>::getID());
    SUtils::add(res, this->id);
    SUtils::add(res, storage);
  }

  template<typename tt, SStorageClass storage>
  void SPointerVar::ensure_type_defined(std::vector<uint32_t>& res,
					std::vector<SDeclarationState*>& declaration_states) {
    SPointer<storage, tt>::ensure_defined(res, delcaration_states);

    // If not a local variable, declare pointer in type-declaration section
    if constexpr(storage != SStorageClass::STORAGE_FUNCTION) {
	this->ensure_defined(res);
      }
  }

  template<typename tt, SStorageClass storage>
  int SPointerVar::getChainLength() {
    return 1; // Count this (the base pointer) as the first link
  }

  template<typename tt, SStorageClass storage>
  void SPointerVar::outputChainNumber(std::vector<uint32_t>& res) {
    SUtils::add(res, this->id); // Add this ID as base pointer
  }

  template<typename tt, SStorageClass storage>
  template<int n>
  SAccessChain<typename member_access_result<tt, n>::type, storage>& member() {
    using rtype = typename member_access_result<tt, n>::type;

    return *SUtils::allocate<SAccessChain<rtype, storage> >(this, n);
  }

  template<typename tt, SStorage storage>
  template<typename tind>
  SAccessChain<typename index_access_result<tt>::type, storage>& operator[](tind&& ind) {
    using rtype = typename index_access_result<tt>::type;
    
    return *SUtils::allocate<SAccessChain<rtype, storage> >(this, ind);
  }


  /*
   * SAccessChain member functions
   */

  template<typename tt, SStorageClass storage>
  template<typename tind>
  SAccessChain<tt, storage>::SAccessChain(SAccessChainBase* base, tind&& index) {
    this->acb = base;
    this->index_value = &SValueWrapper::unwrap_to<tind, int_s>(index);
  }

  template<typename tt, SStorageClass storage>
  void SAccessChain<tt, storage>::define(std::vector<uint32_t>& res) {

    int chain_length = this->getChainLength();
    
    // OpAccessChain
    SUtils::add(res, ((3 + chain_length) << 16) | 65); // Can be substituted for opcode 66 / OpInBoundsAccessChain?
    SUtils::add(res, tt::getID());
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
    SUtils::add(res, this->index_value);
  }
  

  /*
   * SLoadedVal member functions
   */

  template<typename tt, SStorageClass storage>
  SLoadedVal::SLoadedVal(SPointerVal<tt, storage>* pointer) : pointer(pointer) { }

  template<typename tt, SStorageClass storage>
  void SLoadedVal::define(std::vector<uint32_t>& res) {
    this->pointer->ensure_defined(res);

    // OpLoad
    SUtils::add(res, (4 << 16) | 61);
    SUtils::add(res, tt::getID());
    SUtils::add(res, this->id);
    SUtils::add(res, this->pointer->getID());
  }

  template<typename tt, SStorageClass storage>
  void ensure_type_defined(std::vector<uint32_t>& res,
			   std::vector<SDeclarationState*>& declaration_states) {
    
    this->pointer->ensure_type_defined(res, declaration_states);
    
  }
  
};

#endif // __SPURV_POINTERS_IMPL
