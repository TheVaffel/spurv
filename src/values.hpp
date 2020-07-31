#ifndef __SPURV_NODES
#define __SPURV_NODES

#include "declarations.hpp"
#include "constant_registry.hpp"
#include "types.hpp"

#include <cassert>
#include <vector>

namespace spurv {

  /*
   * Utility for determining result type of lookups
   */
  
  template<typename tt>
  struct lookup_result {
    using type = void_s;
  };

  template<int n>
  struct lookup_result<STexture<n> > {
    using type = vec4_s;
  };

  template<int n, typename inner>
  struct lookup_result<SMat<n, 1, inner> > {
    using type = inner;
  };

  template<int n, SStorageClass storage, typename inner>
  struct lookup_result<SArr<n, storage, inner> > {
    using type = inner;
  };

  template<SStorageClass storage, typename inner>
  struct lookup_result<SRunArr<storage, inner> > {
    using type = inner;
  };

  
  /*
   * Utility for determining result type of lookups
   */
  
  template<typename tt, typename ti>
  struct is_lookup_index : std::false_type {};

  template<int n>
  struct is_lookup_index<STexture<n>, SMat<n, 1, float_s>  > : std::true_type {};

  template<int n, typename inner, int iw, int is>
  struct is_lookup_index<SMat<n, 1, inner>, SInt<iw, is> > : std::true_type {};
  
  template<int n, SStorageClass storage, typename inner, int iw, int is>
  struct is_lookup_index<SArr<n, storage, inner>, SInt<iw, is> > : std::true_type {};

  template<SStorageClass storage, typename inner, int iw, int is>
  struct is_lookup_index<SRunArr<storage, inner>, SInt<iw, is> > : std::true_type {};

  
  /*
   * SValue - The mother of all nodes in the syntax trees
   */
  
  template<typename tt>
  class SValue {
    static_assert(is_spurv_type<tt>::value);
  protected:
    unsigned int id;
    bool defined;
  public:

    typedef tt type;
    
    SValue();
    
    virtual void print_nodes_post_order(std::ostream& str) const;

    int getID() const;
    
    void ensure_defined(std::vector<uint32_t>& res);

    virtual void ensure_type_decorated(std::vector<uint32_t>& bin,
				       std::vector<bool*>& decoration_states);
    
    virtual void define(std::vector<uint32_t>& res) = 0;

    virtual void ensure_type_defined(std::vector<uint32_t>& res,
				     std::vector<SDeclarationState*>& declaration_states);

    template<typename ti>
    SValue<typename lookup_result<tt>::type>& operator[](SValue<ti>& index);
    
    SValue<typename lookup_result<tt>::type>& operator[](int s); // Lookup operator that requires constants
    
    friend class SUtils;
  };
  

  /*
   * Constant - Represents nodes that have known value at shader compilation time
   */
    
  template<typename tt>
  class Constant : public SValue<typename MapSType<tt>::type> {
    Constant(const tt& val);

  public:
    virtual void define(std::vector<uint32_t>& res);
    virtual void ensure_type_defined(std::vector<uint32_t>& res,
				     std::vector<SDeclarationState*>& declaration_states);
    
    tt value;

    friend class SUtils;
  };

  
  // /*
  //  * SPointerVar - Base class for pointer-based values (input attributes, uniforms, builtins etc.)
  //  * NB: Currently, since it only contains one ID from SValue, it only supports loading once. 
  //  * Yeah... It is a bit of a mess
  //  */

  // template<typename tt, SStorageClass storage>
  // class SPointerVar : public SValue<tt> {
  // protected:
  //   int pointer_id;
  //   SPointerVar(int pointer_id);
  // public:
  //   virtual void define(std::vector<uint32_t>& res);
  //   virtual void ensure_type_defined(std::vector<uint32_t>& res,
  // 				     std::vector<SDeclarationState*>& declaration_states);
  //   virtual void ensure_type_decorated(std::vector<uint32_t>& bin,
  // 				       std::vector<bool*>& decoration_states);

  //   friend class SUtils;
  // };


  /*
   * SUniformVar - Represents uniforms (duh)
   */
  
  template<SStorageClass storage, typename tt> // n is element number within binding
  class SUniformVar : public SPointerVar<tt, storage> {
    int set_no, bind_no, member_no;
    int pointer_id, parent_struct_id;

