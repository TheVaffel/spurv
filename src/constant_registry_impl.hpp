#ifndef __SPURV_CONSTANT_REGISTRY_IMPL
#define __SPURV_CONSTANT_REGISTRY_IMPL

#include "constant_registry.hpp"

namespace spurv {
  
  /*
   * Static methods 
   */
  
  template<typename tt>
  int SConstantRegistry::ensureDefinedConstant(const tt& val, int id,
					       std::vector<uint32_t>& res) {
    using st = typename MapSType<tt>::type;

    static_assert(st::getKind() == KIND_INT || st::getKind() == KIND_FLOAT,
		  "Constant definitions only allowed for ints and floats for now");

    static_assert(st::getArg0() == 32,
		  "Constant definitions of float or integer type must have bit depth 32, for now");

    int n_id;
    
    if constexpr(st::getKind() == KIND_INT) {

	if(isDefinedInt(st::getArg0(), st::getArg1(), val)) {
	  return getIDInteger(st::getArg0(), st::getArg1(), val);;
	}
	if(isRegisteredInt(st::getArg0(), st::getArg1(), val)) {
	  n_id = getIDInteger(st::getArg0(), st::getArg1(), val);
	} else {
	  registerInt(st::getArg0(), st::getArg1(), val, id);
	  n_id = id;
	}
	
	declareDefinedInt(st::getArg0(), st::getArg1(), val);
	
    } else if constexpr(st::getKind() == KIND_FLOAT) {
	if(isDefinedFloat(st::getArg0(), val)) {
	  return getIDFloat(st::getArg0(), val);
	}

	if(isRegisteredFloat(st::getArg0(), val)) {
	  n_id = getIDFloat(st::getArg0(), val);
	} else {
	  registerFloat(st::getArg0(), val, id);
	  n_id = id;
	}
	
	declareDefinedFloat(st::getArg0(), val);
        
    }
    
    if(st::getID() < 0) {
      printf("Tried to define constant before its type was defined!\n");
      exit(-1);
    }

    // OpConstant
    SUtils::add(res, (4 << 16) | 43);
    SUtils::add(res, st::getID());
    SUtils::add(res, n_id);
    SUtils::add(res, *(uint32_t*)&val);

    return n_id;
  }
};

#endif // __SPURV_CONSTANT_REGISTRY_IMPL
