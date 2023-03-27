#ifndef __SPURV_TYPES
#define __SPURV_TYPES

#include "declarations.hpp"
#include "utils.hpp"

#include <FlatAlg.hpp>

namespace spurv {

  /*
   * Utility struct for SType
   */

  class SDeclarationState {
  public:
    SDeclarationState();
    int id;
    bool is_defined;
  };


  /*
   * NullType
   */

  class NullType {
  public:
    static void ensure_defined(std::vector<uint32_t>&,
			       std::vector<int32_t*>&);
    static void define(std::vector<uint32_t>&);
  };


  /*
   * DSType - Dynamic (runtime) representation of a SType
   */

  struct DSType {
    // NB: If adding more parameters, remember to add more arguments to SType and
    // is_spurv_type below as well
    STypeKind kind;
    int a0, a1, a2, a3, a4;

    std::vector<DSType> inner_types;

    DSType() : kind(STypeKind::KIND_INVALID),
	       a0(0), a1(0), a2(0), a3(0), a4(0), inner_types(0) { };

    bool operator==(const DSType& ds) const;
    bool operator!=(const DSType& ds) const;
  };


  /*
   * SType - The mother of them all
   */

  template<STypeKind kind, int arg0, int arg1, int arg2,
	   int arg3, int arg4, typename... InnerTypes >
  class SType {

  protected:

    static SDeclarationState declarationState;
    static void declareDefined();
  public:

    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<SDeclarationState*>& declaration_states);
    static void ensure_defined_dependencies(std::vector<uint32_t>& bin, std::vector<SDeclarationState*>& declaration_states);
    static void define(std::vector<uint32_t>& bin);

    static void ensure_decorated(std::vector<uint32_t>& bin,
				 std::vector<bool*>& decoration_states);

    static void getDSType(DSType* type);

    static int getID();
    static bool isDefined();
    static int ensureInitID();

    static constexpr STypeKind getKind();
    static constexpr int getArg0();
    static constexpr int getArg1();

    static constexpr int getSize();


