#ifndef __SPURV_CONSTANT_REGISTRY
#define __SPURV_CONSTANT_REGISTRY

#include "declarations.hpp"
#include "types.hpp"

namespace spurv {

  /*
   * SConstantRegistry - Class holding static data structures to ensure each scalar constant is only defined once
   */
  
  class SConstantRegistry {
    // Tuples for ints contain <data type size, signedness, constant> maps to id
    // Pairs for floats contain <data type size, constant>, maps to id
    static std::map<std::tuple<int, int, int>, SDeclarationState> integer_registry;
    static std::map<std::pair<int, float>, SDeclarationState > float_registry;

  public:
    
    static void registerInt(int n, int s, int m, int id);
    static void registerFloat(int n, float f, int id);
    
    static void declareDefinedInt(int n, int s, int m);
    static void declareDefinedFloat(int n, float s);
    
    static bool isDefinedInt(int n, int s, int m);
    static bool isDefinedFloat(int n, float f);

    static bool isRegisteredInt(int n, int s, int m);
    static bool isRegisteredFloat(int n, float f);

    static int getIDInteger(int n, int s, int m);
    static int getIDFloat(int n, float f);

    // Returns the registered id if different from supplied id
    template<typename nt> 
    static int ensureDefinedConstant(const nt& val, int id,
				     std::vector<uint32_t>& res);
    
    static void resetRegistry();
  };

};

#endif // __SPURV_CONSTANT_REGISTRY
