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
    node.incrementRefCount();
    if(this->v1 != nullptr) {
      this->v1->unref_tree();
    }
    v1 = &node;
  };
  
  template<typename tt, SExprOp op, typename tt2, typename tt3>
  void SExpr<tt, op, tt2, tt3>::register_right_node(SValue<tt3>& node) {
    node.incrementRefCount();
    if(this->v2 != nullptr) {
      this->v2->unref_tree();
    }
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
  void SExpr<tt, op, tt2, tt3>::unref_tree() {

    this->ref_count--;

    if(this->ref_count <= 0) {
      if(v1) {
	v1->unref_tree();
      }
      if(v2) {
	v2->unref_tree();
      }
      delete this;
    }
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
    
    
    int opcode = 0;
    
    if(d1 == d2 && d2 == d3) {
      if (d1.kind == KIND_INT) {
	if constexpr(op == EXPR_ADDITION) {
	    opcode = 128;
	  } else if constexpr(op == EXPR_SUBTRACTION) {
	    opcode = 130;
	  } else if constexpr(op == EXPR_MULTIPLICATION) {
	    opcode = 132;
	  } else if constexpr(op == EXPR_DIVISION) {
	    opcode = (d1.a1 == 0 ? 134 : 135);
	  } else {
	  printf("Tried to output expression with integer type and operation = %d\n", op);
	  exit(-1);
	}
      } else if (d1.kind == KIND_FLOAT) {
	if constexpr(op == EXPR_ADDITION) {
	    opcode = 129;
	  } else if constexpr(op == EXPR_SUBTRACTION) {
	    opcode = 131;
	  } else if constexpr(op == EXPR_MULTIPLICATION) {
	    opcode = 133;
	  } else if constexpr(op == EXPR_DIVISION) {
	    opcode = 136;
	  } else {
	  printf("Tried to output expression with float type and operation = %d\n", op);
	  exit(-1);
	}
      } else {
	printf("Tried to output expression where types were equal, but not int or float (feature yet to be implemented, probably\n");
	exit(-1);
      }
      
      SUtils::add(res, (5 << 16) | opcode);
      SUtils::add(res, tt::getID());
      SUtils::add(res, this->getID());
      SUtils::add(res, this->v1->getID());
      SUtils::add(res, this->v2->getID());

    } else {
      if constexpr(op == EXPR_MULTIPLICATION) {
	  if (d2.kind == KIND_MAT && d3.kind == KIND_FLOAT) {
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
	  } else if(d2.kind == KIND_FLOAT && d3.kind == KIND_MAT) {
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
	    if(d1.kind != KIND_FLOAT) {
	      printf("Operands should have made scalar, but didn't\n");
	      exit(-1);
	    }
	    SUtils::add(res, (5 << 16) | 148);
	    SUtils::add(res, tt::id);
	    SUtils::add(res, this->getID());
	    SUtils::add(res, this->v1->getID());
	    SUtils::add(res, this->v2->getID());
	  } else if (d3.a1 == 1) {
	    SUtils::add(res, (5 << 16) | 145); // Vector times Matrix
	    SUtils::add(res, tt::id);
	    SUtils::add(res, this->getID());
	    SUtils::add(res, this->v1->getID());
	    SUtils::add(res, this->v2->getID());
	  } else if (d2.a1 == 1) {
	    printf("Vector - matrix multiplication not yet implemented!\n");
	    exit(-1);
	  } else {
	    SUtils::add(res, (5 << 16) | 146);
	    SUtils::add(res, tt::id);
	    SUtils::add(res, this->getID());
	    SUtils::add(res, this->v1->getID());
	    SUtils::add(res, this->v2->getID());
	  }
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
    SExpr<tt, EXPR_NEGATIVE, tt, void_s>* ex = new SExpr<tt, EXPR_NEGATIVE, tt, void_s>;
    ex->v1 = (&v1);
    ex->v2 = nullptr;
    return *ex;
  }
  
  template<typename tt>
  SExpr<tt, EXPR_ADDITION, tt, tt>& operator+(SValue<tt>& v1, SValue<tt>& v2) {
    SExpr<tt, EXPR_ADDITION, tt, tt>* ex = new SExpr<tt, EXPR_ADDITION, tt, tt>;
    ex->register_left_node(v1);
    ex->register_right_node(v2);
    return *ex;
  };

  template<typename tt>
  SExpr<tt, EXPR_SUBTRACTION, tt, tt>& operator-(SValue<tt>& v1, SValue<tt>& v2) {
    SExpr<tt, EXPR_SUBTRACTION, tt, tt>* ex = new SExpr<tt, EXPR_SUBTRACTION, tt, tt>;
    ex->register_left_node(v1);
    ex->register_right_node(v2);
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
    SExpr<res_type, EXPR_MULTIPLICATION, tt1, tt2>* ex = new SExpr<res_type, EXPR_MULTIPLICATION, tt1, tt2>;
    ex->register_left_node(v1);
    ex->register_right_node(v2);
    return *ex;
  }

  template<int n, int m, int a, int b>
  SExpr<SMat<n, b>, EXPR_DOT, SMat<n, m>, SMat<a, b> >&
  operator*(SValue<SMat<n, m> >& v1, SValue<SMat<a, b> >& v2) {
    static_assert(m == a || (m == 1 && b == 1));
    SExpr<SMat<n, b>, EXPR_DOT, SMat<n, m>, SMat<a, b> >* ex =
      new SExpr<SMat<n, b>, EXPR_DOT, SMat<n, m>, SMat<a, b> >;
    ex->register_left_node(v1);
    ex->register_right_node(v2);
    return *ex;
  }

  template<typename tt1>
  SExpr<tt1, EXPR_MULTIPLICATION, tt1, float_s>& operator*(SValue<tt1>& v1, const float& f) {
    static_assert(is_spurv_mat_type<tt1>::value || is_spurv_float_type<tt1>::value);
    
    Constant<float>* c = new Constant<float>(f);
    SExpr<tt1, EXPR_MULTIPLICATION, tt1, float_s>* ex = new SExpr<tt1, EXPR_MULTIPLICATION, tt1, float_s>;

    ex->register_left_node(v1);
    ex->register_right_node(*c);
    return *ex;
  }

  template<typename tt1>
  SExpr<tt1, EXPR_MULTIPLICATION, tt1, float_s>& operator*(const float& f, SValue<tt1>& v1) {
    return v1 * f;
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
