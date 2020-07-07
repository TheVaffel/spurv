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

  template<int n>
  struct lookup_result<SMat<n, 1> > {
    using type = float_s;
  };

  
  /*
   * Utility for determining result type of lookups
   */
  
  template<typename tt>
  struct lookup_index {
    using type = void_s;
  };

  template<int n>
  struct lookup_index<STexture<n> > {
    using type = SMat<n, 1>;
  };

  template<int n>
  struct lookup_index<SMat<n, 1> > {
    using type = int_s;
  };

  
  /*
   * SValue - The mother of all nodes in the syntax trees
   */
  
  template<typename tt>
  class SValue {
    static_assert(is_spurv_type<tt>::value);
  protected:
    unsigned int id;
    bool defined;
    unsigned int ref_count;
  public:

    typedef tt type;
    
    SValue();
    
    virtual void print_nodes_post_order(std::ostream& str) const;

    int getID() const;
    
    void ensure_defined(std::vector<uint32_t>& res);
    
    virtual void define(std::vector<uint32_t>& res) = 0;

    virtual void ensure_type_defined(std::vector<uint32_t>& res,
				     std::vector<SDeclarationState*>& declaration_states);

    SValue<typename lookup_result<tt>::type>& operator[](SValue<typename lookup_index<tt>::type >& index);
    
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

  /*
   * SPointerVar - Base class for pointer-based values (input attributes, uniforms, builtins etc.)
   * NB: Currently, since it only contains one ID from SValue, it only supports loading once. 
   * Yeah... It is a bit of a mess
   */

  template<typename tt, SStorageClass storage>
  class SPointerVar : public SValue<tt> {
  protected:
    int pointer_id;
    SPointerVar(int pointer_id);
  public:
    virtual void define(std::vector<uint32_t>& res);
    virtual void ensure_type_defined(std::vector<uint32_t>& res,
				     std::vector<SDeclarationState*>& declaration_states);

    friend class SUtils;
  };


  /*
   * SUniformVar - Represents uniforms (duh)
   */
  
  template<typename tt> // n is element number within binding
  class SUniformVar : public SPointerVar<tt, STORAGE_UNIFORM> {
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
    
    InputVar(int n, int pointer_id);

  public:
    // virtual void define(std::vector<uint32_t>& res);

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

      void register_left_node(SValue<tt2>& node);
      void register_right_node(SValue<tt3>& node);

      friend class SUtils;
  };
  
  
  /*
   * ConstructMatrix - Represents a matrix/vector that is constructed from several other members in shader
   */
  
  template<int n, int m>
  class ConstructMatrix : public SValue<SMat<n, m> > {
  protected:
    template<typename... Types>
    ConstructMatrix(Types&&... args);
    
    template<typename t1, typename... trest>
    void insertComponents(int u, t1&& first, trest&&... args);

  public:
    virtual void define(std::vector<uint32_t>& res);
    virtual void ensure_type_defined(std::vector<uint32_t>& res,
				     std::vector<SDeclarationState*>& declaration_states);

    SValue<float_s>* components[n * m]; // Values in row-major order
    
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
    
    friend class SUtils;
  };


  /*
   * Selection method
   */
  
  template<typename tt, typename t1, typename t2, typename t3>
  SelectConstruct<tt>& select(t1&& cond,
			      t2&& true_val,
			      t3&& false_val);


  /*
   * Utilities to distinguish value types from everything else
   */

  template<typename>
  struct is_spurv_value : std::false_type {};
  
  template<typename T>
  struct is_spurv_value<SValue<T> > : std::true_type {};

  template<typename t1, SExprOp op, typename t2, typename t3>
  struct is_spurv_value<SExpr<t1, op, t2, t3> > : std::true_type {};

  template<int n, int m>
  struct is_spurv_value<ConstructMatrix<n, m> > : std::true_type {};
  
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

};

#endif // __SPURV_NODES
