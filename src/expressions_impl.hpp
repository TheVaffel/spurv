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
  void SExpr<tt, op, tt2, tt3>::ensure_type_defined(std::vector<uint32_t>& res,
						    std::vector<SDeclarationState*>& declaration_states) {
    if(this->v1) {
      this->v1->ensure_type_defined(res, declaration_states);
    }

    if(this->v2) {
      this->v2->ensure_type_defined(res, declaration_states);
    }

    tt::ensure_defined(res, declaration_states);
  }

  template<typename tt, SExprOp op, typename tt2, typename tt3>
  void SExpr<tt, op, tt2, tt3>::ensure_type_decorated(std::vector<uint32_t>& res,
						      std::vector<bool*>& decoration_states) {
    if(this->v1) {
      this->v1->ensure_type_decorated(res, decoration_states);
    }

    if(this->v2) {
      this->v2->ensure_type_decorated(res, decoration_states);
    }

    tt::ensure_decorated(res, decoration_states);
  }
  

  /*
   * Utility functions for the expression define() function
   */
  
  // Returns component type (which is the type itself if scalar)
  static const DSType& get_comp_type(const DSType& ds) {
    if(ds.kind == STypeKind::KIND_MAT || ds.kind == STypeKind::KIND_ARR) {
      return ds.inner_types[0];
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
    const DSType& d3_comp = get_comp_type(d3);
    
    int opcode = 0;

    if(op == EXPR_NEGATIVE) {
      if(d1_comp.kind == STypeKind::KIND_INT &&
	 d1_comp.a1 == 1) {
	opcode = 126;
      } else if(d1_comp.kind == STypeKind::KIND_FLOAT) {
	opcode = 127;
      } else {
	printf("[spurv] Negating operator not defined for given type\n");
	exit(-1);
      }

      SUtils::add(res, (4 << 16) | opcode);
      SUtils::add(res, tt::getID());
      SUtils::add(res, this->getID());
      SUtils::add(res, this->v1->getID());
	
    } else if(d1.kind == STypeKind::KIND_BOOL) {
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
      
    } else if(d1 == d2 && d2 == d3 &&
	      !(tt2::getKind() == STypeKind::KIND_MAT && // Make sure not a matrix
		tt2::getArg1() > 1 && tt2::getArg0() > 1)) {
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
	  if (d2.kind == STypeKind::KIND_MAT && d3.kind == d2_comp.kind) {
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
	  } else if(d2.kind == d3_comp.kind && d3.kind == STypeKind::KIND_MAT) {
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
	  if constexpr (tt2::getKind() == STypeKind::KIND_TEXTURE) {
	      // OpImageSampleExplicitLod
	      SUtils::add(res, (7 << 16) | 88);
	      SUtils::add(res, vec4_s::getID());
	      SUtils::add(res, this->getID());
	      SUtils::add(res, this->v1->getID());
	      SUtils::add(res, this->v2->getID());
	      SUtils::add(res, 2); // LoD
	      SUtils::add(res, SConstantRegistry::getIDFloat(32, 0.0f));
	    } else if constexpr (tt2::getKind() == STypeKind::KIND_MAT) {
	    
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
	    } else if constexpr (tt2::getKind() == STypeKind::KIND_ARR ||
				 tt2::getKind() == STypeKind::KIND_RUN_ARR) {
	      int temp_id = SUtils::getNewID();
	    
	      // OpAccessChain <result_pointer_type> <result_id> <array_pointer> <index>
	      SUtils::add(res, (5 << 16) | 65);
	      SUtils::add(res, SPointer<(SStorageClass)tt2::getArg0(),
			  typename tt2::firstInnerType>::getID()); 
	      SUtils::add(res, temp_id);
	      SUtils::add(res, this->v1->getID());
	      SUtils::add(res, this->v2->getID());

	      // OpLoad
	      SUtils::add(res, (4 << 16) | 61);
	      SUtils::add(res, tt2::firstInnerType::getID());
	      SUtils::add(res, this->getID());
	      SUtils::add(res, temp_id);
	    
	    }else {
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
	    } else {
	      printf("[spurv] Conversion float to int not yet implemented (if that's what you tried)\n");
	      exit(-1);
	    }
	  } else {
	    printf("[spurv] Type not yet supported for conversion\n");
	  }
	} else {
	printf("[spurv] Expression operation not yet implemented\n");
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
    ex->register_left_node(v1);
    return *ex;
  }
  
  
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
  

  /*
   * Multiplication overrides - matrix multiplication, matrix scaling, element-wise multiplication
   */
  
  template<typename tt1, typename tt2>
  BOOL_CONCEPT MultiplicableSpurvMatrices =
    (RequireOneSpurvValue<tt1, tt2> &&
     is_spurv_mat_type<typename SValueWrapper::ToType<tt1>::type>::value &&
    is_spurv_mat_type<typename SValueWrapper::ToType<tt2>::type>::value &&
     (SValueWrapper::ToType<tt1>::type::mm == SValueWrapper::ToType<tt2>::type::nn) &&
    (std::is_same<typename SValueWrapper::ToType<tt1>::type::inner_type,
     typename SValueWrapper::ToType<tt2>::type::inner_type>::value));

  template<typename t1, typename t2>
  struct matrix_multiplication_res_type {};
  
  template<int n, int m, int a, int b, typename inner>
  struct matrix_multiplication_res_type<SMat<n, m, inner>, SMat<a, b, inner> > {
    using type = SMat<n, b, inner>;
  };

  // Matrix multiplication
  template<typename tt1, typename tt2>
  requires MultiplicableSpurvMatrices<typename std::remove_reference<tt1>::type, 
				      typename std::remove_reference<tt2>::type>
  SExpr<typename matrix_multiplication_res_type<typename SValueWrapper::unwrapped_type<tt1>::type,
						typename SValueWrapper::unwrapped_type<tt2>::type>::type,
	EXPR_DOT,
	typename SValueWrapper::unwrapped_type<tt1>::type,
	typename SValueWrapper::unwrapped_type<tt2>::type>& operator*(tt1&& ml, tt2&& mr) {
    using matl = typename SValueWrapper::ToType<tt1>::type;
    using matr = typename SValueWrapper::ToType<tt2>::type;
    using matres = typename matrix_multiplication_res_type<typename SValueWrapper::ToType<tt1>::type,
							   typename SValueWrapper::ToType<tt2>::type>::type;

    SExpr<matres, EXPR_DOT, matl, matr>* ex = SUtils::allocate<SExpr<matres, EXPR_DOT, matl, matr> >();

    ex->register_left_node(SValueWrapper::unwrap_to<tt1, matl>(ml));
    ex->register_right_node(SValueWrapper::unwrap_to<tt2, matr>(mr));
    return *ex;
  }

  template<typename mtt, typename stt>
  BOOL_CONCEPT MatrixScalable =
    RequireOneSpurvValue<mtt, stt> &&
    is_spurv_mat_type<typename SValueWrapper::unwrapped_type<mtt>::type>::value &&
    SValueWrapper::does_wrap<stt, typename SValueWrapper::unwrapped_type<mtt>::type::inner_type>::value;

  template<typename tt1, typename tt2>
  struct matrix_type {
    using rtt1 = typename std::remove_reference<tt1>::type;
    using rtt2 = typename std::remove_reference<tt2>::type;

    using it1 = typename SValueWrapper::unwrapped_type<rtt1>::type;
    using it2 = typename SValueWrapper::unwrapped_type<rtt2>::type;

    using type = typename std::conditional<is_spurv_mat_type<it1>::value,
					   it1, it2>::type;
  };

  // Assuming one gives a matrix type
  template<typename tt1, typename tt2>
  struct scalar_type {
    using rtt1 = typename std::remove_reference<tt1>::type;
    using rtt2 = typename std::remove_reference<tt2>::type;

    using it1 = typename SValueWrapper::unwrapped_type<rtt1>::type;
    using it2 = typename SValueWrapper::unwrapped_type<rtt2>::type;

    using type = typename std::conditional<is_spurv_mat_type<it1>::value,
					   it2, it1>::type;
  };

  // Matrix / vector scaling
  template<typename tt1, typename tt2>
  requires (MatrixScalable<typename std::remove_reference<tt1>::type,
	    typename std::remove_reference<tt2>::type> ||
	    MatrixScalable<typename std::remove_reference<tt2>::type,
	    typename std::remove_reference<tt1>::type>)
    SExpr<typename matrix_type<tt1, tt2>::type,
	  EXPR_MULTIPLICATION,
	  typename matrix_type<tt1, tt2>::type,
	  typename matrix_type<tt1, tt2>::type::inner_type>& operator*(tt1&& ml, tt2&& mr) {
    using comp_type = typename matrix_type<tt1, tt2>::type::inner_type;
    using mat_type = typename matrix_type<tt1, tt2>::type;
    

    SExpr<mat_type, EXPR_MULTIPLICATION, mat_type, comp_type>* ex =
      SUtils::allocate<SExpr<mat_type, EXPR_MULTIPLICATION, mat_type, comp_type> >();

    ex->register_left_node(SValueWrapper::unwrap_to<tt1, mat_type>(ml));
    ex->register_right_node(SValueWrapper::unwrap_to<tt2, comp_type>(mr));
    return *ex;
    }

  template<typename tt1, typename tt2>
  requires (MatrixScalable<typename std::remove_reference<tt2>::type,
	    typename std::remove_reference<tt1>::type>)
    SExpr<typename matrix_type<tt1, tt2>::type,
	  EXPR_MULTIPLICATION,
	  typename matrix_type<tt1, tt2>::type,
	  typename matrix_type<tt1, tt2>::type::inner_type>& operator*(tt1&& ml, tt2&& mr) {
    return mr * ml;
  }
  
  
  template<typename tt1, typename tt2>
  BOOL_CONCEPT HasSameType =
    RequireOneSpurvValue<tt1, tt2> &&
    SValueWrapper::does_wrap<typename std::remove_reference<typename get_not_spurv_value<tt1, tt2>::type>::type,
			     typename std::remove_reference<typename SValueWrapper::ToType<typename get_spurv_value<tt1, tt2>::type>::type>::type>::value;

  template<typename tt1, typename tt2>
  struct get_common_type {
    using type = typename SValueWrapper::ToType<typename get_spurv_value<tt1, tt2>::type>::type;
  };

  template<typename tt1>
  BOOL_CONCEPT NotWideMatrix =
    !is_spurv_mat_type<tt1>::value || tt1::getArg1() == 1;
  
  // Elementwise multiplication
  template<typename tt1, typename tt2>
  requires (HasSameType<typename std::remove_reference<tt1>::type,
		       typename std::remove_reference<tt2>::type> &&
	    NotWideMatrix<typename get_common_type<tt1, tt2>::type>)
  SExpr<typename get_common_type<tt1, tt2>::type,
	EXPR_MULTIPLICATION,
	typename get_common_type<tt1, tt2>::type,
	typename get_common_type<tt1, tt2>::type>& operator*(tt1&& el, tt2&& er) {

    using sptype = typename get_common_type<tt1, tt2>::type;
    
    SExpr<sptype, EXPR_MULTIPLICATION, sptype, sptype>* ex =
      SUtils::allocate<SExpr<sptype, EXPR_MULTIPLICATION, sptype, sptype> >();

    ex->register_left_node(SValueWrapper::unwrap_to<tt1, sptype>(el));
    ex->register_right_node(SValueWrapper::unwrap_to<tt2, sptype>(er));

    return *ex;
  }


  template<typename tt1, typename tt2>
  BOOL_CONCEPT AreDottable =
    (RequireOneSpurvValue<tt1, tt2> &&
     HasSameType<typename std::remove_reference<tt1>::type,
     typename std::remove_reference<tt2>::type> &&
     is_spurv_mat_type<typename SValueWrapper::unwrapped_type<tt1>::type>::value &&
     SValueWrapper::unwrapped_type<tt1>::type::mm == 1);
     

  template<typename tt1, typename tt2>
  requires AreDottable<tt1, tt2>
  SExpr<typename get_common_type<tt1, tt2>::type::inner_type,
	EXPR_DOT,
	typename get_common_type<tt1, tt2>::type,
	typename get_common_type<tt1, tt2>::type>& dot(tt1&& el, tt2&& er) {
    using sptype = typename get_common_type<tt1, tt2>::type;

    SExpr<typename sptype::inner_type, EXPR_DOT, sptype, sptype>* ex =
      SUtils::allocate<SExpr<typename sptype::inner_type, EXPR_DOT, sptype, sptype> >();

    ex->register_left_node(SValueWrapper::unwrap_to<tt1, sptype>(el));
    ex->register_right_node(SValueWrapper::unwrap_to<tt2, sptype>(er));

    return *ex;
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
  template<typename ti>
  SValue<typename lookup_result<tt>::type>&
  SValue<tt>::operator[](SValue<ti>& index) {

    static_assert(is_lookup_index<tt, ti>::value, "Value cannnot be used as index into this datatype");
    SExpr<typename lookup_result<tt>::type, EXPR_LOOKUP,
	  tt, ti>* ex  =
      SUtils::allocate<SExpr<typename lookup_result<tt>::type, EXPR_LOOKUP,
			     tt, ti> >(); 
    ex->register_left_node(*this);
    ex->register_right_node(index);
    return *ex;
  } 

  template<typename tt>
  SValue<typename lookup_result<tt>::type>& SValue<tt>::operator[](int index) {

    static_assert(is_lookup_index<tt, SInt<32, 1> >::value, "Integer value cannot be used as index into this datatype");
    Constant<int>* c = SUtils::allocate<Constant<int> >(index);
    SExpr<typename lookup_result<tt>::type, EXPR_LOOKUP,
	  tt, SInt<32, 1> >* ex  =
      SUtils::allocate<SExpr<typename lookup_result<tt>::type, EXPR_LOOKUP,
			     tt, SInt<32, 1> > >(); 
    ex->register_left_node(*this);
    ex->register_right_node(*c);

    return *ex;
  }


  /*
   * Casting
   */

  template<typename t1, typename t2>
  SExpr<t1, EXPR_CAST, t2>& cast(SValue<t2>& val) {
    static_assert(is_spurv_castable<t2, t1>::value,
		  "[spurv::cast] The supplied type is not castable to desired type");
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
	       typename uwr<t1, t2>::type>& construct_comparison_val(t1 v1, t2 v2) {
    using tt = typename uwr<t1, t2>::type;
    static_assert(is_spurv_int_type<tt>::value || is_spurv_float_type<tt>::value,
		  "Comparison not yet defined for non-scalar types");

    SExpr<SBool, op, tt, tt>* ex =
      SUtils::allocate<SExpr<SBool, op, tt, tt> >();

    ex->register_left_node(SValueWrapper::unwrap_to<t1, tt>(v1));
    ex->register_right_node(SValueWrapper::unwrap_to<t2, tt>(v2));
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
