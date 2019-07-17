#ifndef __SPURV_TYPES_IMPL
#define __SPURV_TYPES_IMPL

#include "types.hpp"

namespace spurv {
  
  
  /* 
   * Static methods
   */
  
  template<STypeKind kind, int arg0, int arg1, typename... InnerTypes>
  void SType<kind, arg0, arg1, InnerTypes...>::getDSType(DSType* type) {
      type->kind = kind;
      type->a0 = arg0;
      type->a1 = arg1;

      constexpr int n = sizeof...(InnerTypes);
      
      if constexpr (n > 0) {
	type->inner_types = new DSType[n];
	SUtils::getDSTypesRecursive<InnerTypes...>(type->inner_types);
      }
    }

  template<STypeKind kind, int arg0, int arg1, typename... InnerTypes>
  int SType<kind, arg0, arg1, InnerTypes...>::getID() {
    if(declarationState.id == -1) {
      printf("Kind = %d, arg0 = %d, arg1 = %d\n", kind, arg0, arg1);
      printf("Tried to use type declarationState.id before defined\n");
      // Fall through to catch errors other places..
    }
    return declarationState.id;
  }

  template<STypeKind kind, int arg0, int arg1, typename... InnerTypes>
  int SType<kind, arg0, arg1, InnerTypes...>::ensureInitID() {
    if(declarationState.id == -1) {
      declarationState.id = SUtils::getNewID();
    }
    return declarationState.id;
  }

  template<STypeKind kind, int arg0, int arg1, typename... InnerTypes>
  bool SType<kind, arg0, arg1, InnerTypes...>::isDefined() {
    return declarationState.is_defined;
  }

  template<STypeKind kind, int arg0, int arg1, typename... InnerTypes>
  void SType<kind, arg0, arg1, InnerTypes...>::declareDefined() {
    declarationState.is_defined = true;
  }

  template<STypeKind kind, int arg0, int arg1, typename... InnerTypes>
  constexpr STypeKind SType<kind, arg0, arg1, InnerTypes...>::getKind() {
    return kind;
  }

  template<STypeKind kind, int arg0, int arg1, typename... InnerTypes>
  constexpr int SType<kind, arg0, arg1, InnerTypes...>::getArg0() {
    return arg0;
  }

  template<STypeKind kind, int arg0, int arg1, typename... InnerTypes>
  constexpr int SType<kind, arg0, arg1, InnerTypes...>::getArg1() {
    return arg1;
  }
  
  
  /*
   * Static variables
   */
  
  template<STypeKind kind, int arg0, int arg1, typename... InnerTypes>
  SDeclarationState SType<kind, arg0, arg1, InnerTypes...>::declarationState;
  
  
  /*
   * Default Member functions
   */

  template<STypeKind kind, int arg0, int arg1, typename... InnerTypes>
  void SType<kind, arg0, arg1, InnerTypes...>::ensure_defined_dependencies(std::vector<uint32_t>& bin,
									       std::vector<SDeclarationState*>& declaration_states) { }
  
  /*
   * Int member functions
   */
  
  template<int n, int signedness>
  void SInt<n, signedness>::define(std::vector<uint32_t>& bin) {
    SInt<n, signedness>::ensureInitID();
    SInt<n, signedness>::declareDefined();
    
    SUtils::add(bin, (4 << 16) | 21);
    SUtils::add(bin, SInt<n, signedness>::declarationState.id);
    SUtils::add(bin, n); // Width
    SUtils::add(bin, signedness); // 0 = unsigned, 1 = signed

  }

  template<int n, int signedness>
  void SInt<n, signedness>::ensure_defined(std::vector<uint32_t>& bin, std::vector<SDeclarationState*>& declaration_states) {
    if( !SInt<n, signedness>::isDefined()) {
      define(bin);
      declaration_states.push_back(&(SInt<n, signedness>::declarationState));
    }
  }

  template<int n, int signedness>
  constexpr int SInt<n, signedness>::getSize() {
    return (n + 7) / 8; // Round up number of bytes
  }

  
  /*
   * Float member functions
   */
  
