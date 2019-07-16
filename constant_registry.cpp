
#include "constant_registry.hpp"

#include <map>
#include <algorithm> // pair
#include <tuple>

namespace spurv {

  std::map<std::tuple<int, int, int>, int> ConstantRegistry::integer_registry;
  std::map<std::pair<int, float>, int > ConstantRegistry::float_registry;

  
  bool ConstantRegistry::isDefinedInt(int n, int s, int m) {
    return integer_registry.find(std::make_tuple(n, s, m)) != integer_registry.end();
  }

  bool ConstantRegistry::isDefinedFloat(int n, float f) {
    return float_registry.find(std::make_pair(n, f)) != float_registry.end();
  }

  void ConstantRegistry::registerInt(int n, int s, int m, int id) {
    if(isDefinedInt(n, s, m) ) {
      printf("Tried to redefine int\n");
      exit(-1);
    } else {
      integer_registry[std::make_tuple(n, s, m)] = id;
    }
  }

  void ConstantRegistry::registerFloat(int n, float f, int id) {
    if(isDefinedFloat(n, f) ) {
      printf("Tried to redefine float\n");
      exit(-1);
    } else {
      float_registry[std::make_pair(n, f)] = id;
    }
  } 

  int ConstantRegistry::getIDInteger(int n, int s, int m) {
    if(!isDefinedInt(n, s, m) ) {
      printf("Tried to get id of unregistered int\n");
      exit(-1);
    }
    
    return integer_registry[std::make_tuple(n, s, m)];
  }

  int ConstantRegistry::getIDFloat(int n, float f) {
    if(!isDefinedFloat(n, f) ) {
      printf("Tried to get id of unregistered float\n");
      exit(-1);
    }
    
    return float_registry[std::make_pair(n, f)];
  }

  void ConstantRegistry::resetRegistry() {
    integer_registry.clear();
    float_registry.clear();
  }
  
};
