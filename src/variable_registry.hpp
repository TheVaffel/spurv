#ifndef __SPURV_VARIABLE_REGISTRY
#define __SPURV_VARIABLE_REGISTRY

#include "values.hpp"

#include <cstdint>
#include <vector>

namespace spurv {

  /*
   * SVariableEntryBase - base class for SVariableEntry's
   */

  class SVariableEntryBase {

    virtual void write_definition(std::vector<uint32_t>& bin) = 0;

    friend class SVariableRegistry;
  };

  
  /*
   * SVariableEntry - maintains information about a single variable
   */

  template<typename tt>
  class SVariableEntry : SVariableEntryBase {
    SLocal<tt>* variable;

    virtual void write_definition(std::vector<uint32_t>& bin);

    SVariableEntry(SLocal<tt>* local);

    friend class SVariableRegistry;
  };
  
  
  /*
   * SVariableRegistry - class to maintain the list of local variables, 
   * so that they can be outputput in the first block of the function
   */
  
  class SVariableRegistry {
    static std::vector<SVariableEntryBase*> variables;

    static void write_variable_definitions(std::vector<uint32_t>& bin);

    template<typename tt>
    static void add_variable(SLocal<tt>* var);

    static void clear();

    template<SShaderType type, typename... Inputs>
    friend class SShader;

    template<typename tt>
    friend class SLocal;
  };
};

#endif // __SPURV_VARIABLE_REGISTRY
