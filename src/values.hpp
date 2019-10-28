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
  struct SValue {
    static_assert(is_spurv_type<tt>::value);
  protected:
    uint id;
    bool defined;
    uint ref_count;
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
    
  };
  

  /*
   * Constant - Represents nodes that have known value at shader compilation time
   */
    
  template<typename tt>
  struct Constant : public SValue<typename MapSType<tt>::type> {
    Constant(const tt& val);

    virtual void define(std::vector<uint32_t>& res);
    virtual void ensure_type_defined(std::vector<uint32_t>& res,
				     std::vector<SDeclarationState*>& declaration_states);
    
    tt value;
  };


  /*
   * SIOVar - Base class for input attributes and uniforms
   */

  template<typename tt>
  struct SIOVar : public SValue<tt> {
    std::string name;
    SIOVar();
    SIOVar(std::string _name);

  };


  /*
   * SUniformVar - Represents uniforms (duh)
   */
  
  template<typename tt> // n is element number within binding
  struct SUniformVar : public SIOVar<tt> {
    int set_no, bind_no, member_no;
    int pointer_id, parent_struct_id;

    Constant<int>* member_index;
    
    SUniformVar(int s, int b, int m, int pointer_id, int parent_struct_id) ;
    virtual void define(std::vector<uint32_t>& res);
    virtual void ensure_type_defined(std::vector<uint32_t>& res,
				     std::vector<SDeclarationState*>& declaration_states);
  };


  /*
   * InputVar - Represents input attributes
   */
  
  template<typename tt>
  struct InputVar : public SIOVar<tt> {
    int input_no;
    int pointer_id;
    
    InputVar(int n, int pointer_id);

    virtual void define(std::vector<uint32_t>& res);
  };

  
  /*
   * SExpr - Nodes that represent branches of the syntax tree
   */
  
  template<typename tt, SExprOp op, typename tt2 = void_s, typename tt3 = void_s>
  struct SExpr : public SValue<tt> {
    static_assert(is_spurv_type<tt>::value);
    static_assert(is_spurv_type<tt2>::value);
    static_assert(is_spurv_type<tt3>::value);
    
    // Enforce SExpr to be at most binary (surely, this won't come back and bite me later...)
    SValue<tt2>* v1;
    SValue<tt3>* v2;

    SExpr();

    void register_left_node(SValue<tt2>& node);
    void register_right_node(SValue<tt3>& node);

    // Since expressions are returned as references from e.g.
    // binary operations, if you assign a normal (non-reference) variable to an expression resulting
    // from such an operation, it will (if I understand correctly) call the copy constructor, and thus
    // we get a new object put on the stack, which we should not delete, although all its children are
    // heap-allocated, so they should be deleted. Also, this means the top-most node gets no references
    // and is lost in the void
    // Thus, we avoid copies
    SExpr(const SExpr<tt, op, tt2, tt3>& e) = delete;

    virtual void print_nodes_post_order(std::ostream& str) const ;
    virtual void ensure_type_defined(std::vector<uint32_t>& res, std::vector<SDeclarationState*>& declaration_states);
    virtual void define(std::vector<uint32_t>& res);
  };

  
  /*
   * ConstructMatrix - Represents a matrix/vector that is constructed from several other members in shader
   */
  
  template<int n, int m>
  struct ConstructMatrix : public SValue<SMat<n, m> > {
  protected:
    template<typename... Types>
    ConstructMatrix(Types&&... args);
    
    
    template<typename... Types>
    void insertComponents(int u, float f, Types&&... args);

    template<typename First, typename... Types>
    void insertComponents(int u, First& first, Types&&... args);

    virtual void define(std::vector<uint32_t>& res);
    virtual void ensure_type_defined(std::vector<uint32_t>& res,
				     std::vector<SDeclarationState*>& declaration_states);

    SValue<float_s>* components[n * m]; // Values in row-major order
    bool is_constant[n * m]; // #feelsbadman, but oh well
    
    friend class SUtils;
  };
  

  /*
   * SelectConstruct - Represents a conditional choice between two values
   */

  template<typename tt>
  struct SelectConstruct : public SValue<tt> {
    
    SValue<tt> *val_true, *val_false;
    SValue<SBool >* condition;

    SelectConstruct(SValue<SBool>& cond,
		    SValue<tt>& tru_val,
		    SValue<tt>& fal_val);
    
    virtual void define(std::vector<uint32_t>& res);
    virtual void ensure_type_defined(std::vector<uint32_t>& res,
				     std::vector<SDeclarationState*>& declaration_states);
  };


  /*
   * Selection method
   */
  
  template<typename tt>
  SelectConstruct<tt>& select(SValue<SBool>& cond,
			      SValue<tt>& true_val,
			      SValue<tt>& false_val);

  /*
   * Utilities to distinguish value types from everything else
   */

  // Hacky trick incoming
  // https://stackoverflow.com/questions/5997956/how-to-determine-if-a-type-is-derived-from-a-template-class

  template<typename s_type>
  constexpr std::true_type _is_spurv_value(SValue<s_type> const volatile&);
  constexpr std::false_type _is_spurv_value(...);

  template<typename T>
  constexpr bool is_spurv_value(T&& t) {
    return decltype(_is_spurv_value(t))::value;
  }
  
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
