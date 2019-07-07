#include "types.hpp"

namespace spurv {

  /*
   * NullType member functions
   */
  
  void NullType::ensure_defined(std::vector<uint32_t>&,
			     std::vector<int32_t*>&) {}
  void NullType::define(std::vector<uint32_t>&) {}

  
  /*
   * DSpurvType member functions
   */
  
  DSpurvType::~DSpurvType() {
    if (this->inner_type) {
      if(this->kind == SPURV_TYPE_STRUCT) {
	delete[] this->inner_type;
      } else {
	delete this->inner_type;
      }
    }
  }
  
  bool DSpurvType::operator==(const DSpurvType& ds) const {
    bool a = true;
    a = a && this->kind == ds.kind;
    a = a && this->a0 == ds.a0;
    a = a && this->a1 == ds.a1;
    if(this->inner_type && ds.inner_type) {
      return a && (*this->inner_type == *ds.inner_type);
    } else if( this->inner_type || ds.inner_type) {
      return false;
    }
      
    return a;
  }

  
  /*
   * Void member functions
   */
  
  template<>
  void SpurvType<SPURV_TYPE_VOID>::define(std::vector<uint32_t>& bin) {
    id = Utils::getNewID();
    Utils::add(bin, (2 << 16) | 19);
    Utils::add(bin, SpurvType<SPURV_TYPE_VOID>::id);
  }

  template<>
  void SpurvType<SPURV_TYPE_VOID>::ensure_defined(std::vector<uint32_t>& bin, std::vector<int*>& ids) {
    if( SpurvType<SPURV_TYPE_VOID>::id < 0) {
      define(bin);
      ids.push_back(&(SpurvType<SPURV_TYPE_VOID>::id));
    }
  }
};
