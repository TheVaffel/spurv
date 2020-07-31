#include "pointers.hpp"

namespace spurv {

  /*
   * SPointerBase member functions
   */

  SPointerBase::SPointerBase() {
    this->id = SUtils::getNewID();
    this->is_defined = false;
  }

  int SPointerBase::getID() {
    return this->id;
  }
  
  void SPointerBase::ensure_defined(std::vector<uint32_t>& res) {

    if(!this->is_defined) {
      this->define(res);
    }
    
    this->is_defined = true;
  }

};
