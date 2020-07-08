#ifndef __SPURV_EXPRESSIONS_IMPL
#define __SPURV_EXPRESSIONS_IMPL

#include "declarations.hpp"
#include "types.hpp"

namespace spurv {

  /*
   * Utility functions
   */

  template<typename tt, SExprOp op, typename tt2, typename tt3>
  SExpr<tt, op, tt2, tt3>::SExpr() {
    this->v1 = nullptr;
    this->v2 = nullptr;
  }

  template<typename tt, SExprOp op, typename tt2, typename tt3>
  void SExpr<tt, op, tt2, tt3>::register_left_node(SValue<tt2>& node) {
    v1 = &node;
  };
  
  template<typename tt, SExprOp op, typename tt2, typename tt3>
  void SExpr<tt, op, tt2, tt3>::register_right_node(SValue<tt3>& node) {
    v2 = &node;
  }

  
  /*
   * Overrides of inherited member functions
   */
  
  template<typename tt, SExprOp op, typename tt2, typename tt3>
  void SExpr<tt, op, tt2, tt3>::print_nodes_post_order(std::ostream& str) const {
    v1->print_nodes_post_order(str);
      
    if constexpr(op != EXPR_NEGATIVE) {
	v2->print_nodes_post_order(str);
      }
    str << this->getID() << std::endl;
  }

  
  template<typename tt, SExprOp op, typename tt2, typename tt3>
  void SExpr<tt, op, tt2, tt3>::ensure_type_defined(std::vector<uint32_t>& res, std::vector<SDeclarationState*>& declaration_states) {
    if(this->v1) {
      this->v1->ensure_type_defined(res, declaration_states);
    }

    if(this->v2) {
      this->v2->ensure_type_defined(res, declaration_states);
    }

    tt::ensure_defined(res, declaration_states);
  }

  /*
   * Utility functions for the expression define() function
   */
  
  // Returns component type (which is the type itself if scalar)
  static const DSType& get_comp_type(const DSType& ds) {
    if(ds.kind == STypeKind::KIND_MAT || ds.kind == STypeKind::KIND_ARR) {
      return *ds.inner_types;
    }

    return ds;
  }

  static int num_components(const DSType& ds) {
    if(ds.kind == STypeKind::KIND_MAT) {
      return ds.a0 * ds.a1;
    } else if(ds.kind == STypeKind::KIND_BOOL ||
	      ds.kind == STypeKind::KIND_FLOAT ||
	      ds.kind == STypeKind::KIND_INT) {
      return 1;
    } else {
      printf("[spurv] num_components cannot be called for this kind\n");
      exit(-1);
    }
  }

  /*
   * Output the expression to the binary
   */
  
