#ifndef __SPURV_TYPES
#define __SPURV_TYPES

#include "declarations.hpp"
#include "utils.hpp"

#include <FlatAlg.hpp>

namespace spurv {
  
  /*
   * Utility struct for SpurvType
   */

  struct TypeDeclarationState {
    TypeDeclarationState();
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
   * DSpurvType - Dynamic (runtime) representation of a SpurvType
   */
  
  struct DSpurvType {
    // NB: If adding more parameters, remember to add more arguments to SpurvType and
    // is_spurv_type below as well
    SpurvTypeKind kind;
    int a0, a1;
    
    // NB: This will be a list if kind == SPURV_TYPE_STRUCT
    DSpurvType* inner_types;
    int num_inner_types;
    
    constexpr DSpurvType() : kind(SPURV_TYPE_INVALID),
      a0(0), a1(0), inner_types(nullptr), num_inner_types(0) { }

    ~DSpurvType() ;
    
    bool operator==(const DSpurvType& ds) const;
  };
  

  /*
   * SpurvType - The mother of them all
   */
  
  template<SpurvTypeKind kind, int arg0 = 0, int arg1 = 0, typename... InnerTypes >
  class SpurvType {
    
  protected:
    
    static TypeDeclarationState declarationState;
    static void declareDefined();
  public:
  
    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<TypeDeclarationState*>& declaration_states);
    static void ensure_defined_dependencies(std::vector<uint32_t>& bin, std::vector<TypeDeclarationState*>& declaration_states);
    static void define(std::vector<uint32_t>& bin);
    
    static void getDSpurvType(DSpurvType* type);

    static int getID();
    static bool isDefined();
    static int ensureInitID();
    
    static constexpr SpurvTypeKind getKind();
    static constexpr int getArg0();
    static constexpr int getArg1();

    static constexpr int getSize();
  };

    
  /*
   * SpurvInt - Representation of integers
   */

  template<int n, int signedness>
  class SpurvInt : public SpurvType<SPURV_TYPE_INT, n, signedness> {
  public:
    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<TypeDeclarationState*>& declaration_states);
    static void define(std::vector<uint32_t>& bin);
    static constexpr int getSize();
  };
  

  /*
   * SpurvFloat - Representation of floats
   */
  
  template<int n>
  class SpurvFloat : public SpurvType<SPURV_TYPE_FLOAT, n> {
  public:
    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<TypeDeclarationState*>& declaration_states);
    static void define(std::vector<uint32_t>& bin);
    static constexpr int getSize();
  };
  

  /*
   * SpurvMat - Representation of vectors and matrices
   */
  
  template<int n, int m>
  class SpurvMat : public SpurvType<SPURV_TYPE_MAT, n, m> {
  public:
    static void ensure_defined_dependencies(std::vector<uint32_t>& bin,
					    std::vector<TypeDeclarationState*>& declaration_states);
    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<TypeDeclarationState*>& declaration_states);
    static void define(std::vector<uint32_t>& bin);
    static constexpr int getSize();
  };

  
  /*
   * Static explicit type checks - more general (more checks below)
   */
  
  template<typename>
  struct is_spurv_type : std::false_type {};

  template<int n, int s>
  struct is_spurv_type<SpurvInt<n, s> > : std::true_type {};

  template<int n>
  struct is_spurv_type<SpurvFloat<n> > : std::true_type {};

  template<int n, int m>
  struct is_spurv_type<SpurvMat<n, m> > : std::true_type {};

  template<>
  struct is_spurv_type<NullType> : std::true_type {};


  /*
   * SpurvArr - Representation of arrays
   */
  
  template<int n, typename tt>
  class SpurvArr : public SpurvType<SPURV_TYPE_ARR, n, 0, tt >  {
    constexpr SpurvArr() {
      static_assert(is_spurv_type<tt>::value, "Inner type of SpurvArr must be a spurv type");
    }

  public:    
    static void ensure_defined_dependencies(std::vector<uint32_t>& bin,
					    std::vector<TypeDeclarationState*>& declaration_states);
    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<TypeDeclarationState*>& declaration_states);
    static void define(std::vector<uint32_t>& bin);
    static constexpr int getSize();
  };
  

  /*
   * SpurvPointer - Representation of pointers
   */
  
  template<SpurvStorageClass storage, typename tt>
  class SpurvPointer : public SpurvType<SPURV_TYPE_POINTER, (int)storage, 0, tt> {
  public:
    static void ensure_defined_dependencies(std::vector<uint32_t>& bin,
					    std::vector<TypeDeclarationState*>& declaration_states);
    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<TypeDeclarationState*>& declaration_states);
    static void define(std::vector<uint32_t>& bin);
  };


  /*
   * SpurvStruct - Representation of structs
   */
  
  template<typename... InnerTypes>
  class SpurvStruct : public SpurvType<SPURV_TYPE_STRUCT, 0, 0, InnerTypes...> {
    static bool is_decorated;
    
  public:
    
    static void ensure_defined_dependencies(std::vector<uint32_t>& bin,
					    std::vector<TypeDeclarationState*>& declaration_states);
    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<TypeDeclarationState*>& declaration_states);
    static void define(std::vector<uint32_t>& bin);
    static void ensure_decorated(std::vector<uint32_t>& bin);
    static void decorate_member_offsets(std::vector<uint32_t>& bin);
    
    static void getDSpurvType(DSpurvType* type);
    static constexpr int getSize();

    template<int member_no, int start_size, typename First, typename... Types>
    static void decorate_member_offsets(std::vector<uint32_t>& bin);
  };

    
  /*
   * More specialized type checks
   */
  
  template<typename>
  struct is_spurv_mat_type : std::false_type {};
  
  template<int n, typename tt>
  struct is_spurv_type<SpurvArr<n, tt> > : std::true_type { static_assert(is_spurv_type<tt>::value); };

  template<SpurvStorageClass storage, typename tt>
  struct is_spurv_type<SpurvPointer<storage, tt> > : std::true_type { static_assert(is_spurv_type<tt>::value); };

  template<typename... InnerTypes>
  struct is_spurv_type<SpurvStruct<InnerTypes...> > : std::true_type{ static_assert(Utils::isSpurvTypeRecursive<InnerTypes...>); };

  template<int n, int m>
  struct is_spurv_mat_type<SpurvMat<n, m> > : std::true_type {};

  
  template<typename>
  struct is_spurv_float_type : std::false_type {};
  
  template<int n>
  struct is_spurv_float_type<SpurvFloat<n> > : std::true_type {};

  template<typename>
  struct is_spurv_int_type : std::false_type {};

  template<int n, int s>
  struct is_spurv_int_type<SpurvInt<n, s> > : std::true_type {};
  

  /*
   * Types defined by default
   */
  
  typedef SpurvType<SPURV_TYPE_VOID> void_s;
  typedef SpurvInt<32, 1> int_s;
  typedef SpurvInt<32, 0> uint_s;
  typedef SpurvFloat<32> float_s;
  typedef SpurvMat<2, 2> mat2_s;
  typedef SpurvMat<3, 3> mat3_s;
  typedef SpurvMat<4, 4> mat4_s;
  typedef SpurvMat<2, 1> vec2_s;
  typedef SpurvMat<3, 1> vec3_s;
  typedef SpurvMat<4, 1> vec4_s;
  typedef SpurvArr<1, float_s> arr_1_float_s;

  
  /*
   * Small type mapper
   */
  
  template<typename tt>
  struct MapSpurvType;

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