    using firstInnerType = std::tuple_element_t<0, std::tuple<InnerTypes..., void> >;

  };



  /*
   * SBool - Representation of boolean values
   */

  class SBool : public SType<STypeKind::KIND_BOOL> {
  public:
    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<SDeclarationState*>& declaration_states);
    static void define(std::vector<uint32_t>& bin);
    static constexpr int getSize();
  };


  /*
   * SInt - Representation of integers
   */

  template<int n, int signedness>
  class SInt : public SType<STypeKind::KIND_INT, n, signedness> {
  public:
    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<SDeclarationState*>& declaration_states);
    static void define(std::vector<uint32_t>& bin);
    static constexpr int getSize();

    template<typename tt>
    static SValue<SInt<n, signedness> >& cons(tt&& arg);
  };


  /*
   * SFloat - Representation of floats
   */

  template<int n>
  class SFloat : public SType<STypeKind::KIND_FLOAT, n> {
  public:
    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<SDeclarationState*>& declaration_states);
    static void define(std::vector<uint32_t>& bin);
    static constexpr int getSize();

    template<typename tt>
    static SValue<SFloat<n> >& cons(tt&& arg);
  };


  /*
   * SMat - Representation of vectors and matrices
   */

  template<int n, int m, typename inner>
  class SMat : public SType<STypeKind::KIND_MAT, n, m, 0, 0, 0, inner> {
  public:
    static void ensure_defined_dependencies(std::vector<uint32_t>& bin,
					    std::vector<SDeclarationState*>& declaration_states);
    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<SDeclarationState*>& declaration_states);
    static void define(std::vector<uint32_t>& bin);
    static constexpr int getSize();

    template<typename... Types>
    static ConstructMatrix<n, m, inner>& cons(Types&&... args);

    using inner_type = inner;
    static constexpr int nn = n;
    static constexpr int mm = m;
  };


  /*
   * Static explicit type checks - more general (more checks below)
   */

  template<typename>
  struct is_spurv_type : std::false_type {};

  template<>
  struct is_spurv_type<SType<STypeKind::KIND_VOID> > : std::true_type {};

  template<>
  struct is_spurv_type<SBool> : std::true_type {};

  template<int n, int s>
  struct is_spurv_type<SInt<n, s> > : std::true_type {};

  template<int n>
  struct is_spurv_type<SFloat<n> > : std::true_type {};

  template<int n, int m, typename inner>
  struct is_spurv_type<SMat<n, m, inner> > : std::true_type {};

  template<>
  struct is_spurv_type<NullType> : std::true_type {};


  /*
   * SArr - Representation of arrays
   */

  template<int n, SStorageClass storage, typename tt>
  class SArr : public SType<STypeKind::KIND_ARR, storage, n, 0, 0, 0, tt >  {
    constexpr SArr() {
      static_assert(is_spurv_type<tt>::value, "Inner type of SArr must be a spurv type");
    }

  public:
    static void ensure_defined_dependencies(std::vector<uint32_t>& bin,
					    std::vector<SDeclarationState*>& declaration_states);
    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<SDeclarationState*>& declaration_states);
    static void ensure_decorated(std::vector<uint32_t>& bin,
				 std::vector<bool*>& decoration_states);
    static void define(std::vector<uint32_t>& bin);
    static constexpr int getSize();

    using inner_type = tt;
  };


  /*
   * SRunArr - Array with runtime-specified length
   */

  template<SStorageClass storage, typename tt>
  class SRunArr : public SType<STypeKind::KIND_RUN_ARR, storage, 0, 0, 0, 0, tt> {
    constexpr SRunArr() {
      static_assert(is_spurv_type<tt>::value, "Inner type of SRunArr must be a spurv type");
    }

    static bool is_decorated;

  public:
    static void ensure_defined_dependencies(std::vector<uint32_t>& bin,
					    std::vector<SDeclarationState*>& declaration_states);
    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<SDeclarationState*>& declaration_states);
    static void define(std::vector<uint32_t>& bin);
    static void ensure_decorated(std::vector<uint32_t>& bin,
				 std::vector<bool*>& decoration_states);
  };


  /*
   * SPointer - Representation of pointers
   */

  template<SStorageClass storage, typename tt>
  class SPointer : public SType<STypeKind::KIND_POINTER, (int)storage, 0, 0, 0, 0, tt> {
  public:
    static void ensure_defined_dependencies(std::vector<uint32_t>& bin,
					    std::vector<SDeclarationState*>& declaration_states);
    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<SDeclarationState*>& declaration_states);
    static void define(std::vector<uint32_t>& bin);
    static void ensure_decorated(std::vector<uint32_t>& bin,
				 std::vector<bool*>& decoration_states);
  };


  /*
   * SStruct - Representation of structs
   */

  template<SDecoration decoration, typename... InnerTypes>
  class SStruct : public SType<STypeKind::KIND_STRUCT, (int)decoration, 0, 0, 0, 0, InnerTypes...> {
    static bool is_decorated;

  public:

    static void ensure_defined_dependencies(std::vector<uint32_t>& bin,
					    std::vector<SDeclarationState*>& declaration_states);
    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<SDeclarationState*>& declaration_states);
    static void define(std::vector<uint32_t>& bin);
    static void ensure_decorated(std::vector<uint32_t>& bin,
				 std::vector<bool*>& decoration_states);

    template<int member_no, int start_size, typename First, typename... Types>
    static void decorate_members(std::vector<uint32_t>& bin,
				 std::vector<bool*>& decoration_states);

    static void decorate_block(std::vector<uint32_t>& bin,
			       std::vector<bool*>& decoration_states);

    static constexpr int getSize();

    template<SStorageClass stind, typename... inner>
    friend class SStructBinding;

  };


  /*
   * SImage - Representation of abstract image
   */

  template<int dims, int depth, int arrayed, int multisamp, int sampled>
  class SImage : public SType<STypeKind::KIND_IMAGE, dims, depth, arrayed, multisamp, sampled > {
  public:
    static void ensure_defined_dependencies(std::vector<uint32_t>& bin,
					    std::vector<SDeclarationState*>& declaration_states);
    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<SDeclarationState*>& declaration_states);
    static void define(std::vector<uint32_t>& bin);
  };


  /*
   * STexture - Representation of textures of n dimensions
   */

  template<int n>
  class STexture : public SType<STypeKind::KIND_TEXTURE, n> {

  public:
    static void ensure_defined_dependencies(std::vector<uint32_t>& bin,
					    std::vector<SDeclarationState*>& declaration_states);
    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<SDeclarationState*>& declaration_states);
    static void define(std::vector<uint32_t>& bin);
  };


  /*
   * More specialized type checks
   */

  template<typename>
  struct is_spurv_mat_type : std::false_type {};

  template<int n, SStorageClass storage, typename tt>
  struct is_spurv_type<SArr<n, storage, tt> > : std::true_type { static_assert(is_spurv_type<tt>::value); };

  template<SStorageClass storage, typename tt>
  struct is_spurv_type<SPointer<storage, tt> > : std::true_type { static_assert(is_spurv_type<tt>::value); };

  template<int d>
  struct is_spurv_type<STexture<d> > : std::true_type {} ;

  template<SStorageClass storage, typename tt>
  struct is_spurv_type<SRunArr<storage, tt> > : std::true_type {};

  template<int dims, int depth, int arrayed, int multisamp, int sampled>
  struct is_spurv_type<SImage<dims, depth, arrayed, multisamp, sampled> > : std::true_type { };

  template<int n, int m, typename inner>
  struct is_spurv_mat_type<SMat<n, m, inner> > : std::true_type {};

  template<typename>
  struct is_spurv_float_type : std::false_type {};

  template<int n>
  struct is_spurv_float_type<SFloat<n> > : std::true_type {};

  template<typename>
  struct is_spurv_int_type : std::false_type {};

  template<int n, int s>
  struct is_spurv_int_type<SInt<n, s> > : std::true_type {};

  template<typename>
  struct is_spurv_signed_int_type : std::false_type {};

  template<int n>
  struct is_spurv_signed_int_type<SInt<n, 1> > : std::true_type {};

  template<typename>
  struct is_spurv_texture_type : std::false_type {};

  template<int n>
  struct is_spurv_texture_type<STexture<n> > : std::true_type {};

  template<typename>
  struct is_spurv_runtime_array : std::false_type {};

  template<SStorageClass storage, typename tt>
  struct is_spurv_runtime_array<SRunArr<storage, tt> > : std::true_type {};



  template<typename FirstType, typename... InnerTypes>
  constexpr bool isSTypeRecursive() {
    if constexpr(is_spurv_type<FirstType>::value == false) {
	return false;
      }

    if constexpr(sizeof...(InnerTypes) > 0) {
	return isSTypeRecursive<InnerTypes...>();
      }

    return true;
  }

  template<SDecoration decor, typename... InnerTypes>
  struct is_spurv_type<SStruct<decor, InnerTypes...> > : std::true_type{ static_assert(isSTypeRecursive<InnerTypes...>()); };


  /*
   * Function to determine whether a type represents a uniform constant (as opposed
   * to a descriptor set)
   */

  template<typename... InnerTypes>
  static constexpr bool isUniformConstantType = sizeof...(InnerTypes) == 1 &&
    is_spurv_texture_type<typename SUtils::NthType<0, InnerTypes...>::type>::value;

  /*
   * Types defined by default
   */

  typedef SType<STypeKind::KIND_VOID> void_s;
  typedef SBool bool_s;
  typedef SInt<32, 1> int32_s;
  typedef SInt<32, 1> int_s;
  typedef SInt<32, 0> uint32_s;
  typedef SInt<32, 0> uint_s;
  typedef SFloat<32> float_s;
  typedef SMat<2, 2, float_s> mat2_s;
  typedef SMat<3, 3, float_s> mat3_s;
  typedef SMat<4, 4, float_s> mat4_s;
  typedef SMat<2, 1, float_s> vec2_s;
  typedef SMat<3, 1, float_s> vec3_s;
  typedef SMat<4, 1, float_s> vec4_s;

  typedef SMat<2, 1, int_s>   ivec2_s;
  typedef SMat<3, 1, int_s>   ivec3_s;
  typedef SMat<4, 1, int_s>   ivec4_s;
  typedef SMat<2, 1, uint_s>  uvec2_s;
  typedef SMat<3, 1, uint_s>  uvec3_s;
  typedef SMat<4, 1, uint_s>  uvec4_s;

  typedef SArr<1, STORAGE_INPUT, float_s> arr_1_float_s;

  typedef SRunArr<STORAGE_STORAGE_BUFFER, float_s> float_sarr_s;
  typedef SRunArr<STORAGE_STORAGE_BUFFER, mat2_s> mat2_sarr_s;
  typedef SRunArr<STORAGE_STORAGE_BUFFER, mat3_s> mat3_sarr_s;
  typedef SRunArr<STORAGE_STORAGE_BUFFER, mat4_s> mat4_sarr_s;
  typedef SRunArr<STORAGE_STORAGE_BUFFER, vec2_s> vec2_sarr_s;
  typedef SRunArr<STORAGE_STORAGE_BUFFER, vec3_s> vec3_sarr_s;
  typedef SRunArr<STORAGE_STORAGE_BUFFER, vec4_s> vec4_sarr_s;

  template<typename inner_type>
  using storage_rarray = SRunArr<SStorageClass::STORAGE_STORAGE_BUFFER, inner_type>;

  template<int n, typename inner_type>
  using storage_array = SArr<n, SStorageClass::STORAGE_STORAGE_BUFFER, inner_type>;

  template<int n, typename inner_type>
  using uniform_array = SArr<n, SStorageClass::STORAGE_UNIFORM, inner_type>;

  template<typename... inner_types>
  using sstruct = SStruct<SDecoration::NONE, inner_types...>;

  // Local variables

  template<typename tt>
  using local_s = SPointer<STORAGE_FUNCTION, tt>;

  typedef SImage<1, 0, 0, 0, 0>  image2D_s;
  typedef STexture<2>            texture2D_s;
  
  /*
   * Small type mapper
   */
  
  template<typename tt>
  struct MapSType {
    typedef void type;
  };

  template<>
  struct MapSType<void> {
    typedef void_s type;
  };

  template<>
  struct MapSType<int32_t> {
    typedef int_s type;
  };

  template<>
  struct MapSType<uint32_t> {
    typedef uint_s type;
  };

  template<>
  struct MapSType<float> {
    typedef float_s type;
  };

  template<>
  struct MapSType<double> {
    typedef SFloat<64> type;
  };

  template<int n, int m>
  struct MapSType<falg::Matrix<n, m> > {
    typedef SType<STypeKind::KIND_MAT, n, m> type;
  };


  /*
   * Inverse mapper
   */

  template<typename tt>
  struct InvMapSType {
    typedef void type;
  };

  template<>
  struct InvMapSType<void_s> {
    typedef void type;
  };

  template<>
  struct InvMapSType<uint_s> {
    typedef uint32_t type;
  };

  template<>
  struct InvMapSType<int_s> {
    typedef int32_t type;
  };

  template<>
  struct InvMapSType<float_s> {
    typedef float type;
  };

  template<int n, int m>
  struct InvMapSType<SMat<n, m, float_s> > {
    typedef falg::Matrix<n, m> type;
  };
};

#endif // __SPURV_TYPES
