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
    
    // NB: This will be a list if kind == KIND_STRUCT
    DSType* inner_types;
    int num_inner_types;
    
    constexpr DSType() : kind(STypeKind::KIND_INVALID),
      a0(0), a1(0), a2(0), a3(0), a4(0),
      inner_types(nullptr),  
      num_inner_types(0){ };

    ~DSType() ;
    
    bool operator==(const DSType& ds) const;
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
    
    static void getDSType(DSType* type);

    static int getID();
    static bool isDefined();
    static int ensureInitID();
    
    static constexpr STypeKind getKind();
    static constexpr int getArg0();
    static constexpr int getArg1();

    static constexpr int getSize();
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

    static SValue<SInt<n, signedness> >& cons(int64_t arg);
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
    static SValue<SFloat<n> >& cons(tt arg);
  };
  

  /*
   * SMat - Representation of vectors and matrices
   */
  
  template<int n, int m>
  class SMat : public SType<STypeKind::KIND_MAT, n, m> {
  public:
    static void ensure_defined_dependencies(std::vector<uint32_t>& bin,
					    std::vector<SDeclarationState*>& declaration_states);
    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<SDeclarationState*>& declaration_states);
    static void define(std::vector<uint32_t>& bin);
    static constexpr int getSize();

    template<typename... Types>
    static ConstructMatrix<n, m>& cons(Types&&... args);
  };

  
  /*
   * Static explicit type checks - more general (more checks below)
   */
  
  template<typename>
  struct is_spurv_type : std::false_type {};

  template<>
  struct is_spurv_type<SBool> : std::true_type {};

  template<int n, int s>
  struct is_spurv_type<SInt<n, s> > : std::true_type {};

  template<int n>
  struct is_spurv_type<SFloat<n> > : std::true_type {};

  template<int n, int m>
  struct is_spurv_type<SMat<n, m> > : std::true_type {};

  template<>
  struct is_spurv_type<NullType> : std::true_type {};


  /*
   * SArr - Representation of arrays
   */
  
  template<int n, typename tt>
  class SArr : public SType<STypeKind::KIND_ARR, n, 0, 0, 0, 0, tt >  {
    constexpr SArr() {
      static_assert(is_spurv_type<tt>::value, "Inner type of SArr must be a spurv type");
    }

  public:    
    static void ensure_defined_dependencies(std::vector<uint32_t>& bin,
					    std::vector<SDeclarationState*>& declaration_states);
    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<SDeclarationState*>& declaration_states);
    static void define(std::vector<uint32_t>& bin);
    static constexpr int getSize();
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
  };


  /*
   * SStruct - Representation of structs
   */
  
  template<typename... InnerTypes>
  class SStruct : public SType<STypeKind::KIND_STRUCT, 0, 0, 0, 0, 0, InnerTypes...> {
    static bool is_decorated;
    
  public:
    
    static void ensure_defined_dependencies(std::vector<uint32_t>& bin,
					    std::vector<SDeclarationState*>& declaration_states);
    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<SDeclarationState*>& declaration_states);
    static void define(std::vector<uint32_t>& bin);
    static void ensure_decorated(std::vector<uint32_t>& bin,
				 std::vector<bool*>& decoration_states);
    static void decorate_member_offsets(std::vector<uint32_t>& bin);
    
    static constexpr int getSize();

    template<int member_no, int start_size, typename First, typename... Types>
    static void decorate_member_offsets(std::vector<uint32_t>& bin);
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
  
  template<int n, typename tt>
  struct is_spurv_type<SArr<n, tt> > : std::true_type { static_assert(is_spurv_type<tt>::value); };

  template<SStorageClass storage, typename tt>
  struct is_spurv_type<SPointer<storage, tt> > : std::true_type { static_assert(is_spurv_type<tt>::value); };

  template<typename... InnerTypes>
  struct is_spurv_type<SStruct<InnerTypes...> > : std::true_type{ static_assert(SUtils::isSTypeRecursive<InnerTypes...>); };
  
  template<int d>
  struct is_spurv_type<STexture<d> > : std::true_type {} ;

  template<int n, int m>
  struct is_spurv_mat_type<SMat<n, m> > : std::true_type {};

  
  template<typename>
  struct is_spurv_float_type : std::false_type {};
  
  template<int n>
  struct is_spurv_float_type<SFloat<n> > : std::true_type {};

  template<typename>
  struct is_spurv_int_type : std::false_type {};

  template<int n, int s>
  struct is_spurv_int_type<SInt<n, s> > : std::true_type {};

  template<typename>
  struct is_spurv_texture_type : std::false_type {};

  template<int n>
  struct is_spurv_texture_type<STexture<n> > : std::true_type {};


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
  typedef SInt<32, 1> int_s;
  typedef SInt<32, 0> uint_s;
  typedef SFloat<32> float_s;
  typedef SMat<2, 2> mat2_s;
  typedef SMat<3, 3> mat3_s;
  typedef SMat<4, 4> mat4_s;
  typedef SMat<2, 1> vec2_s;
  typedef SMat<3, 1> vec3_s;
  typedef SMat<4, 1> vec4_s;
  typedef SArr<1, float_s> arr_1_float_s;
  typedef STexture<2> texture2D_s;

  
  /*
   * Small type mapper
   */
  
  template<typename tt>
  struct MapSType;

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
  struct InvMapSType<float_s> {
    typedef float type;
  };

  template<int n, int m>
  struct InvMapSType<SMat<n, m> > {
    typedef falg::Matrix<n, m> type;
  };
};

#endif // __SPURV_TYPES