  template<typename tt, SExprOp op, typename tt2, typename tt3>
  void SExpr<tt, op, tt2, tt3>::define(std::vector<uint32_t>& res) {
    if(this->v1) {
      this->v1->ensure_defined(res);
    }

    if(this->v2) {
      this->v2->ensure_defined(res);
    }

    DSType d1, d2, d3;
    tt::getDSType(&d1);
    tt2::getDSType(&d2);
    tt3::getDSType(&d3);

    const DSType& d1_comp = get_comp_type(d1);
    const DSType& d2_comp = get_comp_type(d2);
    
    int opcode = 0;
    if(d1.kind == STypeKind::KIND_BOOL) {
      if (d2.kind == STypeKind::KIND_INT &&
	  d3.kind == STypeKind::KIND_INT) {
	
	if constexpr(op == EXPR_EQUAL) {
	    opcode = 170;
	  } else if(op == EXPR_NOTEQUAL) {
	  opcode = 171;
	} else {
	  if(d2.a1 == 0 && d3.a1 == 0) {
	    if constexpr(op == EXPR_LESSTHAN) {
	      opcode = 176;
	      } else if(op == EXPR_GREATERTHAN) {
	      opcode = 172;
	    } else if(op == EXPR_LESSOREQUAL) {
	      opcode = 178;
	    } else if(op == EXPR_GREATEROREQUAL) {
	      opcode = 174;
	    } else {
	      printf("Comparison operator %d not defined for unsigned int\n",
		     (int)op);
	    }
	  } else if(d2.a1 == 1 && d3.a1 == 1) {
	    if constexpr(op == EXPR_LESSTHAN) {
		opcode = 177;
	      } else if (op == EXPR_GREATERTHAN) {
	      opcode = 173;
	    } else if(op == EXPR_LESSOREQUAL) {
	      opcode = 179;
	    } else if(op == EXPR_GREATEROREQUAL) {
	      opcode = 175;
	    } else {
	      printf("Comparison operator %d not defined for signed int\n",
		     (int)op);
	    }
	  }
	}
      } else if(d2.kind == STypeKind::KIND_FLOAT &&
		d3.kind == STypeKind::KIND_FLOAT) {
	// Using OpFOrd as opposed to OpFUnord
	// Not sure right now how the difference would turn out
	if constexpr(op == EXPR_EQUAL) {
	    opcode = 180;
	  } else if(op == EXPR_NOTEQUAL) {
	  opcode = 182;
	} else if(op == EXPR_LESSTHAN) {
	  opcode = 184;
	} else if(op == EXPR_GREATERTHAN) {
	  opcode = 186;
	} else if(op == EXPR_LESSOREQUAL) {
	  opcode = 188;
	} else if(op == EXPR_GREATEROREQUAL) {
	  opcode = 190;
	} else {
	  printf("Comparison operator %d not defined for floats\n", (int)op);
	}
      } else {
	printf("Comparison operator %d not defined for kinds %d and %d\n",
	       (int)op, (int)d2.kind, (int)d3.kind);
      }

      SUtils::add(res, (5 << 16) | opcode);
      SUtils::add(res, tt::getID());
      SUtils::add(res, this->getID());
      SUtils::add(res, this->v1->getID());
      SUtils::add(res, this->v2->getID());
      
    } else if(d1 == d2 && d2 == d3) {
      if (d1_comp.kind == STypeKind::KIND_INT) {
	if constexpr(op == EXPR_ADDITION) {
	    opcode = 128;
	  } else if constexpr(op == EXPR_SUBTRACTION) {
	    opcode = 130;
	  } else if constexpr(op == EXPR_MULTIPLICATION) {
	    opcode = 132;
	  } else if constexpr(op == EXPR_DIVISION) {
	    opcode = (d1.a1 == 0 ? 134 : 135); // OpUDiv / OpSDiv
	  } else if constexpr(op == EXPR_REM) {
	    opcode = (d1.a1 == 0 ? 137 : 138); // OpUMod (!) / OpSRem
	  } else if constexpr(op == EXPR_MOD) {
	    opcode = (d1.a1 == 0 ? 137 : 139); // OpUMod / OpSMod
	  } else {
	  printf("Tried to output expression with integer type and operation = %d\n", (int)op);
	  exit(-1);
	}
      } else if (d1_comp.kind == STypeKind::KIND_FLOAT) {
	if constexpr(op == EXPR_ADDITION) {
	    opcode = 129;
	  } else if constexpr(op == EXPR_SUBTRACTION) {
	    opcode = 131;
	  } else if constexpr(op == EXPR_MULTIPLICATION) {
	    opcode = 133;
	  } else if constexpr(op == EXPR_DIVISION) {
	    opcode = 136;
	  } else if constexpr(op == EXPR_REM) {
	    opcode = 140; // OpFRem
	  } else if constexpr(op == EXPR_MOD) {
	    opcode = 141; // OpFMod
	  } else {
	  printf("Tried to output expression with float type and operation = %d\n", (int)op);
	  exit(-1);
	}
      } else {
	printf("Tried to output expression where types were equal, but not int or float (feature yet to be implemented, probably)\n");
	exit(-1);
      }
      
      SUtils::add(res, (5 << 16) | opcode);
      SUtils::add(res, tt::getID());
      SUtils::add(res, this->getID());
      SUtils::add(res, this->v1->getID());
      SUtils::add(res, this->v2->getID());

    } else {
      if constexpr(op == EXPR_MULTIPLICATION) {
	  if (d2.kind == STypeKind::KIND_MAT && d3.kind == STypeKind::KIND_FLOAT) {
	    // Differ between vector and matrix
	    if (d2.a1 == 1) {
	      SUtils::add(res, (5 << 16) | 142);
	    } else {
	      SUtils::add(res, (5 << 16) | 143);
	    }
	    
	    SUtils::add(res, tt::getID());
	    SUtils::add(res, this->getID());
	    SUtils::add(res, this->v1->getID());
	    SUtils::add(res, this->v2->getID());
	  } else if(d2.kind == STypeKind::KIND_FLOAT && d3.kind == STypeKind::KIND_MAT) {
	    if (d3.a1 == 1) {
	      SUtils::add(res, (5 << 16) | 142);
	    } else {
	      SUtils::add(res, (5 << 16) | 143);
	    }

	    SUtils::add(res, tt::getID());
	    SUtils::add(res, this->getID());
	    SUtils::add(res, this->v2->getID());
	    SUtils::add(res, this->v1->getID());
	  } else {
	    printf("Tried to use EXPR_MULTIPLICATION for something else than float times mat or float times float\n");
	    exit(-1);
	  }
	} else if constexpr(op == EXPR_DOT) {
	  if (d2.a1 == 1 && d3.a1 == 1) {
	    if(d1.kind != STypeKind::KIND_FLOAT) {
	      printf("Operands should have made scalar, but didn't\n");
	      exit(-1);
	    }
	    SUtils::add(res, (5 << 16) | 148);
	    SUtils::add(res, tt::getID());
	    SUtils::add(res, this->getID());
	    SUtils::add(res, this->v1->getID());
	    SUtils::add(res, this->v2->getID());
	  } else if (d3.a1 == 1) {
	    SUtils::add(res, (5 << 16) | 145); // Matrix times vector
	    SUtils::add(res, tt::getID());
	    SUtils::add(res, this->getID());
	    SUtils::add(res, this->v1->getID());
	    SUtils::add(res, this->v2->getID());
	  } else if (d2.a1 == 1) {
	    printf("[spurv] Error: Vector - matrix multiplication not yet implemented!\n");
	    exit(-1);
	  } else {
	    SUtils::add(res, (5 << 16) | 146);
	    SUtils::add(res, tt::getID());
	    SUtils::add(res, this->getID());
	    SUtils::add(res, this->v1->getID());
	    SUtils::add(res, this->v2->getID());
	  }
	} else if constexpr(op == EXPR_LOOKUP) {
	  if(d2.kind == STypeKind::KIND_TEXTURE) {
	    // OpImageSampleExplicitLod
	    SUtils::add(res, (7 << 16) | 88);
	    SUtils::add(res, vec4_s::getID());
	    SUtils::add(res, this->getID());
	    SUtils::add(res, this->v1->getID());
	    SUtils::add(res, this->v2->getID());
	    SUtils::add(res, 2); // LoD
	    SUtils::add(res, SConstantRegistry::getIDFloat(32, 0.0f));
	  } else if(d2.kind == STypeKind::KIND_MAT) {
	    
	    if (d2.a1 == 1) {
	      // OpVectorExtractDynamic <result_type> <result_id> <vector> <index>
	      SUtils::add(res, (5 << 16) | 77);
	      SUtils::add(res, float_s::getID());
	      SUtils::add(res, this->getID());
	      SUtils::add(res, this->v1->getID());
	      SUtils::add(res, this->v2->getID());
	      
	    } else {
	      // OpCompositeExtract <result_type> <result_id> <matrix> <index>
	      SUtils::add(res, (5 << 16) | 81);
	      SUtils::add(res, SMat<tt2::getArg0(), 1, typename tt2::firstInnerType>::getID());
	      SUtils::add(res, this->getID());
	      SUtils::add(res, this->v1->getID());
	      SUtils::add(res, this->v2->getID());
	    
	    }
	  } else {
	    printf("Expression lookup operation not yet implemented\n");
	    exit(-1);
	  }
	} else if constexpr(op == EXPR_CAST) {
	  if(d1 == d2) {
	    printf("[spurv] Error: Trying to convert a value to the type it already is");
	    exit(-1);
	  }

	  assert(num_components(d1) == num_components(d2));
	  
	  if(d1_comp.kind == STypeKind::KIND_FLOAT) {
	    if(d2_comp.kind == STypeKind::KIND_FLOAT) {
	      // OpFConvert
	      SUtils::add(res, (4 << 16) | 115);
	      SUtils::add(res, tt::getID());
	      SUtils::add(res, this->getID());
	      SUtils::add(res, this->v1->getID());
	    } else if(d2_comp.kind == STypeKind::KIND_INT) {
	      // OpConvertSToF / OpConvertUToF
	      // (Yes, think this is right, although the order is S-U, opposite of convention)
	      int opcode = d2.a1 == 1 ? 111 : 112;
	      
	      SUtils::add(res, (4 << 16) | opcode);
	      SUtils::add(res, tt::getID());
	      SUtils::add(res, this->getID());
	      SUtils::add(res, this->v1->getID());
	    } else {
	      printf("[spurv] Error: Trying to convert to float from unsupported value\n");
	    }
	  } else if(d1_comp.kind == STypeKind::KIND_INT) {
	    if(d2_comp.kind == STypeKind::KIND_INT) {
	      if (d1_comp.a0 != d2_comp.a0 && d1_comp.a1 != d2_comp.a1) {
		// Both bit width and signedness differs, must do two operations
		int temp_id = SUtils::getNewID();

		// Not sure what's the best order, I choose to bitcast first
		
		// OpBitCast
		SUtils::add(res, (4 << 16) | 124);
		SUtils::add(res, tt::getID());
		SUtils::add(res, temp_id);
		SUtils::add(res, this->v1->getID());

		// OpSConvert / OpUConvert
		int opcode = d2_comp.a1 == 1 ? 114 : 113;
		SUtils::add(res, (4 << 16) | opcode);
		SUtils::add(res, tt::getID());
		SUtils::add(res, this->getID());
		SUtils::add(res, temp_id);
	      } else if(d1_comp.a0 == d2_comp.a0) {
		// Width is equal, only do signedness convertion
		// OpBitCast
		SUtils::add(res, (4 << 16) | 124);
		SUtils::add(res, tt::getID());
		SUtils::add(res, this->getID());
		SUtils::add(res, this->v1->getID());
	      } else {
		// Since we know not everything is equal, width must be different
		// OpSConvert / OpUConvert
		int opcode = d2_comp.a1 == 1 ? 114 : 113;
		SUtils::add(res, (4 << 16) | opcode);
		SUtils::add(res, tt::getID());
		SUtils::add(res, this->getID());
		SUtils::add(res, this->v1->getID());
	      }
	    }
	  }
	} else {
	printf("Expression operation not yet implemented\n");
	exit(-1);
      }
    }
  }