    Constant<int>* member_index;
    
    SUniformVar(int s, int b, int m, int pointer_id, int parent_struct_id) ;
  public:
    virtual void define(std::vector<uint32_t>& res);
    virtual void ensure_type_defined(std::vector<uint32_t>& res,
				     std::vector<SDeclarationState*>& declaration_states);

    friend class SUtils;
  };


  /*
   * InputVar - Represents input attributes
   */
  
  template<typename tt>
  class InputVar : public SPointerVar<tt, STORAGE_INPUT> {
    int input_no;
    
    InputVar(int n);

  public:
    // virtual void define(std::vector<uint32_t>& res);

    friend class SUtils;
  };
  


  /* 
   * SLocal - represents a pointer to local variable
   */

  template<typename tt>
  class SLocal : public SPointerVar<tt, SStorageClass::STORAGE_FUNCTION> {

    SLocal();
    
    friend class SForLoop;
    friend class SUtils;
  };

  
  /*
   * SCustomVal - values that have a custom initialization routine
   */

  template<typename tt>
  class SCustomVal : public SValue<tt> {
    virtual void define(std::vector<uint32_t>& res);

    friend class SUtils;
  };


  /*
   * SGLSLHomoFun - Nodes representing "homogenous" functions (domain and range are
   * of equal type) in GLSL
   */

  template<typename tt>
  class SGLSLHomoFun : public SValue<tt> {
    static_assert(is_spurv_type<tt>::value);

    GLSLFunction opcode;
    std::vector<SValue<tt>* > args;
    
    SGLSLHomoFun(GLSLFunction opcode, const std::vector<SValue<tt>* >& args);
    

  public:
    virtual void define(std::vector<uint32_t>& res);

    friend class SUtils;
  };
  
  
  /*
   * SExpr - Nodes that represent branches of the syntax tree
   */
  
  template<typename tt, SExprOp op, typename tt2 = void_s, typename tt3 = void_s>
  class SExpr : public SValue<tt> {
    static_assert(is_spurv_type<tt>::value);
    static_assert(is_spurv_type<tt2>::value);
    static_assert(is_spurv_type<tt3>::value);
    
    // Enforce SExpr to be at most binary (surely, this won't come back and bite me later...)
    SValue<tt2>* v1;
    SValue<tt3>* v2;

    SExpr();

    // Since expressions are returned as references from e.g.
    // binary operations, if you assign a normal (non-reference) variable to an expression resulting
    // from such an operation, it will (if I understand correctly) call the copy constructor, and thus
    // we get a new object put on the stack, which we should not delete, although all its children are
    // heap-allocated, so they should be deleted. Also, this means the top-most node gets no references
    // and is lost in the void
    // Thus, we avoid copies
    SExpr(const SExpr<tt, op, tt2, tt3>& e) = delete;


  public:
    virtual void print_nodes_post_order(std::ostream& str) const ;
    virtual void ensure_type_defined(std::vector<uint32_t>& res, std::vector<SDeclarationState*>& declaration_states);
    virtual void define(std::vector<uint32_t>& res);
    virtual void ensure_type_decorated(std::vector<uint32_t>& bin,
				       std::vector<bool*>& decoration_states);

      void register_left_node(SValue<tt2>& node);
      void register_right_node(SValue<tt3>& node);

      friend class SUtils;
  };
  
  
  /*
   * ConstructMatrix - Represents a matrix/vector that is constructed from several other members in shader
   */
  
  template<int n, int m, typename inner>
  class ConstructMatrix : public SValue<SMat<n, m, inner> > {
  protected:

    bool using_columns();
    
    template<typename... Types>
    ConstructMatrix(Types&&... args);
    
    template<typename t1, typename... trest>
    void insertComponents(int u, t1&& first, trest&&... args);

    template<typename t1, typename... trest>
    void insertColumns(int u, t1&& first, trest&&... args);

    std::vector<void*> components; // Values in row-major order

  public:
    virtual void define(std::vector<uint32_t>& res);
    virtual void ensure_type_defined(std::vector<uint32_t>& res,
				     std::vector<SDeclarationState*>& declaration_states);
  
    friend class SUtils;
  };


  /*
   * SelectConstruct - Represents a conditional choice between two values
   */

