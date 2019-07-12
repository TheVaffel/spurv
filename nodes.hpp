#ifndef __SPURV_NODES
#define __SPURV_NODES

#include "declarations.hpp"
#include "constant_registry.hpp"

#include <cassert>

namespace spurv {
  
  /*
   * ValueNode member functions
   */
  
  template<typename tt>
  int ValueNode<tt>::getID() const {
    return this->id;
  }
  
  template<typename tt>
  void ValueNode<tt>::declareDefined() {
    this->defined = true;
  }

  template<typename tt>
  bool ValueNode<tt>::isDefined() const {
    return this->defined;
  }

  template<typename tt>
  void ValueNode<tt>::incrementRefCount() {
    this->ref_count++;
  }

  template<typename tt>
  void ValueNode<tt>::print_nodes_post_order(std::ostream& str) const {
    str << this->id << std::endl;
  }

  template<typename tt>
  ValueNode<tt>::ValueNode() { this->id = Utils::getNewID(); this->ref_count = 0; this->defined = false; }

  template<typename tt>
  void ValueNode<tt>::ensure_defined(std::vector<uint32_t>& res) {
    if(this->isDefined()) {
      return;
    }

    this->define(res);
    this->declareDefined();
  }
  
  template<typename tt>
  void ValueNode<tt>::ensure_type_defined(std::vector<uint32_t>& res, std::vector<TypeDeclarationState*>& declaration_states) {
    tt::ensure_defined(res, declaration_states);
  }


  /*
   * Constant member functions
   */

  template<typename tt>
  Constant<tt>::Constant(const tt& val) {
    this->value = val;
  }

  template<typename tt>
  void Constant<tt>::unref_tree() {
    this->ref_count--;
    if(this->ref_count <= 0) {
      delete this;
    }
  }

  template<typename tt>
  void Constant<tt>::define(std::vector<uint32_t>& res) {

    ConstantRegistry::ensureDefinedConstant<tt>(this->value, this->id,
						res);
  }


  /*
   * Var member functions
   */

  template<typename tt>
  Var<tt>::Var() {}
  
  template<typename tt>
  Var<tt>::Var(std::string _name) {
    this->name = _name;
  }
  
  template<typename tt>
  void Var<tt>::unref_tree() {
    this->ref_count--;
    if (this->ref_count <= 0) {
      delete this;
    }
  }

  
  /*
   * UniformVar member functions
   */
  
  template<typename tt>
  UniformVar<tt>::UniformVar(int s, int b, int m, int pointer_id, int parent_struct_id) {
    this->set_no = s;
    this->bind_no = b;
    this->member_no = m;
    this->pointer_id = pointer_id;
    this->parent_struct_id = parent_struct_id;
  }

  template<typename tt>
  void UniformVar<tt>::define(std::vector<uint32_t>& res) {

    int individual_pointer_id = Utils::getNewID();
    
    // OpAccessChain
    Utils::add(res, (5 << 16) | 65);
    Utils::add(res, SpurvPointer<SPURV_STORAGE_UNIFORM, tt>::getID());
    Utils::add(res, individual_pointer_id);
    Utils::add(res, this->parent_struct_id);
    Utils::add(res, ConstantRegistry::getIDInteger(32, 1, this->member_no)); 
    
    // OpLoad
    Utils::add(res, (4 << 16) | 61);
    Utils::add(res, tt::getID());
    Utils::add(res, this->id);
    Utils::add(res, individual_pointer_id);
  }

  template<typename tt>
  void UniformVar<tt>::ensure_type_defined(std::vector<uint32_t>& res,
					      std::vector<TypeDeclarationState*>& declaration_states) {
    tt::ensure_defined(res, declaration_states);
    SpurvPointer<SPURV_STORAGE_UNIFORM, tt>::ensure_defined(res, declaration_states);

    // A little bit hacky, but at least it makes the job done
    // This ensures that the constant int we need to use when
    // accessing this variable within the uniformbinding, is
    // defined
    
    SpurvInt<32, 1>::ensure_defined(res, declaration_states);
    ConstantRegistry::ensureDefinedConstant((int32_t)this->member_no, Utils::getNewID(), res);
  }

  
  /*
   * InputVar member functions
   */
  
  template<typename tt>
  InputVar<tt>::InputVar(int n, int pointer_id) {
    this->input_no = n;
    this->pointer_id = pointer_id;
  }

  template<typename tt>
  void InputVar<tt>::define(std::vector<uint32_t>& res) {
    // OpLoad
    Utils::add(res, (4 << 16) | 61);
    Utils::add(res, tt::getID());
    Utils::add(res, this->id);
    Utils::add(res, this->pointer_id);
  }
};

#endif // __SPURV_NODES
