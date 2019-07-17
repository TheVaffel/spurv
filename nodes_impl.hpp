#ifndef __SPURV_NODES_IMPL
#define __SPURV_NODES_IMPL

#include "nodes.hpp"
#include "constant_registry_impl.hpp"

namespace spurv {
    
  /*
   * SValue member functions
   */
  
  template<typename tt>
  int SValue<tt>::getID() const {
    return this->id;
  }
  
  template<typename tt>
  void SValue<tt>::declareDefined() {
    this->defined = true;
  }

  template<typename tt>
  bool SValue<tt>::isDefined() const {
    return this->defined;
  }

  template<typename tt>
  void SValue<tt>::incrementRefCount() {
    this->ref_count++;
  }

  template<typename tt>
  void SValue<tt>::print_nodes_post_order(std::ostream& str) const {
    str << this->id << std::endl;
  }

  template<typename tt>
  SValue<tt>::SValue() { this->id = SUtils::getNewID(); this->ref_count = 0; this->defined = false; }

  template<typename tt>
  void SValue<tt>::ensure_defined(std::vector<uint32_t>& res) {
    if(this->isDefined()) {
      return;
    }

    this->define(res);
    this->declareDefined();
  }
  
  template<typename tt>
  void SValue<tt>::ensure_type_defined(std::vector<uint32_t>& res, std::vector<SDeclarationState*>& declaration_states) {
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

    SConstantRegistry::ensureDefinedConstant<tt>(this->value, this->id,
						res);
  }


  /*
   * Var member functions
   */

  template<typename tt>
  SIOVar<tt>::SIOVar() {}
  
  template<typename tt>
  SIOVar<tt>::SIOVar(std::string _name) {
    this->name = _name;
  }
  
  template<typename tt>
  void SIOVar<tt>::unref_tree() {
    this->ref_count--;
    if (this->ref_count <= 0) {
      delete this;
    }
  }

  
  /*
   * SUniformVar member functions
   */
  
  template<typename tt>
  SUniformVar<tt>::SUniformVar(int s, int b, int m, int pointer_id, int parent_struct_id) {
    this->set_no = s;
    this->bind_no = b;
    this->member_no = m;
    this->pointer_id = pointer_id;
    this->parent_struct_id = parent_struct_id;
  }

  template<typename tt>
  void SUniformVar<tt>::define(std::vector<uint32_t>& res) {

    int individual_pointer_id = SUtils::getNewID();
    
    // OpAccessChain
    SUtils::add(res, (5 << 16) | 65);
    SUtils::add(res, SPointer<STORAGE_UNIFORM, tt>::getID());
    SUtils::add(res, individual_pointer_id);
    SUtils::add(res, this->parent_struct_id);
    SUtils::add(res, SConstantRegistry::getIDInteger(32, 1, this->member_no)); 
    
    // OpLoad
    SUtils::add(res, (4 << 16) | 61);
    SUtils::add(res, tt::getID());
    SUtils::add(res, this->id);
    SUtils::add(res, individual_pointer_id);
  }

  template<typename tt>
  void SUniformVar<tt>::ensure_type_defined(std::vector<uint32_t>& res,
					      std::vector<SDeclarationState*>& declaration_states) {
    tt::ensure_defined(res, declaration_states);
    SPointer<STORAGE_UNIFORM, tt>::ensure_defined(res, declaration_states);

    // A little bit hacky, but at least it makes the job done
    // This ensures that the constant int we need to use when
    // accessing this variable within the uniformbinding, is
    // defined
    
    SInt<32, 1>::ensure_defined(res, declaration_states);
    SConstantRegistry::ensureDefinedConstant((int32_t)this->member_no, SUtils::getNewID(), res);
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
    SUtils::add(res, (4 << 16) | 61);
    SUtils::add(res, tt::getID());
    SUtils::add(res, this->id);
    SUtils::add(res, this->pointer_id);
  }
};

#endif // __SPURV_NODES_IMPL