  /*
   * Utility type checker
   */
  
  template<typename tt1, typename tt2>
  struct multiplication_res_type {
    // NB: Doesn't cover all cases
    typedef typename std::conditional<std::is_same<tt2, float_s>::value, tt1, tt2>::type tmp0_type;
    typedef typename std::conditional<std::is_same<tt1, float_s>::value, tt2, tmp0_type >::type type;
  };

  
  /*
   * Operator functions
   */

  template<typename tt>
  SExpr<tt, EXPR_NEGATIVE, tt, void_s>& operator-(SValue<tt>& v1) {
    SExpr<tt, EXPR_NEGATIVE, tt, void_s>* ex = SUtils::allocate<SExpr<tt, EXPR_NEGATIVE, tt, void_s> >();
    ex->v1 = (&v1);
    ex->v2 = nullptr;
    return *ex;
  }
    

  // Just a shorthand (It is actually needed pretty badly for readability
  template<typename a, typename b>
  requires RequireOneSpurvValue<a, b>
  struct uwr { using type =
      typename SValueWrapper::unambiguous_unwrapped_require_spurv_type<a, b>::type;
  };
  
  // Additions
  template<typename in1_t, typename in2_t>
  SExpr<typename uwr<in1_t, in2_t>::type,
	EXPR_ADDITION,
	typename uwr<in1_t, in2_t>::type,
	typename uwr<in1_t, in2_t>::type>&
  operator+(in1_t&& in1, in2_t&& in2) {
    
    using tt = typename uwr<in1_t, in2_t>::type;

    SExpr<tt, EXPR_ADDITION, tt, tt>* ex = SUtils::allocate<SExpr<tt, EXPR_ADDITION, tt, tt> >();
    ex->register_left_node(SValueWrapper::unwrap_to<in1_t, tt>(in1));
    ex->register_right_node(SValueWrapper::unwrap_to<in2_t, tt>(in2));
    return *ex;
  }

  
  // Subtractions
  template<typename in1_t, typename in2_t>
  SExpr<typename uwr<in1_t, in2_t>::type,
	EXPR_SUBTRACTION,
	typename uwr<in1_t, in2_t>::type,
	typename uwr<in1_t, in2_t>::type>&
  operator-(in1_t&& in1, in2_t&& in2) {
    
    using tt = typename uwr<in1_t, in2_t>::type;

    
    SExpr<tt, EXPR_SUBTRACTION, tt, tt>* ex = SUtils::allocate<SExpr<tt, EXPR_SUBTRACTION, tt, tt> >();
    ex->register_left_node(SValueWrapper::unwrap_to<in1_t, tt>(in1));
    ex->register_right_node(SValueWrapper::unwrap_to<in2_t, tt>(in2));
    return *ex;
  }
  
  
  // Division
  template<typename in1_t, typename in2_t>
  SExpr<typename uwr<in1_t, in2_t>::type,
	EXPR_DIVISION,
	typename uwr<in1_t, in2_t>::type,
	typename uwr<in1_t, in2_t>::type>&
  operator/(in1_t&& in1, in2_t&& in2) {
    using tt = typename uwr<in1_t, in2_t>::type;

    SExpr<tt, EXPR_DIVISION, tt, tt>* ex = SUtils::allocate<SExpr<tt, EXPR_DIVISION, tt, tt> >();
    ex->register_left_node(SValueWrapper::unwrap_to<in1_t, tt>(in1));
    ex->register_right_node(SValueWrapper::unwrap_to<in2_t, tt>(in2));
    return *ex;
  }
  
  
  template<typename tt1, typename tt2>
  SExpr<typename multiplication_res_type<tt1, tt2>::type, EXPR_MULTIPLICATION, tt1, tt2>&
  operator*(SValue<tt1>& v1, SValue< tt2 >& v2) {
    // Don't mix integers and floats and don't allow matrix multiplication here
    static_assert((is_spurv_int_type<tt1>::value && is_spurv_int_type<tt2>::value) ||
		  (is_spurv_float_type<tt1>::value && is_spurv_float_type<tt2>::value) ||
		  (is_spurv_float_type<tt1>::value && is_spurv_mat_type<tt2>::value) ||
		  (is_spurv_mat_type<tt1>::value && is_spurv_float_type<tt2>::value));
    
    typedef typename multiplication_res_type<tt1, tt2>::type res_type;
    SExpr<res_type, EXPR_MULTIPLICATION, tt1, tt2>* ex = SUtils::allocate<SExpr<res_type, EXPR_MULTIPLICATION, tt1, tt2> >();
    ex->register_left_node(v1);
    ex->register_right_node(v2);
    return *ex;
  }

