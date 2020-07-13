#include "variable_registry.hpp"

namespace spurv {

  /*
   * SVariableRegistry static members
   */

  std::vector<SVariableEntryBase*> SVariableRegistry::variables = std::vector<SVariableEntryBase*>();


  /*
   * SVariableRegistry static functions
   */

  void SVariableRegistry::write_variable_definitions(std::vector<uint32_t>& bin) {
    for(SVariableEntryBase* vb : SVariableRegistry::variables) {
      vb->write_definition(bin);
    }
  }

  void SVariableRegistry::clear() {
    for(SVariableEntryBase* vb : SVariableRegistry::variables) {
      delete vb;
    }

    SVariableRegistry::variables.clear();
  }
};
