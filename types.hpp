#ifndef __SPURV_TYPES
#define __SPURV_TYPES

#include "declarations.hpp"

#include <FlatAlg.hpp>
namespace spurv {
  
  /* 
   * Static methods
   */
  
  template<SpurvTypeKind kind, int arg0, int arg1, typename... InnerTypes>
  void SpurvType<kind, arg0, arg1, InnerTypes...>::getDSpurvType(DSpurvType* type) {
      type->kind = kind;
      type->a0 = arg0;
      type->a1 = arg1;

      constexpr int n = sizeof...(InnerTypes);
      
      if constexpr (n > 0) {
	type->inner_types = new DSpurvType[n];
	Utils::getDSpurvTypesRecursive<InnerTypes...>(type->inner_types);
      }
    }

  template<SpurvTypeKind kind, int arg0, int arg1, typename... InnerTypes>
  int SpurvType<kind, arg0, arg1, InnerTypes...>::getID() {
    if(id == -1) {
      printf("Kind = %d, arg0 = %d, arg1 = %d\n", kind, arg0, arg1);
      printf("Tried to use type id before defined\n");
      // Fall through to catch errors other places..
    }
    return id;
  }

  template<SpurvTypeKind kind, int arg0, int arg1, typename... InnerTypes>
  constexpr SpurvTypeKind SpurvType<kind, arg0, arg1, InnerTypes...>::getKind() {
    return kind;
  }

  template<SpurvTypeKind kind, int arg0, int arg1, typename... InnerTypes>
  constexpr int SpurvType<kind, arg0, arg1, InnerTypes...>::getArg0() {
    return arg0;
  }

  template<SpurvTypeKind kind, int arg0, int arg1, typename... InnerTypes>
  constexpr int SpurvType<kind, arg0, arg1, InnerTypes...>::getArg1() {
    return arg1;
  }

  
  /*
   * Static variables
   */
  
  template<SpurvTypeKind kind, int arg0, int arg1, typename... InnerTypes>
  int SpurvType<kind, arg0, arg1, InnerTypes...>::id = -1;

  
  /*
   * Default Member functions
   */

  template<SpurvTypeKind kind, int arg0, int arg1, typename... InnerTypes>
  void SpurvType<kind, arg0, arg1, InnerTypes...>::ensure_defined_dependencies(std::vector<uint32_t>& bin,
									    std::vector<int*>& ids) { }
  
  /*
   * Int member functions
   */
  
  template<int n, int signedness>
  void SpurvInt<n, signedness>::define(std::vector<uint32_t>& bin) {
    SpurvInt<n, signedness>::id = Utils::getNewID();
    Utils::add(bin, (4 << 16) | 21);
    Utils::add(bin, SpurvInt<n, signedness>::id);
    Utils::add(bin, n); // Width
    Utils::add(bin, signedness); // 0 = unsigned, 1 = signed
  }

  template<int n, int signedness>
  void SpurvInt<n, signedness>::ensure_defined(std::vector<uint32_t>& bin, std::vector<int*>& ids) {
    if(SpurvType<SPURV_TYPE_INT, n, signedness>::id < 0) {
      define(bin);
      ids.push_back(&(SpurvType<SPURV_TYPE_INT, n, signedness>::id));
    }
  }

  
  /*
   * Float member functions
   */
  
  template<int n>
  void SpurvFloat<n>::define(std::vector<uint32_t>& bin) {
    SpurvFloat<n>::id = Utils::getNewID();
    Utils::add(bin, (3 << 16) | 22);
    Utils::add(bin, SpurvType<SPURV_TYPE_FLOAT, n>::id);
    Utils::add(bin, n);
  }
  
  template<int n>
  void SpurvFloat<n>::ensure_defined(std::vector<uint32_t>& bin, std::vector<int*>& ids) {
    if(SpurvType<SPURV_TYPE_FLOAT, n>::id < 0) {
      define(bin);
      ids.push_back(&(SpurvType<SPURV_TYPE_FLOAT, n>::id));
    }
  }


  /*
   * Mat member functions
   */
  
  template<int n, int m>
  void SpurvMat<n, m>::ensure_defined_dependencies(std::vector<uint32_t>& bin,
						   std::vector<int*>& ids) {
    if constexpr(m == 1) {
	SpurvFloat<32>::ensure_defined(bin, ids);
      } else {
      SpurvType<SPURV_TYPE_MAT, n, 1>::ensure_defined(bin, ids);
    }
  }

  template<int n, int m>
  void SpurvMat<n, m>::ensure_defined(std::vector<uint32_t>& bin,
				      std::vector<int*>& ids) {
    if(SpurvType<SPURV_TYPE_MAT, n, m>::id < 0) {
      ensure_defined_dependencies(bin, ids);
      define(bin);
      ids.push_back(&(SpurvType<SPURV_TYPE_MAT, n, m>::id));
    }
  }
  
  template<int n, int m>
  void SpurvMat<n, m>::define(std::vector<uint32_t>& bin) {
    SpurvMat<n, m>::id = Utils::getNewID();
    if constexpr(m == 1) {
	Utils::add(bin, (4 << 16) | 23);
	Utils::add(bin, SpurvType<SPURV_TYPE_MAT, n, 1>::getID());
	Utils::add(bin, SpurvType<SPURV_TYPE_FLOAT, 32>::getID());
	Utils::add(bin, n);
      } else {
      Utils::add(bin, (4 << 16) | 24);
      Utils::add(bin, SpurvType<SPURV_TYPE_MAT, n, m>::getID());
      Utils::add(bin, SpurvType<SPURV_TYPE_MAT, n, 1>::getID());
      Utils::add(bin, m);
    }
  }