  template<int n, int m, int a, int b, typename inner>
  SExpr<SMat<n, b, inner>, EXPR_DOT, SMat<n, m, inner>, SMat<a, b, inner> >&
  operator*(SValue<SMat<n, m, inner> >& v1, SValue<SMat<a, b, inner> >& v2) {
    static_assert(m == a || (m == 1 && b == 1));
    SExpr<SMat<n, b, inner>, EXPR_DOT, SMat<n, m, inner>, SMat<a, b, inner> >* ex = SUtils::allocate<SExpr<SMat<n, b, inner>, EXPR_DOT, SMat<n, m, inner>, SMat<a, b, inner> > >();
    ex->register_left_node(v1);
    ex->register_right_node(v2);
    return *ex;
  }

  template<typename tt1>
  SExpr<tt1, EXPR_MULTIPLICATION, tt1, float_s>& operator*(SValue<tt1>& v1, const float& f) {
    static_assert(is_spurv_mat_type<tt1>::value || is_spurv_float_type<tt1>::value);
    
    Constant<float>* c = SUtils::allocate<Constant<float> >(f);
    SExpr<tt1, EXPR_MULTIPLICATION, tt1, float_s>* ex = SUtils::allocate< SExpr<tt1, EXPR_MULTIPLICATION, tt1, float_s> >();

    ex->register_left_node(v1);
    ex->register_right_node(*c);
    return *ex;
  }

