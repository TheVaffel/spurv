#ifndef __SPURV_NODES
#define __SPURV_NODES

#include "declarations.hpp"
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
  void ValueNode<tt>::ensure_type_defined(std::vector<uint32_t>& res, std::vector<int32_t*>& ids) {
    tt::ensure_defined(res, ids);
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
    DSpurvType ds;
    MapSpurvType<tt>::type::getDSpurvType(&ds);
    if (ds.kind == SPURV_TYPE_FLOAT ||
	ds.kind == SPURV_TYPE_INT) {
      if(ds.a0 != 32) {
	printf("Bitwidth of 32 is the only supported at this point\n");
	exit(-1);
      }
      assert(ds.a0 == 32);
      Utils::add(res, (4 << 16) | 43);
      Utils::add(res, MapSpurvType<tt>::type::getID());
      Utils::add(res, this->getID());
      Utils::add(res, *(int32_t*)(&this->value));
    } else {
      printf("Constants not of integer or float type not yet supported");
      exit(-1);
    }

    this->declareDefined();
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
  UniformVar<tt>::UniformVar(int s, int b) {
    this->set_no = s;
    this->bind_no = b;
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

    this->declareDefined();
  }
};

#endif // __SPURV_NODES
