#ifndef __SPURV_CONSTANT_REGISTRY_IMPL
#define __SPURV_CONSTANT_REGISTRY_IMPL

#include "constant_registry.hpp"

namespace spurv {
  
  /*
   * Static methods 
   */
  
  template<typename tt>
  void ConstantRegistry::ensureDefinedConstant(const tt& val, int id,
					       std::vector<uint32_t>& res) {
    using st = typename MapSpurvType<tt>::type;

    static_assert(st::getKind() == SPURV_TYPE_INT || st::getKind() == SPURV_TYPE_FLOAT,
		  "Constant definitions only allowed for ints and floats for now");

    static_assert(st::getArg0() == 32,
		  "Constant definitions of float or integer type must have bit depth 32, for now");

    
    if constexpr(st::getKind() == SPURV_TYPE_INT) {
	
      if(isDefinedInt(st::getArg0(), st::getArg1(), val)) {
	return;
      } else {
	registerInt(st::getArg0(), st::getArg1(), val, id);
      }
	
    } else if constexpr(st::getKind() == SPURV_TYPE_FLOAT) {
	
      if(isDefinedFloat(st::getArg0(), val)) {
        return;
      } else {
	registerFloat(st::getArg0(), val, id);
      }
	  
    }
    
    if(st::getID() < 0) {
      printf("Tried to define constant before its type was defined!\n");
      exit(-1);
    }

    // OpConstant
    Utils::add(res, (4 << 16) | 43);
    Utils::add(res, st::getID());
    Utils::add(res, id);
    Utils::add(res, *(uint32_t*)&val);

  }
};

#endif // __SPURV_CONSTANT_REGISTRY_IMPL
