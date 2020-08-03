#include "uniforms.hpp"

namespace spurv {

  /*
   * SUniformBindingBase member functions
   */
  
  SUniformBindingBase::SUniformBindingBase(int ns, int nb) : set_no(ns), binding_no(nb) { }

  int SUniformBindingBase::getSetNo() {
    return this->set_no;
  }

  int SUniformBindingBase::getBindingNo() {
    return this->binding_no;
  }

  void SUniformBindingBase::decorateType(std::vector<uint32_t>& bin,
					 std::vector<bool*>& decoration_states) { }
};
