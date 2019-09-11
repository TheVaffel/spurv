
#include "variable_registry.hpp"

#include <vector>

namespace spurv {

  std::vector<VariableEntry> SVariableRegistry::entries;
  
  void SVariableRegistry::addVariable(uint32_t variable_id,
				      uint32_t type_id) {
    VariableEntry entry;
    entry.variable_id = variable_id;
    entry.type_id = type_id;

    SVariableRegistry::entries.push_back(entry);
  }

  void SVariableRegistry::resetVariables() {
    SVariableRegistry::entries.clear();
  }

  std::vector<VariableEntry>* SVariableRegistry::getVector() {
    return &SVariableRegistry::entries;
  }
};
