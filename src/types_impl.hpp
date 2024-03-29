#ifndef __SPURV_TYPES_IMPL
#define __SPURV_TYPES_IMPL

#include "types.hpp"
#include "constant_registry.hpp"

namespace spurv {


  /*
   * Static methods
   */

  template<STypeKind kind, int arg0, int arg1, int arg2, int arg3, int arg4, typename... InnerTypes>
  void SType<kind, arg0, arg1, arg2, arg3, arg4, InnerTypes...>::getDSType(DSType* type) {
      type->kind = kind;
      type->a0 = arg0;
      type->a1 = arg1;

      constexpr int n = sizeof...(InnerTypes);

      if constexpr (n > 0) {
	  type->inner_types = std::vector<DSType>(n);
	  SUtils::getDSTypesRecursive<InnerTypes...>(type->inner_types.data());
	}
    }

  template<STypeKind kind, int arg0, int arg1, int arg2, int arg3, int arg4, typename... InnerTypes>
  int SType<kind, arg0, arg1, arg2, arg3, arg4, InnerTypes...>::getID() {
    if(declarationState.id == -1) {
      printf("Tried to use type declarationState.id before defined\n");
      // Fall through to catch errors other places..

    }
    return declarationState.id;
  }

  template<STypeKind kind, int arg0, int arg1, int arg2, int arg3, int arg4, typename... InnerTypes>
  int SType<kind, arg0, arg1,arg2, arg3, arg4,  InnerTypes...>::ensureInitID() {
    if(declarationState.id == -1) {
      declarationState.id = SUtils::getNewID();
    }
    return declarationState.id;
  }

  template<STypeKind kind, int arg0, int arg1, int arg2, int arg3, int arg4, typename... InnerType>
  void SType<kind, arg0, arg1, arg2, arg3, arg4, InnerType...>::ensure_decorated(std::vector<uint32_t>& bin,
										  std::vector<bool*>& decoration_states) { }

  template<STypeKind kind, int arg0, int arg1, int arg2, int arg3, int arg4, typename... InnerTypes>
  bool SType<kind, arg0, arg1, arg2, arg3, arg4, InnerTypes...>::isDefined() {
    return declarationState.is_defined;
  }

  template<STypeKind kind, int arg0, int arg1, int arg2, int arg3, int arg4, typename... InnerTypes>
  void SType<kind, arg0, arg1, arg2, arg3, arg4, InnerTypes...>::declareDefined() {
    declarationState.is_defined = true;
  }

  template<STypeKind kind, int arg0, int arg1, int arg2, int arg3, int arg4, typename... InnerTypes>
  constexpr STypeKind SType<kind, arg0, arg1, arg2, arg3, arg4, InnerTypes...>::getKind() {
    return kind;
  }

  template<STypeKind kind, int arg0, int arg1, int arg2, int arg3, int arg4, typename... InnerTypes>
  constexpr int SType<kind, arg0, arg1, arg2, arg3, arg4, InnerTypes...>::getArg0() {
    return arg0;
  }

  template<STypeKind kind, int arg0, int arg1, int arg2, int arg3, int arg4, typename... InnerTypes>
  constexpr int SType<kind, arg0, arg1, arg2, arg3, arg4, InnerTypes...>::getArg1() {
    return arg1;
  }


  /*
   * Static variables
   */

  template<STypeKind kind, int arg0, int arg1, int arg2, int arg3, int arg4, typename... InnerTypes>
  SDeclarationState SType<kind, arg0, arg1, arg2, arg3, arg4, InnerTypes...>::declarationState;


  /*
   * Default Member functions
   */