  template<int n>
  void SFloat<n>::define(std::vector<uint32_t>& bin) {
    SFloat<n>::ensureInitID();
    SFloat<n>::declareDefined();
    
    SUtils::add(bin, (3 << 16) | 22);
    SUtils::add(bin, SFloat<n>::declarationState.id);
    SUtils::add(bin, n);

  }
  
  template<int n>
  void SFloat<n>::ensure_defined(std::vector<uint32_t>& bin, std::vector<SDeclarationState*>& declaration_states) {

    if( !SFloat<n>::isDefined()) {
      define(bin);
      declaration_states.push_back(&(SFloat<n>::declarationState));
    }
  }

  template<int n>
  constexpr int SFloat<n>::getSize() {
    return (n + 7) / 8; // Round up number of bytes
  }


  /*
   * Mat member functions
   */
  
  template<int n, int m>
  void SMat<n, m>::ensure_defined_dependencies(std::vector<uint32_t>& bin,
						   std::vector<SDeclarationState*>& declaration_states) {
    if constexpr(m == 1) {
	SFloat<32>::ensure_defined(bin, declaration_states);
      } else {
      SMat<n, 1>::ensure_defined(bin, declaration_states);
    }
  }

  template<int n, int m>
  void SMat<n, m>::ensure_defined(std::vector<uint32_t>& bin,
				      std::vector<SDeclarationState*>& declaration_states) {
    if( !SMat<n, m>::isDefined()) {
      ensure_defined_dependencies(bin, declaration_states);
      define(bin);
      declaration_states.push_back(&(SMat<n, m>::declarationState));
    }
  }
  
  template<int n, int m>
  void SMat<n, m>::define(std::vector<uint32_t>& bin) {

    SMat<n, m>::ensureInitID();
    SMat<n, m>::declareDefined();
    
    if constexpr(m == 1) {
	SUtils::add(bin, (4 << 16) | 23);
	SUtils::add(bin, SMat<n, 1>::getID());
	SUtils::add(bin, SFloat<32>::getID());
	SUtils::add(bin, n);
      } else {
      SUtils::add(bin, (4 << 16) | 24);
      SUtils::add(bin, SMat<n, m>::getID());
      SUtils::add(bin, SMat<n, 1>::getID());
      SUtils::add(bin, m);
    }
  }

  template<int n, int m>
  constexpr int SMat<n, m>::getSize() {
    // Assume perfectly aligned and with 32-bit components
    return n * m * 4; 
  }
  

  /*
   * Arr member functions
   */
  
  template<int n, typename tt>
  void SArr<n, tt>::ensure_defined_dependencies(std::vector<uint32_t>& bin,
						    std::vector<SDeclarationState*>& declaration_states) {
    tt::ensure_defined(bin, declaration_states);
  }

  template<int n, typename tt>
  void SArr<n, tt>::ensure_defined(std::vector<uint32_t>& bin, std::vector<SDeclarationState*>& declaration_states) {
    if( !SArr<n, tt>::isDefined()) {
	ensure_defined_dependencies(bin, declaration_states);
	define(bin);
	declaration_states.push_back(&(SArr<n, tt>::declarationState));
      }
    }
  
  template<int n, typename tt>
  void SArr<n, tt>::define(std::vector<uint32_t>& bin) {
    SArr<n, tt>::ensureInitID();
    SArr<n, tt>::declareDefined();
    
    SUtils::add(bin, (4 << 16) | 28);
    SUtils::add(bin, SArr<n, tt>::declarationState.id);
    SUtils::add(bin, tt::getID());
    SUtils::add(bin, n);
  }

  template<int n, typename tt>
  constexpr int SArr<n, tt>::getSize() {
    return n * tt::getSize(); // Assume perfectly aligned
  }

  
  /*
   * Pointer member functions
   */

  template<SStorageClass storage, typename tt>
  void SPointer<storage, tt>::ensure_defined_dependencies(std::vector<uint32_t>& bin,
							      std::vector<SDeclarationState*>& declaration_states) {
    tt::ensure_defined(bin, declaration_states);
  }

