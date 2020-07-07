#ifndef __SPURV_NODES_IMPL
#define __SPURV_NODES_IMPL

#include "values.hpp"
#include "variable_registry.hpp"
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
  void SValue<tt>::print_nodes_post_order(std::ostream& str) const {
    str << this->id << std::endl;
  }

  template<typename tt>
  SValue<tt>::SValue() { this->id = SUtils::getNewID(); this->ref_count = 0; this->defined = false; }
  
  template<typename tt>
  void SValue<tt>::ensure_defined(std::vector<uint32_t>& res)  {
    if(this->defined) {
      return;
    }

    this->define(res);
    this->defined = true;
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
    if constexpr(std::is_same<tt, float>::value ||
		 std::is_same<tt, double>::value) {
	if(SConstantRegistry::isRegisteredFloat(sizeof(tt) * 8, val)) {
	  this->id = SConstantRegistry::getIDFloat(sizeof(tt) * 8, val);
	} else {
	  SConstantRegistry::registerFloat(sizeof(tt) * 8, val, this->id);
	}

      } else {
      int i = std::numeric_limits<tt>::is_signed ? 1 : 0;
      if(SConstantRegistry::isRegisteredInt(sizeof(tt) * 8, i, val)) {
	this->id = SConstantRegistry::getIDInteger(sizeof(tt) * 8, i, val);
      } else {
	SConstantRegistry::registerInt(sizeof(tt) * 8, i, val, this->id);
      }

    }
  }


  template<typename tt>
  void Constant<tt>::define(std::vector<uint32_t>& res) {

    // SConstantRegistry::ensureDefinedConstant<tt>(this->value, this->id,
    // res);
  }
  
  template<typename tt>
  void Constant<tt>::ensure_type_defined(std::vector<uint32_t>& res,
					 std::vector<SDeclarationState*>& states) {
    MapSType<tt>::type::ensure_defined(res,
				       states);

    SConstantRegistry::ensureDefinedConstant<tt>(this->value, this->id,
						 res);
  }


  /*
   * SPointerVar member functions
   */

  template<typename tt, SStorageClass storage>
  SPointerVar<tt, storage>::SPointerVar(int pointer_id) : pointer_id(pointer_id) { }

  template<typename tt, SStorageClass storage>
  void SPointerVar<tt, storage>::define(std::vector<uint32_t>& res) {
    // This default implementation assumes the pointer itself has already
    // been defined somewhere else (which is the case for input vars, builtins),
    // and only loads from the pointer
    
    // OpLoad
    SUtils::add(res, (4 << 16) | 61);
    SUtils::add(res, tt::getID());
    SUtils::add(res, this->id);
    SUtils::add(res, this->pointer_id);
  }

  template<typename tt, SStorageClass storage>
  void SPointerVar<tt, storage>::ensure_type_defined(std::vector<uint32_t>& res,
						std::vector<SDeclarationState*>& declaration_states) {
    tt::ensure_defined(res, declaration_states);
    SPointer<storage, tt>::ensure_defined(res, declaration_states);
  }
  
  
  /*
   * SUniformVar member functions
   */
  
  template<typename tt>
  SUniformVar<tt>::SUniformVar(int s, int b, int m,
			       int pointer_id, int parent_struct_id) : SPointerVar<tt, STORAGE_UNIFORM>(pointer_id) {
    this->set_no = s;
    this->bind_no = b;
    this->member_no = m;
    this->parent_struct_id = parent_struct_id;

    this->member_index = SUtils::allocate<Constant<int> >(this->member_no);
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

    this->member_index->ensure_type_defined(res, declaration_states);
  }

  
  /*
   * InputVar member functions
   */
  
  template<typename tt>
  InputVar<tt>::InputVar(int n, int pointer_id) : SPointerVar<tt, STORAGE_INPUT>(pointer_id) {
    this->input_no = n;
  }
  
  /*
   * ConstructMatrix member functions
   */

  template<int n, int m>
  template<typename... Types>
  ConstructMatrix<n, m>::ConstructMatrix(Types&&... args) {
    static_assert(sizeof...(args) == n * m,
		  "Number of arguments to matrix construction does not match number of components in matrix");
    insertComponents(0, args...);
  }

  template<int n, int m>
  template<typename t1, typename... trest>
  void ConstructMatrix<n, m>::insertComponents(int u, t1&& first, trest&&... rest) {

    this->components[u] = &SValueWrapper::unwrap_to<t1, SFloat<32> >(first);
    if constexpr(sizeof...(rest) > 0) {
	insertComponents(u + 1, rest...);
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
    // A bit hacky but oh well
    SMat<n, m>::ensure_defined(res, declaration_states);
    for(int i = 0; i < n * m; i++) {
      this->components[i]->ensure_type_defined(res,
					       declaration_states);
    }
  }


  /*
   * SelectConstruct member functions
   */

  template<typename tt>
  SelectConstruct<tt>::SelectConstruct(SValue<SBool>& cond,
				   SValue<tt>& true_val,
				   SValue<tt>& false_val) {
    this->condition = &cond;
    this->val_true = &true_val;
    this->val_false = &false_val;
  }

  template<typename tt>
  void SelectConstruct<tt>::define(std::vector<uint32_t>& res) {

    this->condition->ensure_defined(res);
    
    uint32_t final_label = SUtils::getNewID();
    uint32_t true_label = SUtils::getNewID();
    uint32_t false_label = SUtils::getNewID();
    
    // OpSelectionMerge <label> <selection control>
    SUtils::add(res, (3 << 16) | 247);
    SUtils::add(res, final_label);
    SUtils::add(res, 0); // None

    // OpBranchConditional <condition> <true_branch> <false_branch>
    SUtils::add(res, (4 << 16) | 250);
    SUtils::add(res, this->condition->getID());
    SUtils::add(res, true_label);
    SUtils::add(res, false_label);

    // OpLabel <label>
    SUtils::add(res, (2 << 16) | 248);
    SUtils::add(res, true_label);

    // Optimally, we can define this value inside this branch only
    this->val_true->ensure_defined(res);

    // OpBranch <final_label>
    SUtils::add(res, (2 << 16) | 249);
    SUtils::add(res, final_label);
    
    // OpLabel <false_label>
    SUtils::add(res, (2 << 16) | 248);
    SUtils::add(res, false_label);

    this->val_false->ensure_defined(res);

    // OpBranch <final_label>
    SUtils::add(res, (2 << 16) | 249);
    SUtils::add(res, final_label);

    // OpLabel <final_label>
    SUtils::add(res, (2 << 16) | 248);
    SUtils::add(res, final_label);

    // OpPhi <result_type> <result_id> <variable_true> <parent_true> <variable_false> <parent_false>
    SUtils::add(res, (7 << 16) | 245);
    SUtils::add(res, tt::getID());
    SUtils::add(res, this->getID());
    SUtils::add(res, this->val_true->getID());
    SUtils::add(res, true_label);
    SUtils::add(res, this->val_false->getID());
    SUtils::add(res, false_label);
    
  }

  template<typename tt>
  void SelectConstruct<tt>::ensure_type_defined(std::vector<uint32_t>& res,
						std::vector<SDeclarationState*>& declaration_states) {
    tt::ensure_defined(res, declaration_states);
    SPointer<STORAGE_FUNCTION, tt>::ensure_defined(res, declaration_states);
    this->condition->ensure_type_defined(res, declaration_states);
    this->val_true->ensure_type_defined(res, declaration_states);
    this->val_false->ensure_type_defined(res, declaration_states);
  }

  // Shorthand to make the below code somewhat readable
  template<typename S, typename T>
  struct uuap {
    using type = typename SValueWrapper::unambiguous_unwrapped_allow_primitives<typename std::remove_reference<S>::type,
										typename std::remove_reference<T>::type>::type;
  };
  
  template<typename t1, typename t2, typename t3>
  requires RequireUnambiguousUnwrappable<t2, t3> && SValueWrapper::does_wrap<typename std::remove_reference<t1>::type, SBool>::value
  SelectConstruct<typename uuap<t2, t3>::type>& select(t1&& cond,
						       t2&& true_val,
						       t3&& false_val) {
    using tt1 = typename std::remove_reference<t1>::type;

    using unwrapped_res_type = typename uuap<t2, t3>::type;

    static_assert(SValueWrapper::does_wrap<tt1, SBool>::value);

    SelectConstruct<unwrapped_res_type>* v =
      SUtils::allocate<SelectConstruct<unwrapped_res_type> >(SValueWrapper::unwrap_to<t1, SBool>(cond),
							     SValueWrapper::unwrap_to<t2, unwrapped_res_type>(true_val),
							     SValueWrapper::unwrap_to<t3, unwrapped_res_type>(false_val));
    return *v;
  }
  
};

#endif // __SPURV_NODES_IMPL