  template<STypeKind kind, int arg0, int arg1, int arg2, int arg3, int arg4, typename... InnerTypes>
  void SType<kind, arg0, arg1, arg2, arg3, arg4, InnerTypes...>::ensure_defined_dependencies(std::vector<uint32_t>& bin,
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

  template<int n, int signedness>
  template<typename tt>
  SValue<SInt<n, signedness> >& SInt<n, signedness>::cons(tt&& arg) {
    static_assert(std::is_convertible<tt, int32_t>::value, "Integer construction value must be convertible to integer");
    SValue<SInt<n, signedness> >* value = SUtils::allocate<Constant< typename InvMapSType<SInt<n, signedness> >::type> >(arg);
    return *value;
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
  void SFloat<n>::ensure_defined(std::vector<uint32_t>& bin,
				 std::vector<SDeclarationState*>& declaration_states) {

    if( !SFloat<n>::isDefined()) {
      define(bin);
      declaration_states.push_back(&(SFloat<n>::declarationState));
    }
  }

  template<int n>
  constexpr int SFloat<n>::getSize() {
    return (n + 7) / 8; // Round up number of bytes
  }

  template<int n>
  template<typename tt>
  SValue<SFloat<n> >& SFloat<n>::cons(tt&& arg) {
      static_assert(std::is_convertible<tt, float>::value, "Value must be convertible to float");
      float f = static_cast<float>(arg);
    SValue<SFloat<n> >* value = SUtils::allocate<Constant<float> >(f);
    return *value;
  }


  /*
   * Mat member functions
   */

  template<int n, int m, typename inner>
  void SMat<n, m, inner>::ensure_defined_dependencies(std::vector<uint32_t>& bin,
							       std::vector<SDeclarationState*>& declaration_states) {
    if constexpr(m == 1) {
	inner::ensure_defined(bin, declaration_states);
      } else {
      SMat<n, 1, inner>::ensure_defined(bin, declaration_states);
    }
  }

  template<int n, int m, typename inner>
  void SMat<n, m, inner>::ensure_defined(std::vector<uint32_t>& bin,
					 std::vector<SDeclarationState*>& declaration_states) {
    if( !SMat<n, m, inner>::isDefined()) {
      ensure_defined_dependencies(bin, declaration_states);
      define(bin);
      declaration_states.push_back(&(SMat<n, m, inner>::declarationState));
    }
  }

  template<int n, int m, typename inner>
  void SMat<n, m, inner>::define(std::vector<uint32_t>& bin) {

    SMat<n, m, inner>::ensureInitID();
    SMat<n, m, inner>::declareDefined();

    if constexpr(m == 1) {
	SUtils::add(bin, (4 << 16) | 23);
	SUtils::add(bin, SMat<n, 1, inner>::getID());
	SUtils::add(bin, inner::getID());
	SUtils::add(bin, n);
      } else {
      SUtils::add(bin, (4 << 16) | 24);
      SUtils::add(bin, SMat<n, m, inner>::getID());
      SUtils::add(bin, SMat<n, 1, inner>::getID());
      SUtils::add(bin, m);
    }
  }

  template<int n, int m, typename inner>
  constexpr int SMat<n, m, inner>::getSize() {
    // Assume perfectly aligned and with 32-bit components
    return n * m * 4;
  }


  template<int n, int m, typename inner>
  template<typename... Types>
  ConstructMatrix<n, m, inner>& SMat<n, m, inner>::cons(Types&&... args) {
    return *(SUtils::allocate<ConstructMatrix<n, m, inner> >(args...));
  }


  /*
   * Arr member functions
   */

  template<int n, SStorageClass storage, typename tt>
  void SArr<n, storage, tt>::ensure_defined_dependencies(std::vector<uint32_t>& bin,
						    std::vector<SDeclarationState*>& declaration_states) {
    tt::ensure_defined(bin, declaration_states);
    SPointer<storage, tt>::ensure_defined(bin, declaration_states);
  }

  template<int n, SStorageClass storage, typename tt>
  void SArr<n, storage, tt>::ensure_defined(std::vector<uint32_t>& bin, std::vector<SDeclarationState*>& declaration_states) {
    if( !SArr<n, storage, tt>::isDefined()) {
	ensure_defined_dependencies(bin, declaration_states);
	define(bin);
	declaration_states.push_back(&(SArr<n, storage, tt>::declarationState));
      }
    }

  template<int n, SStorageClass storage, typename tt>
  void SArr<n, storage, tt>::define(std::vector<uint32_t>& bin) {
    SArr<n, storage, tt>::ensureInitID();
    SArr<n, storage, tt>::declareDefined();

    SUtils::add(bin, (4 << 16) | 28);
    SUtils::add(bin, SArr<n, storage, tt>::declarationState.id);
    SUtils::add(bin, tt::getID());
    SUtils::add(bin, n);
  }

  template<int n, SStorageClass storage, typename tt>
  void SArr<n, storage, tt>::ensure_decorated(std::vector<uint32_t>& bin,
				     std::vector<bool*>& decoration_states) {
    tt::ensure_decorated(bin,
			 decoration_states);

  }

  template<int n, SStorageClass storage, typename tt>
  constexpr int SArr<n, storage, tt>::getSize() {
    return n * tt::getSize(); // Assume perfectly aligned
  }


  /*
   * RunArr member functions
   */

  template<SStorageClass storage, typename tt>
  void SRunArr<storage, tt>::ensure_defined_dependencies(std::vector<uint32_t>& bin,
					       std::vector<SDeclarationState*>& declaration_states) {
    tt::ensure_defined(bin, declaration_states);
    SPointer<storage, tt>::ensure_defined(bin, declaration_states);
  }

  template<SStorageClass storage, typename tt>
  void SRunArr<storage, tt>::ensure_defined(std::vector<uint32_t>& bin, std::vector<SDeclarationState*>& declaration_states) {
    if( !SRunArr<storage, tt>::isDefined()) {
      ensure_defined_dependencies(bin, declaration_states);
      define(bin);
      declaration_states.push_back(&(SRunArr<storage, tt>::declarationState));
    }
  }

  template<SStorageClass storage, typename tt>
  void SRunArr<storage, tt>::define(std::vector<uint32_t>& bin) {
    SRunArr<storage, tt>::ensureInitID();
    SRunArr<storage, tt>::declareDefined();

    SUtils::add(bin, (3 << 16) | 29);
    SUtils::add(bin, SRunArr<storage, tt>::declarationState.id);
    SUtils::add(bin, tt::getID());
  }

  template<SStorageClass storage, typename tt>
  void SRunArr<storage, tt>::ensure_decorated(std::vector<uint32_t>& bin,
				     std::vector<bool*>& decoration_states) {
    if( is_decorated) {
      return;
    }

    tt::ensure_decorated(bin,
			 decoration_states);

    SRunArr<storage, tt>::ensureInitID();

    is_decorated = true;
    decoration_states.push_back(&is_decorated);

    // OpDecorate <type_id> ArrayStride <type_size>
    SUtils::add(bin, (4 << 16) | 71);
    SUtils::add(bin, SRunArr<storage, tt>::declarationState.id);
    SUtils::add(bin, 6); // ArrayStride
    SUtils::add(bin, tt::getSize());

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

  template<SStorageClass storage, typename tt>
  void SPointer<storage, tt>::ensure_decorated(std::vector<uint32_t>& bin,
					       std::vector<bool*>& decoration_states) {
    // Most input/output variable decorations are handled in shader, only decorate inner types here
    tt::ensure_decorated(bin, decoration_states);

  }

  // No getSize function defined for pointers


  /*
   * Struct member variables
   */

  template<SDecoration decor, typename... InnerTypes>
  bool SStruct<decor, InnerTypes...>::is_decorated = false;

  template<SStorageClass storage, typename inner>
  bool SRunArr<storage, inner>::is_decorated = false;


  /*
   * Struct member functions
   */

  template<SDecoration decor, typename... InnerTypes>
  void SStruct<decor, InnerTypes...>::ensure_defined_dependencies(std::vector<uint32_t>& bin,
							       std::vector<SDeclarationState*>& declaration_states) {
    SUtils::ensureDefinedRecursive<InnerTypes...>(bin, declaration_states);
  }

  template<SDecoration decor, typename... InnerTypes>
  void SStruct<decor, InnerTypes...>::ensure_defined(std::vector<uint32_t>& bin,
						   std::vector<SDeclarationState*>& declaration_states) {
    if( !SStruct<decor, InnerTypes...>::isDefined()) {
      ensure_defined_dependencies(bin, declaration_states);
      define(bin);
      declaration_states.push_back(&(SStruct<decor, InnerTypes...>::declarationState));
    }
  }

  template<SDecoration decor, typename... InnerTypes>
  void SStruct<decor, InnerTypes...>::define(std::vector<uint32_t>& bin) {
    SStruct<decor, InnerTypes...>::ensureInitID();
    SStruct<decor, InnerTypes...>::declareDefined();

    SUtils::add(bin, ((2 + sizeof...(InnerTypes)) << 16) | 30);
    SUtils::add(bin, SStruct<decor, InnerTypes...>::declarationState.id);
    SUtils::addIDsRecursive<InnerTypes...>(bin);

  }

  template<SDecoration decor, typename... InnerTypes>
  constexpr int SStruct<decor, InnerTypes...>::getSize() {
    return SUtils::getSumSize<InnerTypes...>();
  }

  template<SDecoration decor, typename... InnerTypes>
  template<int member_no, int start_size, typename First, typename... Types>
  void SStruct<decor, InnerTypes...>::decorate_members(std::vector<uint32_t>& bin,
						       std::vector<bool*>& decoration_states) {

    if constexpr( is_spurv_mat_type<First>::value && First::getArg1() != 1) {
	// MemberDecorate <struct_id> <member_no> ColMajor
	SUtils::add(bin, (4 << 16) | 72);
	SUtils::add(bin, SStruct<decor, InnerTypes...>::getID());
	SUtils::add(bin, member_no);
	SUtils::add(bin, 5);

	// MemberDecorate <struct_id> <member_no> MatrixStride <stride>
	SUtils::add(bin, (5 << 16) | 72);
	SUtils::add(bin, SStruct<decor, InnerTypes...>::getID());
	SUtils::add(bin, member_no);
	SUtils::add(bin, 7);
	SUtils::add(bin, First::getSize() / First::getArg1());

      }

    // MemberDecorate <struct_id> <member_no> Offset <offset>
    SUtils::add(bin, (5 << 16) | 72);
    SUtils::add(bin, SStruct<decor, InnerTypes...>::getID());
    SUtils::add(bin, member_no);
    SUtils::add(bin, 35);
    SUtils::add(bin, start_size);

    // When using structs as uniform inputs (as is the only use per now),
    // one must declare each member as non-writable (readonly), or the
    // device is required to use the vertexPipelineStoresAndAtomics feature

    // MemberDecorate <struct_id> <member_no> NonWritable
    SUtils::add(bin, (4 << 16) | 72);
    SUtils::add(bin, SStruct<decor, InnerTypes...>::getID());
    SUtils::add(bin, member_no);
    SUtils::add(bin, 24);

    First::ensure_decorated(bin, decoration_states);


    if constexpr( sizeof...(Types) > 0) {
	SStruct<decor, InnerTypes...>::decorate_members<member_no + 1,
							start_size + First::getSize(),
							Types...>(bin, decoration_states);
      }
  }

  template<SDecoration decor, typename... InnerTypes>
  void SStruct<decor, InnerTypes...>::ensure_decorated(std::vector<uint32_t>& bin,
						std::vector<bool*>& decoration_states) {
    if( is_decorated) {
      return;
    }

    SStruct<decor, InnerTypes...>::ensureInitID();

    is_decorated = true;
    decoration_states.push_back(&is_decorated);

    if constexpr(decor == SDecoration::BLOCK) {
	SStruct<decor, InnerTypes...>::decorate_block(bin, decoration_states);
      }

    SStruct<decor, InnerTypes...>::
      decorate_members<0, 0, InnerTypes...>(bin, decoration_states);
  }

  template<SDecoration decor, typename... InnerTypes>
  void SStruct<decor, InnerTypes...>::decorate_block(std::vector<uint32_t>& bin,
					      std::vector<bool*>& decoration_states) {
    if(!((int)decor & (int)SDecoration::BLOCK)) {
      printf("[spurv::SStruct::decorate_block] Tried to decorate struct without Block decoration as Block\n");
      exit(-1);
    }

    // OpDecorate <type id> Block
    SUtils::add(bin, (3 << 16) | 71);
    SUtils::add(bin, SStruct<decor, InnerTypes...>::declarationState.id);
    SUtils::add(bin, 2);
  }


  /*
   * SImage functions
   */

  template<int dim, int depth, int arrayed, int multisamp, int sampled>
  void SImage<dim, depth, arrayed, multisamp, sampled>::ensure_defined_dependencies(std::vector<uint32_t>& bin,
							std::vector<SDeclarationState*>& declaration_states) {
    SFloat<32>::ensure_defined(bin, declaration_states);
  }

  template<int dim, int depth, int arrayed, int multisamp, int sampled>
  void SImage<dim, depth, arrayed, multisamp, sampled>::ensure_defined(std::vector<uint32_t>& bin,
								       std::vector<SDeclarationState*>& declaration_states) {
    if(!SImage<dim, depth, arrayed, multisamp, sampled>::isDefined()) {
      ensure_defined_dependencies(bin, declaration_states);
      define(bin);
      declaration_states.push_back(&(SImage<dim, depth, arrayed, multisamp, sampled>::declarationState));
    }
  }

  template<int dims, int depth, int arrayed, int multisamp, int sampled>
  void SImage<dims, depth, arrayed, multisamp, sampled>::define(std::vector<uint32_t>& bin) {
    using ThisType = SImage<dims, depth, arrayed, multisamp, sampled>;
    ThisType::ensureInitID();
    ThisType::declareDefined();

    // OpTypeImage
    SUtils::add(bin, (9 << 16) | 25);
    SUtils::add(bin, ThisType::getID());
    SUtils::add(bin, SFloat<32>::getID());
    SUtils::add(bin, dims);
    SUtils::add(bin, depth);
    SUtils::add(bin, arrayed);
    SUtils::add(bin, multisamp);
    SUtils::add(bin, sampled);
    SUtils::add(bin, 1); // Rgba32f
  }


  /*
   * STexture functions
   */

  template<int n>
  void STexture<n>::ensure_defined_dependencies(std::vector<uint32_t>& bin,
						std::vector<SDeclarationState*>& declaration_states) {
    static_assert(n > 0 && n <= 3, "Texture dimension must be positive and at most 3");

    // 2D is 1, etc..
    SImage<n - 1, 0, 0, 0, 1>::ensure_defined(bin, declaration_states);

    // Explicitly define 0 to be default Lod when sampling
    SFloat<32>::ensure_defined(bin, declaration_states);

    SConstantRegistry::ensureDefinedConstant<float>(0.0f, SUtils::getNewID(), bin);

    // Result will always be 4-component vector (according to specs, for some reason)
    vec4_s::ensure_defined(bin, declaration_states);
  }

  template<int n>
  void STexture<n>::ensure_defined(std::vector<uint32_t>& bin, std::vector<SDeclarationState*>& declaration_states) {
    if(!STexture<n>::isDefined()) {
      ensure_defined_dependencies(bin, declaration_states);
      define(bin);
      declaration_states.push_back(&(STexture<n>::declarationState));
    }
  }

  template<int n>
  void STexture<n>::define(std::vector<uint32_t>& bin) {

    STexture<n>::ensureInitID();
    STexture<n>::declareDefined();

    // OpTypeSampledImage
    SUtils::add(bin, (3 << 16) | 27);
    SUtils::add(bin, STexture<n>::getID());
    SUtils::add(bin, SImage<n - 1, 0, 0, 0, 1>::getID());
  }
};
#endif // __SPURV_TYPES_IMPL
