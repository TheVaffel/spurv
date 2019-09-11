#ifndef __SPURV_VARIABLE_REGISTRY
#define __SPURV_VARIABLE_REGISTRY

#include "declarations.hpp"

namespace spurv {

  struct VariableEntry {
    uint32_t variable_id;
    uint32_t type_id;
  };
  
  class SVariableRegistry {

    static std::vector<VariableEntry> entries;

    static void addVariable(uint32_t variable_id, uint32_t type_id);

    static void resetVariables();

    static std::vector<VariableEntry>* getVector();

    template<typename tt>
    friend class SelectConstruct;
    
    template<SShaderType type, typename... InputTypes>
    friend class SShader;
  };

};

#endif // __SPURV_VARIABLE_REGISTRY