  template<SStorageClass storage, typename tt>
  void SPointer<storage, tt>::ensure_defined(std::vector<uint32_t>& bin, std::vector<SDeclarationState*>& declaration_states) {
    if( !SPointer<storage, tt>::isDefined()) {
      ensure_defined_dependencies(bin, declaration_states);
      define(bin);
      declaration_states.push_back(&(SPointer<storage, tt>::declarationState));
    }
  }

  template<SStorageClass storage, typename tt>
  void SPointer<storage, tt>::define(std::vector<uint32_t>& bin) {
    SPointer<storage, tt>::ensureInitID();
    SPointer<storage, tt>::declareDefined();
    
    SUtils::add(bin, (4 << 16) | 32);
    SUtils::add(bin, SPointer<storage, tt>::declarationState.id);
    SUtils::add(bin, (int)storage);
    SUtils::add(bin, tt::getID());

  }

  // No getSize function defined for pointers
  
  /*
   * Struct member variables
   */ 

  template<typename... InnerTypes>
  bool SStruct<InnerTypes...>::is_decorated = false;
  
  
  /*
   * Struct member functions
   */
  
  template<typename... InnerTypes>
  void SStruct<InnerTypes...>::ensure_defined_dependencies(std::vector<uint32_t>& bin,
							       std::vector<SDeclarationState*>& declaration_states) {
    SUtils::ensureDefinedRecursive<InnerTypes...>(bin, declaration_states);
  }

  template<typename... InnerTypes>
  void SStruct<InnerTypes...>::ensure_defined(std::vector<uint32_t>& bin,
						   std::vector<SDeclarationState*>& declaration_states) {
    if( !SStruct<InnerTypes...>::isDefined()) {
      ensure_defined_dependencies(bin, declaration_states);
      define(bin);
      declaration_states.push_back(&(SStruct<InnerTypes...>::declarationState));
    }
  }

  template<typename... InnerTypes>
  void SStruct<InnerTypes...>::define(std::vector<uint32_t>& bin) {
    SStruct<InnerTypes...>::ensureInitID();
    SStruct<InnerTypes...>::declareDefined();
    
    SUtils::add(bin, ((2 + sizeof...(InnerTypes)) << 16) | 30);
    SUtils::add(bin, SStruct<InnerTypes...>::declarationState.id);
    SUtils::addIDsRecursive<InnerTypes...>(bin);

  }

  template<typename... InnerTypes>
  void SStruct<InnerTypes...>::getDSType(DSType* type) {
    constexpr int n = sizeof...(InnerTypes);
    type->kind = KIND_STRUCT;
    type->inner_types = new DSType[sizeof...(InnerTypes)];
    SUtils::getDSTypesRecursive<InnerTypes...>(type);
  }

  template<typename... InnerTypes>
  constexpr int SStruct<InnerTypes...>::getSize() {
    return SUtils::getSumSize<InnerTypes...>();
  }
  
  template<typename... InnerTypes>
  template<int member_no, int start_size, typename First, typename... Types>
  void SStruct<InnerTypes...>::decorate_member_offsets(std::vector<uint32_t>& bin) {

    // MemberDecorate <struct_id> Offset <member_no> <offset>
    SUtils::add(bin, (5 << 16) | 72);
    SUtils::add(bin, SStruct<InnerTypes...>::getID());
    SUtils::add(bin, member_no);
    SUtils::add(bin, 35);
    SUtils::add(bin, start_size);

    if constexpr( sizeof...(Types) > 0) {
        decorate_member_offsets<member_no + 1,
					     start_size + First::getSize(),
					     Types...>(bin);
      }	       
  }

  
  template<typename... InnerTypes>
  void SStruct<InnerTypes...>::ensure_decorated(std::vector<uint32_t>& bin) {
    if( is_decorated) {
      return;
    }

    SStruct<InnerTypes...>::ensureInitID();

    is_decorated = true;

    // OpDecorate <type id> Block
    SUtils::add(bin, (3 << 16) | 71);
    SUtils::add(bin, SStruct<InnerTypes...>::declarationState.id);
    SUtils::add(bin, 2);

    SStruct<InnerTypes...>::
      decorate_member_offsets<0, 0, InnerTypes...>(bin);
  }
  
};
#endif // __SPURV_TYPES_IMPL
