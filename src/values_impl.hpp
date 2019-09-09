#ifndef __SPURV_NODES_IMPL
#define __SPURV_NODES_IMPL

#include "values.hpp"
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

    // Update ID in case it has been overwritten
    // The hack level is enormous, I know
    if constexpr(std::is_same<tt, float>::value) {
	this->id = SConstantRegistry::getIDFloat(32, this->value);
      } else if(std::is_same<tt, int>::value) {
      this->id = SConstantRegistry::getIDInteger(32, 1, this->value);
    } else {
      this->id = SConstantRegistry::getIDInteger(32, 0, this->value);
    }
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

  
  /*
   * ConstructMatrix member functions
   */

  template<int n, int m>
  template<typename... Types>
  ConstructMatrix<n, m>::ConstructMatrix(const Types&... args) {
    static_assert(sizeof...(args) == n * m,
		  "Number of arguments to matrix construction does not match number of components in matrix");
    insertComponents(0, args...);
  }

  template<int n, int m>
  template<typename First, typename... Types>
  void ConstructMatrix<n, m>::insertComponents(int u, const First& first, const Types&... args) {

    if constexpr(std::is_same<First, float_s>::value) {
	this->components[u] = &first;

	this->is_constant[u] = false;
	
      } else if(std::is_convertible<First, float>::value) {

      Constant<float>* constant = SUtils::allocate<Constant<float> >(static_cast<float>(first));
      
      this->components[u] = constant;
      this->is_constant[u] = true;
      
    } else {
      // static_assert(false, "Datatypes given to ConstructMatrix must be convertible to float");
      std::cout << "Datatypes given to ConstructMatrix must be convertible to float" << std::endl;
    }

    this->components[u]->incrementRefCount();

    if constexpr(sizeof...(args) > 0) {
	insertComponents(u + 1, args...);
      }
  }

  template<int n, int m>
  void ConstructMatrix<n, m>::unref_tree() {
    this->ref_count--;
    if(this->ref_count <= 0) {
      for(int i = 0; i < n * m; i++) {
	this->components[i]->unref_tree();
      }
      delete this;
    }
  }

  template<int n, int m>
  void ConstructMatrix<n, m>::define(std::vector<uint32_t>& res) {
    for(int i = 0; i < n * m; i++) {
      this->components[i]->ensure_defined(res);
    }

    // OpCompositeConstruct <result type> <resul id> <components...>
    SUtils::add(res, ((3 + n * m) << 16) | 80);
    SUtils::add(res, SMat<n, m>::getID());
    SUtils::add(res, this->id);

    for(int i = 0; i < m; i++) { // Output in col-major order
      for(int j = 0; j < n; j++) {
	SUtils::add(res, this->components[j * m + i]->getID()); // Think this is right?
      }
    }
  }

  template<int n, int m>
  void ConstructMatrix<n, m>::ensure_type_defined(std::vector<uint32_t>& res,
						  std::vector<SDeclarationState*>& declaration_states) {
    SMat<n, m>::ensure_defined(res, declaration_states);
    for(int i = 0; i < n * m; i++) {
      if(this->is_constant[i]) {
	this->components[i]->define(res);
      }
    }
  }

};

#endif // __SPURV_NODES_IMPL
