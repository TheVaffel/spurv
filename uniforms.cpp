#include "uniforms.hpp"

namespace spurv {

  /*
   * SpurvUniformBindingBase member functions
   */
  
  SpurvUniformBindingBase::SpurvUniformBindingBase(int ns, int nb) : set_no(ns), binding_no(nb) {
    pointer_id = Utils::getNewID();
  }

  int SpurvUniformBindingBase::getSetNo() {
    return this->set_no;
  }

  int SpurvUniformBindingBase::getBindingNo() {
    return this->binding_no;
  }

  int SpurvUniformBindingBase::getPointerID() {
    return this->pointer_id;
  }
  
};
