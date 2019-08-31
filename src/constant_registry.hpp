#ifndef __SPURV_CONSTANT_REGISTRY
#define __SPURV_CONSTANT_REGISTRY

#include "declarations.hpp"

namespace spurv {

  /*
   * SConstantRegistry - Class holding static data structures to ensure each scalar constant is only defined once
   */
  
  class SConstantRegistry {
    // Tuples for ints contain <data type size, signedness, constant> maps to id
    // Pairs for floats contain <data type size, constant>, maps to id
    static std::map<std::tuple<int, int, int>, int> integer_registry;
    static std::map<std::pair<int, float>, int > float_registry;

    static void registerInt(int n, int s, int m, int id);
    static void registerFloat(int n, float f, int id);
    
  public:

    static bool isDefinedInt(int n, int s, int m);
    static bool isDefinedFloat(int n, float f);

    static int getIDInteger(int n, int s, int m);
    static int getIDFloat(int n, float f);

    template<typename nt> 
    static void ensureDefinedConstant(const nt& val, int id,
				      std::vector<uint32_t>& res);
    
    static void resetRegistry();
  };

};

#endif // __SPURV_CONSTANT_REGISTRY