  template<typename tt1>
  SExpr<tt1, EXPR_MULTIPLICATION, tt1, float_s>& operator*(const float& f, SValue<tt1>& v1) {
    return v1 * f;
  }

  // Mod and Rem
  template<typename in1_t, typename in2_t>
  SExpr<typename uwr<in1_t, in2_t>::type,
	EXPR_MOD,
	typename uwr<in1_t, in2_t>::type,
	typename uwr<in1_t, in2_t>::type>& mod(in1_t&& in1, in2_t&& in2) {
    using tt = typename uwr<in1_t, in2_t>::type;

    SExpr<tt, EXPR_MOD, tt, tt>* ex = SUtils::allocate<SExpr<tt, EXPR_MOD, tt, tt> >();
    ex->register_left_node(SValueWrapper::unwrap_to<in1_t, tt>(in1));
    ex->register_right_node(SValueWrapper::unwrap_to<in2_t, tt>(in2));
    return *ex;
  }

  template<typename in1_t, typename in2_t>
  SExpr<typename uwr<in1_t, in2_t>::type,
	EXPR_REM,
	typename uwr<in1_t, in2_t>::type,
	typename uwr<in1_t, in2_t>::type>& rem(in1_t&& in1, in2_t&& in2) {

    using tt = typename uwr<in1_t, in2_t>::type;

    SExpr<tt, EXPR_REM, tt, tt>* ex = SUtils::allocate<SExpr<tt, EXPR_REM, tt, tt> >();
    ex->register_left_node(SValueWrapper::unwrap_to<in1_t, tt>(in1));
    ex->register_right_node(SValueWrapper::unwrap_to<in2_t, tt>(in2));
    return *ex;
  }

