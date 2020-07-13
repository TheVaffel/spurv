#ifndef __SPURV_VARIABLE_REGISTRY_IMPL
#define __SPURV_VARIABLE_REGISTRY_IMPL

#include "variable_registry.hpp"

namespace spurv {

  /*
   * SVariableEntry member functions
   */

  template<typename tt>
  SVariableEntry<tt>::SVariableEntry(SLocal<tt>* local) {
    this->variable = local;
  }
  
  template<typename tt>
  void SVariableEntry<tt>::write_definition(std::vector<uint32_t>& bin) {
    this->variable->ensure_defined(bin);
  }


  /*
   * SVariableRegistry static functions
   */

  template<typename tt>
  void SVariableRegistry::add_variable(SLocal<tt>* local) {
    SVariableEntry<tt>* ent = new SVariableEntry(local);
    SVariableRegistry::variables.push_back(ent);
  }
};

#endif // __SPURV_VARIABLE_REGISTRY_IMPL