  template<typename tt>
  class SelectConstruct : public SValue<tt> {
    
    SValue<tt> *val_true, *val_false;
    SValue<SBool >* condition;

    SelectConstruct(SValue<SBool>& cond,
		    SValue<tt>& true_val,
		    SValue<tt>& false_val);
    
  public:
    virtual void define(std::vector<uint32_t>& res);
    virtual void ensure_type_defined(std::vector<uint32_t>& res,
				     std::vector<SDeclarationState*>& declaration_states);
    virtual void ensure_type_decorated(std::vector<uint32_t>& bin,
				       std::vector<bool*>& decoration_states);
    
    friend class SUtils;
  };


  /*
   * Utilities to distinguish value types from everything else
   */

  template<typename>
  struct is_spurv_value : std::false_type {};

  
  template<typename T>
  struct is_spurv_value<SValue<T> > : std::true_type {};

  // template<typename T>
  // struct is_spurv_value<SValue<T>& > : std::true_type {};
  
  template<typename t1, SExprOp op, typename t2, typename t3>
  struct is_spurv_value<SExpr<t1, op, t2, t3> > : std::true_type {};

  template<int n, int m, typename inner>
  struct is_spurv_value<ConstructMatrix<n, m, inner> > : std::true_type {};

  template<typename tt>
  struct is_spurv_value<SGLSLHomoFun<tt> > : std::true_type {};

  template<typename tt>
  struct is_spurv_value<InputVar<tt> > : std::true_type {};

  template<typename tt>
  struct is_spurv_value<Constant<tt> > : std::true_type {};

  template<typename tt, SStorageClass storage>
  struct is_spurv_value<SPointerVar<tt, storage> > : std::true_type {};

  template<SStorageClass storage, typename tt>
  struct is_spurv_value<SUniformVar<storage, tt> > : std::true_type {};

  template<typename tt>
  struct is_spurv_value<SLocal<tt> > : std::true_type {};

  template<typename tt, SStorageClass stind>
  struct is_spurv_value<SLoadedVal<tt, stind> > : std::true_type {};

  template<typename tt>
  struct is_spurv_value<SCustomVal<tt> > : std::true_type {};

  template<typename tt>
  struct is_spurv_value<SelectConstruct<tt> > : std::true_type {};

  template<typename T>
  struct is_spurv_value<T&> : is_spurv_value<typename std::remove_reference<T>::type> {};
  
  
  /*
   * SValue node types defined by default
   */

  typedef SValue<bool_s>&        bool_v;
  typedef SValue<int_s>&         int_v;
  typedef SValue<uint_s>&        uint_v;
  typedef SValue<float_s>&       float_v;
  typedef SValue<mat2_s>&        mat2_v;
  typedef SValue<mat3_s>&        mat3_v;
  typedef SValue<mat4_s>&        mat4_v;
  typedef SValue<vec2_s>&        vec2_v;
  typedef SValue<vec3_s>&        vec3_v;
  typedef SValue<vec4_s>&        vec4_v;
  typedef SValue<arr_1_float_s>& arr_1_float_v;
  typedef SValue<texture2D_s>&   texture2D_v;

  typedef SValue<float_sarr_s>&   float_sarr_v;
  typedef SValue<mat2_sarr_s>&    mat2_sarr_v;
  typedef SValue<mat3_sarr_s>&    mat3_sarr_v;
  typedef SValue<mat4_sarr_s>&    mat4_sarr_v;
  typedef SValue<vec2_sarr_s>&    vec2_sarr_v;
  typedef SValue<vec3_sarr_s>&    vec3_sarr_v;
  typedef SValue<vec4_sarr_s>&    vec4_sarr_v;

  typedef SLocal<float_s>&       float_lv;
  typedef SLocal<int_s>&         int_lv;
  typedef SLocal<uint_s>&        uint_lv;
  typedef SLocal<mat2_s>&        mat2_lv;
  typedef SLocal<mat3_s>&        mat3_lv;
  typedef SLocal<mat4_s>&        mat4_lv;
  typedef SLocal<vec2_s>&        vec2_lv;
  typedef SLocal<vec3_s>&        vec3_lv;
  typedef SLocal<vec4_s>&        vec4_lv;

};

#endif // __SPURV_NODES