  template<typename in1_t, typename in2_t>
  SExpr<typename uwr<in1_t, in2_t>::type,
	EXPR_MOD,
	typename uwr<in1_t, in2_t>::type,
	typename uwr<in1_t, in2_t>::type>& operator%(in1_t&& in1, in2_t&& in2) {
    return mod(in1, in2);
  }
  
  
  /*
   * Lookup functions
   */

  template<typename tt>
  SValue<typename lookup_result<tt>::type>&
  SValue<tt>::operator[](SValue<typename lookup_index<tt>::type >& index) {

    SExpr<typename lookup_result<tt>::type, EXPR_LOOKUP,
	  tt, typename lookup_index<tt>::type >* ex  =
      SUtils::allocate<SExpr<typename lookup_result<tt>::type, EXPR_LOOKUP,
			     tt, typename lookup_index<tt>::type > >(); 
    ex->register_left_node(*this);
    ex->register_right_node(index);
    return *ex;
  } 

  template<typename tt>
  SValue<typename lookup_result<tt>::type>& SValue<tt>::operator[](int index) {

    Constant<int>* c = SUtils::allocate<Constant<int> >(index);
    SExpr<typename lookup_result<tt>::type, EXPR_LOOKUP,
	  tt, typename lookup_index<tt>::type >* ex  =
      SUtils::allocate<SExpr<typename lookup_result<tt>::type, EXPR_LOOKUP,
			     tt, typename lookup_index<tt>::type > >(); 
    ex->register_left_node(*this);
    ex->register_right_node(*c);

    return *ex;
  }


