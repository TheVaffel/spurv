
#include "constant_registry.hpp"

#include <map>
#include <algorithm> // pair
#include <tuple>

namespace spurv {

  std::map<std::tuple<int, int, int>, SDeclarationState> SConstantRegistry::integer_registry;
  std::map<std::pair<int, float>, SDeclarationState > SConstantRegistry::float_registry;

  
  bool SConstantRegistry::isDefinedInt(int n, int s, int m) {
    if(integer_registry.find(std::make_tuple(n, s, m)) != integer_registry.end()) {
      return integer_registry[std::make_tuple(n, s, m)].is_defined;
    }
    
    return false;
  }

  bool SConstantRegistry::isDefinedFloat(int n, float f) {
    if(float_registry.find(std::make_pair(n, f)) != float_registry.end()) {
      return float_registry[std::make_pair(n, f)].is_defined;
    }

    return false;
  }

  bool SConstantRegistry::isRegisteredInt(int n, int s, int m) {
    return integer_registry.find(std::make_tuple(n, s, m)) != integer_registry.end();
  }

  bool SConstantRegistry::isRegisteredFloat(int n, float f) {
    return float_registry.find(std::make_pair(n, f)) != float_registry.end();
  }

  void SConstantRegistry::registerInt(int n, int s, int m, int id) {
    if(isRegisteredInt(n, s, m) ) {
      printf("Tried to reregister int\n");
      exit(-1);
    } else {
      SDeclarationState state;
      state.id = id;
      integer_registry[std::make_tuple(n, s, m)] = state;
    }
  }

  void SConstantRegistry::registerFloat(int n, float f, int id) {
    if(isRegisteredFloat(n, f) ) {
      printf("Tried to reregister float\n");
      exit(-1);
    } else {
      SDeclarationState state;
      state.id = id;
      float_registry[std::make_pair(n, f)] = state;
    }
  }

  void SConstantRegistry::declareDefinedInt(int n, int s, int m) {
    if(!isRegisteredInt(n, s, m) ){
      printf("Tried to define unregistered int!\n");
      exit(-1);
    }

    if(isDefinedInt(n, s, m) ) {
      printf("Tride to redeclare defined of already defined int!\n");
      exit(-1);
    }

    integer_registry[std::make_tuple(n, s, m)].is_defined = true;
  }

  void SConstantRegistry::declareDefinedFloat(int n, float s) {
    if(!isRegisteredFloat(n, s) ) {
      printf("Tried to define unregistered float!\n");
      exit(-1);
    }

    if(isDefinedFloat(n, s)) {
      printf("Tried to redeclare defined of already defined float!\n");
      exit(-1);
    }

    float_registry[std::make_pair(n, s)].is_defined = true;
  }

  int SConstantRegistry::getIDInteger(int n, int s, int m) {
    if(!isRegisteredInt(n, s, m) ) {
      printf("Tried to get id of unregistered int\n");
      exit(-1);
    }
    
    return integer_registry[std::make_tuple(n, s, m)].id;
  }

  int SConstantRegistry::getIDFloat(int n, float f) {
    if(!isRegisteredFloat(n, f) ) {
      printf("Tried to get id of unregistered float\n");
      exit(-1);
    }
    
    return float_registry[std::make_pair(n, f)].id;
  }

  void SConstantRegistry::resetRegistry() {
    integer_registry.clear();
    float_registry.clear();
  }
  
};
