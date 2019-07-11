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
    if(declarationState.id == -1 || !isDefined()) {
      printf("Kind = %d, arg0 = %d, arg1 = %d\n", kind, arg0, arg1);
      printf("Tried to use type declarationState.id before defined\n");
      // Fall through to catch errors other places..
    }
    return declarationState.id;
  }

  template<SpurvTypeKind kind, int arg0, int arg1, typename... InnerTypes>
  int SpurvType<kind, arg0, arg1, InnerTypes...>::ensureInitID() {
    if(declarationState.id == -1) {
      declarationState.id = Utils::getNewID();
    }
    return declarationState.id;
  }

  template<SpurvTypeKind kind, int arg0, int arg1, typename... InnerTypes>
  bool SpurvType<kind, arg0, arg1, InnerTypes...>::isDefined() {
    return declarationState.is_defined;
  }

  template<SpurvTypeKind kind, int arg0, int arg1, typename... InnerTypes>
  void SpurvType<kind, arg0, arg1, InnerTypes...>::declareDefined() {
    declarationState.is_defined = true;
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
  TypeDeclarationState SpurvType<kind, arg0, arg1, InnerTypes...>::declarationState;
  
  
  /*
   * Default Member functions
   */

  template<SpurvTypeKind kind, int arg0, int arg1, typename... InnerTypes>
  void SpurvType<kind, arg0, arg1, InnerTypes...>::ensure_defined_dependencies(std::vector<uint32_t>& bin,
									       std::vector<TypeDeclarationState*>& declaration_states) { }
  
  /*
   * Int member functions
   */
  
  template<int n, int signedness>
  void SpurvInt<n, signedness>::define(std::vector<uint32_t>& bin) {
    SpurvInt<n, signedness>::ensureInitID();
    SpurvInt<n, signedness>::declareDefined();
    
    Utils::add(bin, (4 << 16) | 21);
    Utils::add(bin, SpurvInt<n, signedness>::declarationState.id);
    Utils::add(bin, n); // Width
    Utils::add(bin, signedness); // 0 = unsigned, 1 = signed

  }

  template<int n, int signedness>
  void SpurvInt<n, signedness>::ensure_defined(std::vector<uint32_t>& bin, std::vector<TypeDeclarationState*>& declaration_states) {
    if( !SpurvInt<n, signedness>::isDefined()) {
      define(bin);
      declaration_states.push_back(&(SpurvInt<n, signedness>::declarationState));
    }
  }

  
  /*
   * Float member functions
   */
  
  template<int n>
  void SpurvFloat<n>::define(std::vector<uint32_t>& bin) {
    SpurvFloat<n>::ensureInitID();
    SpurvFloat<n>::declareDefined();
    
    Utils::add(bin, (3 << 16) | 22);
    Utils::add(bin, SpurvFloat<n>::declarationState.id);
    Utils::add(bin, n);

  }
  
  template<int n>
  void SpurvFloat<n>::ensure_defined(std::vector<uint32_t>& bin, std::vector<TypeDeclarationState*>& declaration_states) {

    if( !SpurvFloat<n>::isDefined()) {
      define(bin);
      declaration_states.push_back(&(SpurvFloat<n>::declarationState));
    }
  }


  /*
   * Mat member functions
   */
  
  template<int n, int m>
  void SpurvMat<n, m>::ensure_defined_dependencies(std::vector<uint32_t>& bin,
						   std::vector<TypeDeclarationState*>& declaration_states) {
    if constexpr(m == 1) {
	SpurvFloat<32>::ensure_defined(bin, declaration_states);
      } else {
      SpurvMat<n, 1>::ensure_defined(bin, declaration_states);
    }
  }

  template<int n, int m>
  void SpurvMat<n, m>::ensure_defined(std::vector<uint32_t>& bin,
				      std::vector<TypeDeclarationState*>& declaration_states) {
    if( !SpurvMat<n, m>::isDefined()) {
      ensure_defined_dependencies(bin, declaration_states);
      define(bin);
      declaration_states.push_back(&(SpurvMat<n, m>::declarationState));
    }
  }
  
  template<int n, int m>
  void SpurvMat<n, m>::define(std::vector<uint32_t>& bin) {

    SpurvMat<n, m>::ensureInitID();
    SpurvMat<n, m>::declareDefined();
    
    if constexpr(m == 1) {
	Utils::add(bin, (4 << 16) | 23);
	Utils::add(bin, SpurvMat<n, 1>::getID());
	Utils::add(bin, SpurvFloat<32>::getID());
	Utils::add(bin, n);
      } else {
      Utils::add(bin, (4 << 16) | 24);
      Utils::add(bin, SpurvMat<n, m>::getID());
      Utils::add(bin, SpurvMat<n, 1>::getID());
      Utils::add(bin, m);
    }
  }
  

  /*
   * Arr member functions
   */
  
  template<int n, typename tt>
  void SpurvArr<n, tt>::ensure_defined_dependencies(std::vector<uint32_t>& bin,
						    std::vector<TypeDeclarationState*>& declaration_states) {
    tt::ensure_defined(bin, declaration_states);
  }

  template<int n, typename tt>
  void SpurvArr<n, tt>::ensure_defined(std::vector<uint32_t>& bin, std::vector<TypeDeclarationState*>& declaration_states) {
    if( !SpurvArr<n, tt>::isDefined()) {
	ensure_defined_dependencies(bin, declaration_states);
	define(bin);
	declaration_states.push_back(&(SpurvArr<n, tt>::declarationState));
      }
    }
  
  template<int n, typename tt>
  void SpurvArr<n, tt>::define(std::vector<uint32_t>& bin) {
    SpurvArr<n, tt>::ensureInitID();
    SpurvArr<n, tt>::declareDefined();
    
    Utils::add(bin, (4 << 16) | 28);
    Utils::add(bin, SpurvArr<n, tt>::declarationState.id);
    Utils::add(bin, tt::getID());
    Utils::add(bin, n);
  }

  /*
   * Pointer member functions
   */

  template<SpurvStorageClass storage, typename tt>
  void SpurvPointer<storage, tt>::ensure_defined_dependencies(std::vector<uint32_t>& bin,
							      std::vector<TypeDeclarationState*>& declaration_states) {
    tt::ensure_defined(bin, declaration_states);
  }

  template<SpurvStorageClass storage, typename tt>
  void SpurvPointer<storage, tt>::ensure_defined(std::vector<uint32_t>& bin, std::vector<TypeDeclarationState*>& declaration_states) {
    if( !SpurvPointer<storage, tt>::isDefined()) {
      ensure_defined_dependencies(bin, declaration_states);
      define(bin);
      declaration_states.push_back(&(SpurvPointer<storage, tt>::declarationState));
    }
  }

  template<SpurvStorageClass storage, typename tt>
  void SpurvPointer<storage, tt>::define(std::vector<uint32_t>& bin) {
    SpurvPointer<storage, tt>::ensureInitID();
    SpurvPointer<storage, tt>::declareDefined();
    
    Utils::add(bin, (4 << 16) | 32);
    Utils::add(bin, SpurvPointer<storage, tt>::declarationState.id);
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
							       std::vector<TypeDeclarationState*>& declaration_states) {
    Utils::ensureDefinedRecursive<InnerTypes...>(bin, declaration_states);
  }

  template<typename... InnerTypes>
  void SpurvStruct<InnerTypes...>::ensure_defined(std::vector<uint32_t>& bin,
						   std::vector<TypeDeclarationState*>& declaration_states) {
    if( !SpurvStruct<InnerTypes...>::isDefined()) {
      ensure_defined_dependencies(bin, declaration_states);
      define(bin);
      declaration_states.push_back(&(SpurvStruct<InnerTypes...>::declarationState));
    }
  }

  template<typename... InnerTypes>
  void SpurvStruct<InnerTypes...>::define(std::vector<uint32_t>& bin) {
    SpurvStruct<InnerTypes...>::ensureInitID();
    SpurvStruct<InnerTypes...>::declareDefined();
    
    Utils::add(bin, ((2 + sizeof...(InnerTypes)) << 16) | 30);
    Utils::add(bin, SpurvStruct<InnerTypes...>::declarationState.id);
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
    Utils::add(bin, SpurvStruct<InnerTypes...>::declarationState.id);
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