  /*
   * Casting
   */

  template<typename t1, typename t2>
  SExpr<t1, EXPR_CAST, t2>& cast(SValue<t2>& val) {
    SExpr<t1, EXPR_CAST, t2>* ex = SUtils::allocate<SExpr<t1, EXPR_CAST, t2> >();
    
    ex->register_left_node(val);
    return *ex;
  }
  

  /*
   * Comparison operations
   */

  template<typename t1, typename t2, SExprOp op>
  static SExpr<SBool, op,
	       typename uwr<t1, t2>::type,
	       typename uwr<t1, t2>::type>& construct_comparison_val(t1&& v1, t2&& v2) {
    using tt = typename uwr<t1, t2>::type;
    static_assert(is_spurv_int_type<tt>::value || is_spurv_float_type<tt>::value,
		  "Comparison not yet defined for non-scalar types");

    SExpr<SBool, op, tt, tt>* ex =
      SUtils::allocate<SExpr<SBool, op, tt, tt> >();

    ex->register_left_node(v1);
    ex->register_right_node(v2);
    return *ex;
  }
  
  template<typename t1, typename t2>
  SExpr<SBool, EXPR_EQUAL,
	typename uwr<t1, t2>::type,
	typename uwr<t1, t2>::type>& operator==(t1&& v1, t2&& v2) {
    return construct_comparison_val<t1, t2, EXPR_EQUAL>(v1, v2);
  }

  template<typename t1, typename t2>
  SExpr<SBool, EXPR_NOTEQUAL,
	typename uwr<t1, t2>::type,
	typename uwr<t1, t2>::type>& operator!=(t1&& v1, t2&& v2) {
    return construct_comparison_val<t1, t2, EXPR_NOTEQUAL>(v1, v2);
  }

  template<typename t1, typename t2>
  SExpr<SBool, EXPR_LESSTHAN,
	typename uwr<t1, t2>::type,
	typename uwr<t1, t2>::type>& operator<(t1&& v1, t2&& v2) {
    return construct_comparison_val<t1, t2, EXPR_LESSTHAN>(v1, v2);
  }
  
  template<typename t1, typename t2>
  SExpr<SBool, EXPR_GREATERTHAN,
	typename uwr<t1, t2>::type,
	typename uwr<t1, t2>::type>& operator>(t1&& v1, t2&& v2) {
    return construct_comparison_val<t1, t2, EXPR_GREATERTHAN>(v1, v2);
  }
  
  template<typename t1, typename t2>
  SExpr<SBool, EXPR_LESSOREQUAL,
	typename uwr<t1, t2>::type,
	typename uwr<t1, t2>::type>& operator<=(t1&& v1, t2&& v2) {
    return construct_comparison_val<t1, t2, EXPR_LESSOREQUAL>(v1, v2);
  }
  
  template<typename t1, typename t2>
  SExpr<SBool, EXPR_GREATEROREQUAL,
	typename uwr<t1, t2>::type,
	typename uwr<t1, t2>::type>& operator>=(t1&& v1, t2&& v2) {
    return construct_comparison_val<t1, t2, EXPR_GREATEROREQUAL>(v1, v2);
  }
  
  
  /*
   * Special operator functions
   */
  
  template<typename tt>
  std::ostream& operator<<(std::ostream& out_stream, SValue<tt>& v) {
    v.print_nodes_post_order(out_stream);
    return out_stream;
  }

  
};

#endif // __SPURV_EXPRESSIONS_IMPL
