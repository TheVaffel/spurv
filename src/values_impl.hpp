#ifndef __SPURV_NODES_IMPL
#define __SPURV_NODES_IMPL

#include "values.hpp"
#include "constant_registry_impl.hpp"
#include "event_registry_impl.hpp"

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

  template<typename tt>
  void SValue<tt>::ensure_type_decorated(std::vector<uint32_t>& bin, std::vector<bool*>& decoration_states) {
    tt::ensure_decorated(bin, decoration_states);
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
    SPointer<storage, tt>::ensure_defined(res, declaration_states);
  }

  template<typename tt, SStorageClass storage>
  void SPointerVar<tt, storage>::ensure_type_decorated(std::vector<uint32_t>& res,
						       std::vector<bool*>& decoration_states) {
    SPointer<storage, tt>::ensure_decorated(res, decoration_states);
  }
  
  /*
   * SUniformVar member functions
   */
  
  template<SStorageClass storage, typename tt>
  SUniformVar<storage, tt>::SUniformVar(int s, int b, int m,
			       int pointer_id, int parent_struct_id) : SPointerVar<tt, storage>(pointer_id) {
    this->set_no = s;
    this->bind_no = b;
    this->member_no = m;
    this->parent_struct_id = parent_struct_id;

    this->member_index = SUtils::allocate<Constant<int> >(this->member_no);
  }

  template<SStorageClass storage, typename tt>
  void SUniformVar<storage, tt>::define(std::vector<uint32_t>& res) {

    // If the variable is an array, we don't load from it, only return pointer
    if(tt::getKind() == STypeKind::KIND_ARR ||
       tt::getKind() == STypeKind::KIND_RUN_ARR) {
      // OpAccessChain
      SUtils::add(res, (5 << 16) | 65);
      SUtils::add(res, SPointer<storage, tt>::getID());
      SUtils::add(res, this->id);
      SUtils::add(res, this->parent_struct_id);
      SUtils::add(res, SConstantRegistry::getIDInteger(32, 1, this->member_no));
    } else {
      int individual_pointer_id = SUtils::getNewID();
    
      // OpAccessChain
      SUtils::add(res, (5 << 16) | 65);
      SUtils::add(res, SPointer<storage, tt>::getID());
      SUtils::add(res, individual_pointer_id);
      SUtils::add(res, this->parent_struct_id);
      SUtils::add(res, SConstantRegistry::getIDInteger(32, 1, this->member_no)); 
    
      // OpLoad
      SUtils::add(res, (4 << 16) | 61);
      SUtils::add(res, tt::getID());
      SUtils::add(res, this->id);
      SUtils::add(res, individual_pointer_id);
    }
  }

  template<SStorageClass storage, typename tt>
  void SUniformVar<storage, tt>::ensure_type_defined(std::vector<uint32_t>& res,
					    std::vector<SDeclarationState*>& declaration_states) {

    SPointer<storage, tt>::ensure_defined(res, declaration_states);

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
   * SLocal member functions
   */

  template<typename tt>
  SLocal<tt>::SLocal() {
    SVariableRegistry::add_variable(this);
  }

  template<typename tt>
  void SLocal<tt>::ensure_type_defined(std::vector<uint32_t>& res,
				       std::vector<SDeclarationState*>& declaration_states) {
    SPointer<STORAGE_FUNCTION, tt>::ensure_defined(res,
						   declaration_states);
  }

  template<typename tt>
  void SLocal<tt>::define(std::vector<uint32_t>& res) {
    // OpVariable 
    SUtils::add(res, (4 << 16) | 59);
    SUtils::add(res, SPointer<STORAGE_FUNCTION, tt>::getID());
    SUtils::add(res, this->id);
    SUtils::add(res, STORAGE_FUNCTION);
  }

  template<typename tt>
  SLoadedVal<tt>& SLocal<tt>::load() {

    
    SLoadEvent<tt> *ev = SEventRegistry::addLoad<tt>(this->id);
    SLoadedVal<tt>* val = SUtils::allocate<SLoadedVal<tt> >(this, ev);
    ev->val_p = val;
    // REMEMBER TO REGISTER LOAD EVENT AND IMPLEMENT ORDER ENFORCEMENT!

    
    return *val;
  }

  template<typename tt>
  void SLocal<tt>::store(SValue<tt>& val) {
    SStoreEvent<tt> *ev = SEventRegistry::addStore<tt>(this);
    ev->val_p = &val;
  }


  /*
   * SLoadedVal member functions
   */

  template<typename tt>
  SLoadedVal<tt>::SLoadedVal(SLocal<tt>* pointer, SLoadEvent<tt>* ev) {
    this->pointer = pointer;
    this->event = ev;
  }

  template<typename tt>
  void SLoadedVal<tt>::ensure_type_defined(std::vector<uint32_t>& res,
					   std::vector<SDeclarationState*>& declaration_states) {
    this->pointer->ensure_type_defined(res, declaration_states);
    
    tt::ensure_defined(res, declaration_states);
  }

  template<typename tt>
  void SLoadedVal<tt>::define(std::vector<uint32_t>& res) {
    this->pointer->ensure_defined(res);

    // OpLoad
    SUtils::add(res, (4 << 16) | 61);
    SUtils::add(res, tt::getID());
    SUtils::add(res, this->id);
    SUtils::add(res, this->pointer->getID());
  }

  
  /*
   * SCustomVal member functions
   */

  template<typename tt>
  void SCustomVal<tt>::define(std::vector<uint32_t>& res) {
    // We do nothing and cross fingers the implementor knows what's up
  }
  
  
  /*
   * ConstructMatrix member functions
   */

  template<int n, int m, typename inner>
  bool ConstructMatrix<n, m, inner>::using_columns() {
    return (n > 1 && m > 1 && this->components.size() == m);
  }

  template<int n, int m, typename inner>
  template<typename... Types>
  ConstructMatrix<n, m, inner>::ConstructMatrix(Types&&... args) {
    constexpr int num = SUtils::sum_num_elements(args...);
    static_assert(num  ==  n * m, // sizeof...(args) == n * m,
    		  "Number of arguments to matrix construction does not match number of components in matrix, nums were ");

    // Only vectors can be constructed with concatenating other vectors
    // Matrices, on the other hand, must be given every element explicitly, or constructed from columns only
    static_assert((m == 1 || n == 1) || SUtils::has_only_1_comps(args...) || SUtils::has_only_n_comps(n, args...),
		  "Matrices must be constructed with every element explicitly, or from columns exclusively");

    if constexpr (m == 1 || n == 1 || SUtils::has_only_1_comps(args...)) {
	this->components.resize(n * m);
	insertComponents(0, args...);
      } else {
      this->components.resize(m);
      insertColumns(0, args...);
    }
  }

  template<int n, int m, typename inner>
  template<typename t1, typename... trest>
  void ConstructMatrix<n, m, inner>::insertComponents(int u, t1&& first, trest&&... rest) {
    int plus = 1;
    if constexpr(is_spurv_value<t1>::value) {
	using in_t = typename std::remove_reference<t1>::type::type;
	if constexpr(is_spurv_mat_type<in_t>::value) {
	    for(int i = 0; i < in_t::getArg0() * in_t::getArg1(); i++) {
	      this->components[u + i] = (void*)&SValueWrapper::unwrap_to<SValue<inner>, inner>(first[i]);
	    }
	    plus = in_t::getArg0() * in_t::getArg1();
	  } else {
	  this->components[u] = (void*)&SValueWrapper::unwrap_to<t1, inner>(first);
	}
      } else {
    
      this->components[u] = (void*)&SValueWrapper::unwrap_to<t1, inner>(first);
    }
    if constexpr(sizeof...(rest) > 0) {
	insertComponents(u + plus, rest...);
      }
  }

  template<int n, int m, typename inner>
  template<typename t1, typename... trest>
  void ConstructMatrix<n, m, inner>::insertColumns(int u, t1&& first, trest&&... rest) {
    this->components[u] = (void*)&SValueWrapper::unwrap_to<t1, SMat<n, 1, inner> >(first);

    if constexpr(sizeof...(rest) > 0) {
	insertColumns(u + 1, rest...);
      }
  }

  template<int n, int m, typename inner>
  void ConstructMatrix<n, m, inner>::define(std::vector<uint32_t>& res) {
    for(unsigned int i = 0; i < this->components.size(); i++) {
      ((SValue<inner>*)this->components[i])->ensure_defined(res);
    }

    if (m == 1 || n == 1) {

      // OpCompositeConstruct <result type> <result id> <components...>
      SUtils::add(res, ((3 + n * m) << 16) | 80);
      SUtils::add(res, SMat<n, m, inner>::getID());
      SUtils::add(res, this->id);
      
      for(unsigned int i = 0; i < this->components.size(); i++) {
	SUtils::add(res, ((SValue<inner>*)this->components[i])->getID());
      }
    } else if(this->components.size() == m) {
      // This is a matrix, and components contain columns

      // OpCompositeContsruct <matrix_type> <result_id> <components...>
      SUtils::add(res, ((3 + m) << 16) | 80);
      SUtils::add(res, SMat<n, m, inner>::getID());
      SUtils::add(res, this->id);

      for(int i = 0; i < m; i++) {
	SUtils::add(res, ((SValue<SMat<n, 1, inner> >*)this->components[i])->getID());
      }
    } else {
      std::vector<int> col_ids(m);
      for(int i = 0; i < m; i++) { // Output in col-major order
	col_ids[i] = SUtils::getNewID();
	
	// OpCompositeConstruct <vector type> <result id> <components...>
	SUtils::add(res, ((3 + n) << 16) | 80);
	SUtils::add(res, SMat<n, 1, inner>::getID());
	SUtils::add(res, col_ids[i]);
	
	for(int j = 0; j < n; j++) {
	  
	  SUtils::add(res, ((SValue<inner>*)this->components[j * m + i])->getID()); // Think this is right?
	}
      }

      // OpCompositeConstruct <result type> <result id> <components...>
      SUtils::add(res, ((3 + m) << 16) | 80);
      SUtils::add(res, SMat<n, m, inner>::getID());
      SUtils::add(res, this->id);
      
      for(int i = 0; i < m; i++) {
	SUtils::add(res, col_ids[i]);
      }
    }
  }

  template<int n, int m, typename inner>
  void ConstructMatrix<n, m, inner>::ensure_type_defined(std::vector<uint32_t>& res,
						  std::vector<SDeclarationState*>& declaration_states) {
    // A bit hacky but oh well
    SMat<n, m, inner>::ensure_defined(res, declaration_states);
    for(unsigned int i = 0; i < this->components.size(); i++) {
      if(this->using_columns()) {
	((SValue<SMat<n, 1, inner> >*)this->components[i])->ensure_type_defined(res,
										declaration_states);
      } else {
	((SValue<inner>*)this->components[i])->ensure_type_defined(res,
								   declaration_states);
      }
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

  template<typename tt>
  void SelectConstruct<tt>::ensure_type_decorated(std::vector<uint32_t>& res,
						  std::vector<bool*>& decoration_states) {
    SPointer<STORAGE_FUNCTION, tt>::ensure_decorated(res, decoration_states);
    this->condition->ensure_type_decorated(res, decoration_states);
    this->val_true->ensure_type_decorated(res, decoration_states);
    this->val_false->ensure_type_decorated(res, decoration_states);
  }

  
  // Shorthand
  template<typename t1, typename t2>
  struct ruu {
    using type = typename SValueWrapper::unambiguous_unwrapped_allow_primitives<t1, t2>::type;
  };
  
  template<typename t1, typename t2, typename t3>
  SelectConstruct<typename ruu<t2, t3>::type>& select(t1&& cond,
						      t2&& true_val,
						      t3&& false_val) {
    using tt1 = typename std::remove_reference<t1>::type;

    using unwrapped_res_type = typename ruu<t2, t3>::type;

    static_assert(SValueWrapper::does_wrap<tt1, SBool>::value);

    SelectConstruct<unwrapped_res_type>* v =
      SUtils::allocate<SelectConstruct<unwrapped_res_type> >(SValueWrapper::unwrap_to<t1, SBool>(cond),
							     SValueWrapper::unwrap_to<t2, unwrapped_res_type>(true_val),
							     SValueWrapper::unwrap_to<t3, unwrapped_res_type>(false_val));
    return *v;
  }
  
};

#endif // __SPURV_NODES_IMPL
