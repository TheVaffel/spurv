#ifndef __SPURV_NODES
#define __SPURV_NODES

#include "declarations.hpp"
#include "constant_registry.hpp"
#include "types.hpp"

#include <cassert>

namespace spurv {
  
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

    SValue();
    
    virtual void print_nodes_post_order(std::ostream& str) const;

    int getID() const;
    void declareDefined();
    bool isDefined() const;
    void incrementRefCount();

    void ensure_defined(std::vector<uint32_t>& res);
    
    virtual void define(std::vector<uint32_t>& res) = 0;

    virtual void ensure_type_defined(std::vector<uint32_t>& res, std::vector<SDeclarationState*>& declaration_states);
      
    // Only deletes tree if ref_count reaches zero
    virtual void unref_tree() = 0;
  };


  /*
   * Constant - Represents nodes that have known value at shader compilation time
   */
  template<typename tt>
  struct Constant : public SValue<typename MapSType<tt>::type> {
    Constant(const tt& val);

    virtual void unref_tree();

    virtual void define(std::vector<uint32_t>& res);
    
    tt value;
  };


  /*
   * SIOValue - Base class for input attributes and uniforms
   */

  template<typename tt>
  struct SIOVar : public SValue<tt> {
    std::string name;
    SIOVar();
    SIOVar(std::string _name);
    virtual void unref_tree();
  };


  /*
   * SUniformVar - Represents uniforms (duh)
   */
  
  template<typename tt> // n is element number within binding
  struct SUniformVar : public SIOVar<tt> {
    int set_no, bind_no, member_no;
    int pointer_id, parent_struct_id;
    
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
    virtual void unref_tree();
    virtual void ensure_type_defined(std::vector<uint32_t>& res, std::vector<SDeclarationState*>& declaration_states);
    virtual void define(std::vector<uint32_t>& res);
  };


  /*
   * SValue node types defined by default
   */

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
};

#endif // __SPURV_NODES