  /*
   * Arr member functions
   */
  template<int n, typename tt>
  void SpurvArr<n, tt>::ensure_defined_dependencies(std::vector<uint32_t>& bin,
						    std::vector<int*>& ids) {
    tt::ensure_defined(bin, ids);
  }

  template<int n, typename tt>
  void SpurvArr<n, tt>::ensure_defined(std::vector<uint32_t>& bin, std::vector<int*>& ids) {
      if(SpurvType<SPURV_TYPE_ARR, n, 0, tt>::id < 0) {
	ensure_defined_dependencies(bin, ids);
	define(bin);
	ids.push_back(&(SpurvType<SPURV_TYPE_ARR, n, 0, tt>::id));
      }
    }
  
  template<int n, typename tt>
  void SpurvArr<n, tt>::define(std::vector<uint32_t>& bin) {
      SpurvArr<n, tt>::id = Utils::getNewID();
      Utils::add(bin, (4 << 16) | 28);
      Utils::add(bin, SpurvType<SPURV_TYPE_ARR, n, 0, tt>::id);
      Utils::add(bin, tt::getID());
      Utils::add(bin, n);
    }

  /*
   * Pointer member functions
   */

  template<SpurvStorageClass storage, typename tt>
  void SpurvPointer<storage, tt>::ensure_defined_dependencies(std::vector<uint32_t>& bin,
							      std::vector<int*>& ids) {
    tt::ensure_defined(bin, ids);
  }

  template<SpurvStorageClass storage, typename tt>
  void SpurvPointer<storage, tt>::ensure_defined(std::vector<uint32_t>& bin, std::vector<int*>& ids) {
    if(SpurvType<SPURV_TYPE_POINTER, (int)storage, 0, tt>::id < 0) {
      ensure_defined_dependencies(bin, ids);
      define(bin);
      ids.push_back(&(SpurvType<SPURV_TYPE_POINTER, (int)storage, 0, tt>::id));
    }
  }

  template<SpurvStorageClass storage, typename tt>
  void SpurvPointer<storage, tt>::define(std::vector<uint32_t>& bin) {
    SpurvPointer<storage, tt>::id = Utils::getNewID();
    Utils::add(bin, (4 << 16) | 32);
    Utils::add(bin, SpurvType<SPURV_TYPE_POINTER, (int)storage, 0, tt>::id);
    Utils::add(bin, (int)storage);
    Utils::add(bin, tt::getID());
  }


  /*
   * Struct member variables
   */ 

  template<typename... InnerTypes>
  bool SpurvStruct<InnerTypes...>::is_decorated = false;
  
  
  /*
   * Struct member functions
   */
  
  template<typename... InnerTypes>
  void SpurvStruct<InnerTypes...>::ensure_defined_dependencies(std::vector<uint32_t>& bin,
							       std::vector<int*>& ids) {
    Utils::ensureDefinedRecursive<InnerTypes...>(bin, ids);
  }

  template<typename... InnerTypes>
  void SpurvStruct<InnerTypes...>::ensure_defined(std::vector<uint32_t>& bin,
						   std::vector<int*>& ids) {
    if(SpurvStruct<InnerTypes...>::id < 0) {
      ensure_defined_dependencies(bin, ids);
      define(bin);
      ids.push_back(&(SpurvStruct<InnerTypes...>::id));
    }
  }

  template<typename... InnerTypes>
  void SpurvStruct<InnerTypes...>::define(std::vector<uint32_t>& bin) {
    SpurvStruct<InnerTypes...>::id = Utils::getNewID();
    Utils::add(bin, ((2 + sizeof...(InnerTypes)) << 16) | 30);
    Utils::add(bin, SpurvStruct<InnerTypes...>::id);
    Utils::addIDsRecursive<InnerTypes...>(bin);
  }

  template<typename... InnerTypes>
  void SpurvStruct<InnerTypes...>::getDSpurvType(DSpurvType* type) {
    constexpr int n = sizeof...(InnerTypes);
    type->kind = SPURV_TYPE_STRUCT;
    type->inner_types = new DSpurvType[sizeof...(InnerTypes)];
    Utils::getDSpurvTypesRecursive<InnerTypes...>(type);
  }

  template<typename... InnerTypes>
  void SpurvStruct<InnerTypes...>::ensure_decorated(std::vector<uint32_t>& bin) {
    if( is_decorated) {
      return;
    }

    is_decorated = true;

    // OpDecorate <type id> Block
    Utils::add(bin, (3 << 16) | 71);
    Utils::add(bin, SpurvStruct<InnerTypes...>::id);
    Utils::add(bin, 2);
  }
  
  
  /*
   * Type mapper
   */
  
  template<>
  struct MapSpurvType<void> {
    typedef void_s type;
  };

  template<>
  struct MapSpurvType<int32_t> {
    typedef int_s type;
  };

  template<>
  struct MapSpurvType<uint32_t> {
    typedef uint_s type;
  };

  template<>
  struct MapSpurvType<float> {
    typedef float_s type;
  };

  template<int n, int m>
  struct MapSpurvType<Matrix<n, m> > {
    typedef SpurvType<SPURV_TYPE_MAT, n, m> type;
  };


  
};

#endif // __SPURV_TYPES
