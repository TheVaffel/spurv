#ifndef INCLUDED_SPURV_HPP
#define INCLUDED_SPURV_HPP

#include <vector>
#include <sstream>
#include <iostream>
#include <cmath>
#include <cassert>

#include <string.h>

#include <FlatAlg.hpp>

static constexpr uint num_digits(int a) {
  return (uint)(log10(a)) + 1;
}

static constexpr char digit_to_char(int a) {
  return a + '0';
}

static constexpr int num_to_string(char* c, int n) {
  int u = num_digits(n);
  int currn = n;
  for(int i = u - 1; i >= 0; i--) {
    int dig = currn % 10;
    currn = currn / 10;
    *(c + i) = digit_to_char(dig);
  }
  return n;
}

static int string_word_length(const std::string str) {
  return (str.length() + 1 + 3 ) / 4; // Make room for terminating zero, round up to 4-byte words
}

static void add(std::vector<uint32_t>& binary, int a) {
  binary.push_back(a);
}

static void add(std::vector<uint32_t>& binary, std::string str) {
  int len = str.length(); // Space for null terminator
  int n = string_word_length(str);

  const char* pp = str.c_str();
  for(int i = 0; i < n - 1; i++) {
    add(binary, *((int32_t*)(pp + 4 * i)));
  }

  int left = len - (n - 1) * 4;
  char last_int[4];
  for(int i = 0; i < left; i++) {
    last_int[i] = str[(n - 1) * 4 + i];
  }
  
  for(int i = left; i < 4; i++) {
    last_int[i] = 0; // Pad with zeros
  }

  add(binary, *(int32_t*)last_int);
}

namespace spurv {
  
  static int global_id_counter = 1;
  
  enum SpurvTypeKind {
    SPURV_TYPE_INVALID,
    SPURV_TYPE_VOID,
    SPURV_TYPE_INT,
    SPURV_TYPE_FLOAT,
    SPURV_TYPE_MAT,
    SPURV_TYPE_ARR,
    SPURV_TYPE_POINTER
  };

  // Used to avoid recursive problems in SpurvType
  class NullType {
  public:
    static void ensure_defined(std::vector<uint32_t>&,
			       std::vector<int32_t*>&) {}
    static void define(std::vector<uint32_t>&) {}
  };

  
  // NB: If adding more arguments, remember to add more arguments to SpurvType and
  // is_spurv_type below as well
  struct DSpurvType {
    SpurvTypeKind kind;
    int a0, a1;
    DSpurvType* inner_type;
    
    
    constexpr DSpurvType() : kind(SPURV_TYPE_INVALID),
      a0(0), a1(0), inner_type(nullptr) { }

    ~DSpurvType() {
      if (inner_type) {
	delete inner_type;
      }
    }

    bool operator==(const DSpurvType& ds) const {
      bool a = true;
      a = a && this->kind == ds.kind;
      a = a && this->a0 == ds.a0;
      a = a && this->a1 == ds.a1;
      if(this->inner_type && ds.inner_type) {
	return a && (*this->inner_type == *ds.inner_type);
      } else if( this->inner_type || ds.inner_type) {
	return false;
      }
      
      return a;
      
    }
  };

  
  template<SpurvTypeKind kind, int arg0 = 0, int arg1 = 0, typename inner_type = NullType>
  class SpurvType {
    void init_name();
  protected:
    static int id;
  public:
    char name[32];
    constexpr SpurvType() { static_assert(kind == SPURV_TYPE_VOID); init_name(); id = -1; }

    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<int*>& ids);
    static void ensure_defined_dependencies(std::vector<uint32_t>& bin, std::vector<int*>& ids);
    static void define(std::vector<uint32_t>& bin);
    
    static void get_dspurvtype(DSpurvType* type) {
      type->kind = kind;
      type->a0 = arg0;
      type->a1 = arg1;
      if constexpr (!std::is_same<inner_type, NullType>::value) {
	type->inner_type = new DSpurvType();
	inner_type::get_dspurvtype(type->inner_type);
      }
    }

    static int getID() {
      if(id == -1) {
	printf("Kind = %d, arg0 = %d, arg1 = %d\n", kind, arg0, arg1);
	printf("Tried to use id before defined\n");
	exit(-1);
      }
      return id;
    }
  };

  template<SpurvTypeKind kind, int arg0, int arg1, typename inner_type>
  int SpurvType<kind, arg0, arg1, inner_type>::id = -1;
  
  template<>
  void SpurvType<SPURV_TYPE_VOID>::define(std::vector<uint32_t>& bin) {
    id = global_id_counter++;
    add(bin, (2 << 16) | 19);
    add(bin, SpurvType<SPURV_TYPE_VOID>::id);
  }

  template<>
  void SpurvType<SPURV_TYPE_VOID>::ensure_defined(std::vector<uint32_t>& bin, std::vector<int*>& ids) {
    if( SpurvType<SPURV_TYPE_VOID>::id < 0) {
      define(bin);
      ids.push_back(&(SpurvType<SPURV_TYPE_VOID>::id));
    }
  }
  
  template<SpurvTypeKind kind, int arg0, int arg1, typename inner_type>
  void SpurvType<kind, arg0, arg1, inner_type>::ensure_defined_dependencies(std::vector<uint32_t>& bin,
										   std::vector<int*>& ids) { }
  
  
  template<>
  constexpr void SpurvType<SPURV_TYPE_VOID>::init_name() {
    const char* ss = "void";
    for(int i = 0; i < 5; i++) {
      name[i] = ss[i];
    }
  }
  
  template<int n, int signedness>
  class SpurvInt : public SpurvType<SPURV_TYPE_INT, n, signedness> {
    constexpr void init_name() {
      const char* ss = "int";
      int sn = strlen(ss);
      for(int i = 0; i < sn; i++) {
	this->name[i] = ss[i];
      }
      int l = sn + num_to_string(this->name + sn, n);
      this->name[l] = '\0';
    }

    constexpr SpurvInt() { init_name();}

    static void define(std::vector<uint32_t>& bin) {
      SpurvInt<n, signedness>::id = global_id_counter++;
      add(bin, (4 << 16) | 21);
      add(bin, SpurvInt<n, signedness>::id);
      add(bin, n); // Width
      add(bin, signedness); // 0 = unsigned, 1 = signed
    }
  };

    
  template<int n>
  class SpurvFloat : public SpurvType<SPURV_TYPE_FLOAT, n> {
    constexpr void init_name() {
      const char* ss = "float";
      int sn = strlen(ss);
      for(int i = 0; i < sn; i++) {
	this->name[i] = ss[i];
      }
      int l = sn + num_to_string(this->name + sn, n);
      this->name[l] = '\0';
    }
    constexpr SpurvFloat() { init_name();}
  public:
    static void ensure_defined_dependencies(std::vector<uint32_t>& bin,
					    std::vector<int*>& ids) {}
    
    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<int*>& ids) {
      if(SpurvType<SPURV_TYPE_FLOAT, n>::id < 0) {
	define(bin);
	ids.push_back(&(SpurvType<SPURV_TYPE_FLOAT, n>::id));
      }
    }
    
    static void define(std::vector<uint32_t>& bin) {
      SpurvFloat<n>::id = global_id_counter++;
      add(bin, (3 << 16) | 22);
      add(bin, SpurvType<SPURV_TYPE_FLOAT, n>::id);
      add(bin, n);
    }
  };

  template<int n, int m>
  class SpurvMat : public SpurvType<SPURV_TYPE_MAT, n, m> {
    constexpr void init_name() {
      const char* ss = "mat";
      int sn = strlen(ss);
      for(int i = 0; i < sn; i++) {
	this->name[i] = ss[i];
      }
      int l = sn + num_to_string(this->name + sn, n);
      this->name[l++] = 'x';
      l += num_to_string(this->name + l, m);
      this->name[l] = '\0';
    }
    constexpr SpurvMat() { init_name();}

  public:
    
    static void ensure_defined_dependencies(std::vector<uint32_t>& bin,
					    std::vector<int*>& ids) {
      if constexpr(m == 1) {
	  SpurvFloat<32>::ensure_defined(bin, ids);
	} else {
	SpurvType<SPURV_TYPE_MAT, n, 1>::ensure_defined(bin, ids);
      }
    }
    
    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<int*>& ids) {
      printf("Ensuring mat %d %d defined\n", n, m);
      if(SpurvType<SPURV_TYPE_MAT, n, m>::id < 0) {
	ensure_defined_dependencies(bin, ids);
	define(bin);
	ids.push_back(&(SpurvType<SPURV_TYPE_MAT, n, m>::id));
      }
    }
    
    static void define(std::vector<uint32_t>& bin) {
      SpurvMat<n, m>::id = global_id_counter++;
      if constexpr(m == 1) {
	  add(bin, (4 << 16) | 23);
	  add(bin, SpurvType<SPURV_TYPE_MAT, n, 1>::getID());
	  add(bin, SpurvType<SPURV_TYPE_FLOAT, 32>::getID());
	  add(bin, n);
	} else {
	add(bin, (4 << 16) | 24);
	add(bin, SpurvType<SPURV_TYPE_MAT, n, m>::getID());
	add(bin, SpurvType<SPURV_TYPE_MAT, n, 1>::getID());
	add(bin, m);
      }
    }
  };
  
  template<typename>
  struct is_spurv_type : std::false_type {};

  template<int n, int s>
  struct is_spurv_type<SpurvInt<n, s> > : std::true_type {};

  template<int n>
  struct is_spurv_type<SpurvFloat<n> > : std::true_type {};

  template<int n, int m>
  struct is_spurv_type<SpurvMat<n, m> > : std::true_type {};

  template<>
  struct is_spurv_type<NullType> : std::true_type {};

  template<int n, typename tt>
  class SpurvArr : public SpurvType<SPURV_TYPE_ARR, n, 0, tt >  {
    
    constexpr void init_name() {
      this->name[0] = 'a';
      this->name[1] = 'r';
      this->name[2] = 'r';
      this->name[3] = '_';
      int l = 4 + num_to_string(this->name + 4, n);
      this->name[l] = '\0';
    }
    
    constexpr SpurvArr() {
      static_assert(is_spurv_type<tt>::value, "Inner type of SpurvArr must be a spurv type");
      init_name();
    }

  public:    
    static void ensure_defined_dependencies(std::vector<uint32_t>& bin,
					    std::vector<int*>& ids) {
      tt::ensure_defined(bin, ids);
    }

    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<int*>& ids) {
      if(SpurvType<SPURV_TYPE_ARR, n, 0, tt>::id < 0) {
	ensure_defined_dependencies(bin, ids);
	define(bin);
	ids.push_back(&(SpurvType<SPURV_TYPE_ARR, n, 0, tt>::id));
      }
    }
    
    static void define(std::vector<uint32_t>& bin) {
      SpurvArr<n, tt>::id = global_id_counter++;
      add(bin, (4 << 16) | 28);
      add(bin, SpurvType<SPURV_TYPE_ARR, n, 0, tt>::id);
      add(bin, tt::getID());
      add(bin, n);
    }
    
  };

  enum SpurvStorageClass {
    SPURV_STORAGE_UNIFORM_CONSTANT = 0,
    SPURV_STORAGE_INPUT = 1,
    SPURV_STORAGE_UNIFORM = 2,
    SPURV_STORAGE_OUTPUT = 3
    // ... There are more, but perhaps not needed just now
  };
  
  template<SpurvStorageClass storage, typename tt>
  class SpurvPointer : public SpurvType<SPURV_TYPE_POINTER, (int)storage, 0, tt> {
  public:
    static void ensure_defined_dependencies(std::vector<uint32_t>& bin,
					    std::vector<int*>& ids) {
      tt::ensure_defined(bin, ids);
    }

    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<int*>& ids) {
      if(SpurvType<SPURV_TYPE_POINTER, (int)storage, 0, tt>::id < 0) {
	ensure_defined_dependencies(bin, ids);
	define(bin);
	ids.push_back(&(SpurvType<SPURV_TYPE_POINTER, (int)storage, 0, tt>::id));
      }
    }

    static void define(std::vector<uint32_t>& bin) {
      SpurvPointer<storage, tt>::id = global_id_counter++;
      add(bin, (4 << 16) | 32);
      add(bin, SpurvType<SPURV_TYPE_POINTER, (int)storage, 0, tt>::id);
      add(bin, (int)storage);
      add(bin, tt::getID());
    }
  };

    
  template<typename>
  struct is_spurv_mat_type : std::false_type {};
  
  template<int n, typename tt>
  struct is_spurv_type<SpurvArr<n, tt> > : std::true_type { static_assert(is_spurv_type<tt>::value); };

  template<SpurvStorageClass storage, typename tt>
  struct is_spurv_type<SpurvPointer<storage, tt> > : std::true_type { static_assert(is_spurv_type<tt>::value); };

  template<int n, int m>
  struct is_spurv_mat_type<SpurvMat<n, m> > : std::true_type {};

  
  template<typename>
  struct is_spurv_float_type : std::false_type {};
  
  template<int n>
  struct is_spurv_float_type<SpurvFloat<n> > : std::true_type {};

  
  template<typename>
  struct is_spurv_int_type : std::false_type {};

  template<int n, int s>
  struct is_spurv_int_type<SpurvInt<n, s> > : std::true_type {};
  
  
  template<typename tt>
  struct MapSpurvType;

  typedef SpurvType<SPURV_TYPE_VOID> void_s;
  typedef SpurvInt<32, 1> int_s;
  typedef SpurvInt<32, 0> uint_s;
  typedef SpurvFloat<32> float_s;
  typedef SpurvMat<2, 2> mat2_s;
  typedef SpurvMat<3, 3> mat3_s;
  typedef SpurvMat<4, 4> mat4_s;
  typedef SpurvMat<2, 1> vec2_s;
  typedef SpurvMat<3, 1> vec3_s;
  typedef SpurvMat<4, 1> vec4_s;
  typedef SpurvArr<1, float_s> arr_1_float_s;

    
  template<>
  struct MapSpurvType<void> {
    typedef void_s type;
  };

  template<>
  struct MapSpurvType<int> {
    typedef int_s type;
  };

  template<>
  struct MapSpurvType<float> {
    typedef float_s type;
  };

  template<int n, int m>
  struct MapSpurvType<Matrix<n, m> > {
    typedef SpurvType<SPURV_TYPE_MAT, n, m> type;
  };
  
  enum ExpressionOperation {
    EXPR_NEGATIVE,
    EXPR_ADDITION,
    EXPR_SUBTRACTION,
    EXPR_MULTIPLICATION,
    EXPR_DIVISION,
    EXPR_DOT,
    EXPR_CROSS,
    EXPR_EXP,
    EXPR_SQRT,
    EXPR_POW
  };

  
  template<typename tt>
  struct ValueNode {
    static_assert(is_spurv_type<tt>::value);
  protected:
    uint id;
    bool defined;
    
  public:

    uint ref_count;
    ValueNode() { this->id = global_id_counter++; this->ref_count = 0; this->defined = false; }
    
    virtual void print_nodes_post_order(std::ostream& str) const { str << this->id << std::endl; }

    int getID() const {
      return id;
    }

    void declareDefined() {
      this->defined = true;
    }

    bool isDefined() {
      return this->defined;
    }

    virtual void define(std::vector<uint32_t>& res) = 0;

    virtual void ensure_type_defined(std::vector<uint32_t>& res, std::vector<int32_t*>& ids) {
      tt::ensure_defined(res, ids);
    }
    // Only deletes tree if ref_count reaches zero
    virtual void unref_tree() = 0;
  };

  
  template<typename tt>
  struct Constant : public ValueNode<typename MapSpurvType<tt>::type> {
    Constant(const tt& val) {
      this->value = val;
    }

    virtual void unref_tree() {
      this->ref_count--;
      if(this->ref_count <= 0) {
	delete this;
      }
    }

    virtual void define(std::vector<uint32_t>& res) {
      DSpurvType ds;
      MapSpurvType<tt>::type::get_dspurvtype(&ds);
      if (ds.kind == SPURV_TYPE_FLOAT ||
	  ds.kind == SPURV_TYPE_INT) {
	if(ds.a0 != 32) {
	  printf("Bitwidth of 32 is the only supported at this point\n");
	  exit(-1);
	}
        assert(ds.a0 == 32);
	add(res, (4 << 16) | 43);
	add(res, MapSpurvType<tt>::type::getID());
	add(res, this->getID());
	add(res, *(int32_t*)(&this->value));
      } else {
	printf("Constants not of integer or float type not yet supported");
	exit(-1);
      }

      this->declareDefined();
    }
    
    tt value;
  };

  
  template<typename tt>
  struct Var : public ValueNode<tt> {
    std::string name;
    Var();
    Var(std::string _name);
    virtual void unref_tree() {
      this->ref_count--;
      if (this->ref_count <= 0) {
	delete this;
      }
    }
  };

  template<typename tt>
  struct UniformVar : public Var<tt> {
    int set_no, bind_no;
    
    UniformVar(int s, int b) {
      this->set_no = s;
      this->bind_no = b;
    }
  };

  template<typename tt>
  struct InputVar : public Var<tt> {
    int input_no;
    int pointer_id;
    
    InputVar(int n, int pointer_id) {
      this->input_no = n;
      this->pointer_id = pointer_id;
    }

    virtual void define(std::vector<uint32_t>& res) {
      printf("Defining InputVar with id %d\n", this->id);
      // OpLoad
      add(res, (4 << 16) | 61);
      add(res, tt::getID());
      add(res, this->id);
      add(res, this->pointer_id);

      this->declareDefined();
    }
  };
  
  
  template<typename tt, ExpressionOperation op, typename tt2 = void_s, typename tt3 = void_s>
  struct Expr : public ValueNode<tt> {
    static_assert(is_spurv_type<tt>::value);
    static_assert(is_spurv_type<tt2>::value);
    static_assert(is_spurv_type<tt3>::value);
    
    // Enforce Expr to be at most binary (surely, this won't come back and bite me later...)
    ValueNode<tt2>* v1;
    ValueNode<tt3>* v2;

    Expr() {
      this->v1 = nullptr;
      this->v2 = nullptr;
    }

    void register_left_node(ValueNode<tt2>& node) {
      node.ref_count++;
      if(this->v1 != nullptr) {
        this->v1->unref_tree();
      }
      v1 = &node;
    };
    void register_right_node(ValueNode<tt3>& node) {
      node.ref_count++;
      if(this->v2 != nullptr) {
	this->v2->unref_tree();
      }
      v2 = &node;
    }

    // Since expressions are returned as references from e.g.
    // binary operations, if you assign a normal (non-reference) variable to an expression resulting
    // from such an operation, it will (if I understand correctly) call the copy constructor, and thus
    // we get a new object put on the stack, which we should not delete, although all its children are
    // heap-allocated, so they should be deleted. Also, this means the top-most node gets no references
    // and is lost in the void
    Expr(const Expr<tt, op, tt2, tt3>& e) = delete;

    virtual void print_nodes_post_order(std::ostream& str) const {
      v1->print_nodes_post_order(str);
      
      if constexpr(op != EXPR_NEGATIVE) {
	  v2->print_nodes_post_order(str);
	}
      str << this->getID() << std::endl;
    }

    virtual void unref_tree() {

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
    
    virtual void ensure_type_defined(std::vector<uint32_t>& res, std::vector<int32_t*>& ids) {
      if(this->v1) {
	this->v1->ensure_type_defined(res, ids);
      }

      if(this->v2) {
	this->v2->ensure_type_defined(res, ids);
      }

      tt::ensure_defined(res, ids);
    }

    virtual void define(std::vector<uint32_t>& res) {
      if(this->v1 && !this->v1->isDefined()) {
	this->v1->define(res);
      }

      if(this->v2 && !this->v2->isDefined()) {
	this->v2->define(res);
      }

      DSpurvType d1, d2, d3;
      tt::get_dspurvtype(&d1);
      tt2::get_dspurvtype(&d2);
      tt3::get_dspurvtype(&d3);
    
    
      int opcode = 0;
    
      if(d1 == d2 && d2 == d3) {
	if (d1.kind == SPURV_TYPE_INT) {
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
	} else if (d1.kind == SPURV_TYPE_FLOAT) {
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
      
	add(res, (5 << 16) | opcode);
	// Here, we use the fact that e.g. SpurvInt<n, s> is a class derived from
	// SpurvType<SPURV_TYPE_INT, n, s>, so that their id is shared. Mostly,
	// classes will probably be created with type e.g. SpurvInt<n, s>, so
	// this is important for this to work
	add(res, tt::getID());
	add(res, this->getID());
	add(res, this->v1->getID());
	add(res, this->v2->getID());

      } else {
	if constexpr(op == EXPR_MULTIPLICATION) {
	    if (d2.kind == SPURV_TYPE_MAT && d3.kind == SPURV_TYPE_FLOAT) {
	      // Differ between vector and matrix
	      if (d2.a1 == 1) {
		add(res, (5 << 16) | 142);
	      } else {
		add(res, (5 << 16) | 143);
	      }
	    
	      add(res, tt::getID());
	      add(res, this->getID());
	      add(res, this->v1->getID());
	      add(res, this->v2->getID());
	    } else if(d2.kind == SPURV_TYPE_FLOAT && d3.kind == SPURV_TYPE_MAT) {
	      if (d3.a1 == 1) {
		add(res, (5 << 16) | 142);
	      } else {
		add(res, (5 << 16) | 143);
	      }

	      add(res, tt::getID());
	      add(res, this->getID());
	      add(res, this->v2->getID());
	      add(res, this->v1->getID());
	    } else {
	      printf("Tried to use EXPR_MULTIPLICATION for something else than float times mat or float times float\n");
	      exit(-1);
	    }
	  } else if constexpr(op == EXPR_DOT) {
	    if (d2.a1 == 1 && d3.a1 == 1) {
	      if(d1.kind != SPURV_TYPE_FLOAT) {
		printf("Operands should have made scalar, but didn't\n");
		exit(-1);
	      }
	      add(res, (5 << 16) | 148);
	      add(res, tt::id);
	      add(res, this->getID());
	      add(res, this->v1->getID());
	      add(res, this->v2->getID());
	    } else if (d3.a1 == 1) {
	      add(res, (5 << 16) | 145); // Vector times Matrix
	      add(res, tt::id);
	      add(res, this->getID());
	      add(res, this->v1->getID());
	      add(res, this->v2->getID());
	    } else if (d2.a1 == 1) {
	      printf("Vector - matrix multiplication not yet implemented!\n");
	      exit(-1);
	    } else {
	      add(res, (5 << 16) | 146);
	      add(res, tt::id);
	      add(res, this->getID());
	      add(res, this->v1->getID());
	      add(res, this->v2->getID());
	    }
	  }
      }
    
      this->defined = true;
    }
  
  };

  
  // Operations:
  
  // NB: Doesn't cover all cases
  template<typename tt1, typename tt2>
  struct multiplication_res_type {
    typedef typename std::conditional<std::is_same<tt2, float_s>::value, tt1, tt2>::type tmp0_type;
    typedef typename std::conditional<std::is_same<tt1, float_s>::value, tt2, tmp0_type >::type type;
  };
  
  template<typename tt1, typename tt2>
  Expr<typename multiplication_res_type<tt1, tt2>::type, EXPR_MULTIPLICATION, tt1, tt2>& operator*(ValueNode<tt1>& v1, ValueNode< tt2 >& v2);

  template<int n, int m, int a, int b>
  Expr<SpurvMat<n, b>, EXPR_DOT, SpurvMat<n, m>, SpurvMat<a, b> >& operator*(ValueNode<SpurvMat<n, m> >& v1, ValueNode<SpurvMat<a, b> >& v2);
  
  template<typename tt>
  Expr<tt, EXPR_NEGATIVE, tt, void_s>& operator-(ValueNode<tt>& v1);
  
  template<typename tt>
  Expr<tt, EXPR_ADDITION, tt, tt>& operator+(ValueNode<tt>& v1, ValueNode<tt>& v2);
  
  template<typename tt>
  Expr<tt, EXPR_SUBTRACTION, tt, tt>& operator-(ValueNode<tt>& v1, ValueNode<tt>& v2);
    

  template<typename tt>
  std::ostream& operator<<(std::ostream& out_stream, ValueNode<tt>& v) {
    v.print_nodes_post_order(out_stream);
    return out_stream;
  }
  
  
  /*
   * IMPLEMENTATION
   */  

  template<typename tt>
  Var<tt>::Var() {
    std::cout << "Created Var with id " << this->getID() << std::endl;
  }
  
  template<typename tt>
  Var<tt>::Var(std::string _name) {
    this->name = _name;
  }

  template<typename tt>
  Expr<tt, EXPR_NEGATIVE, tt, void_s>& operator-(ValueNode<tt>& v1) {
    Expr<tt, EXPR_NEGATIVE, tt, void_s>* ex = new Expr<tt, EXPR_NEGATIVE, tt, void_s>;
    ex->v1 = (&v1);
    ex->v2 = nullptr;
    return *ex;
  }
  
  template<typename tt>
  Expr<tt, EXPR_ADDITION, tt, tt>& operator+(ValueNode<tt>& v1, ValueNode<tt>& v2) {
    Expr<tt, EXPR_ADDITION, tt, tt>* ex = new Expr<tt, EXPR_ADDITION, tt, tt>;
    ex->register_left_node(v1);
    ex->register_right_node(v2);
    return *ex;
  };

  template<typename tt>
  Expr<tt, EXPR_SUBTRACTION, tt, tt>& operator-(ValueNode<tt>& v1, ValueNode<tt>& v2) {
    Expr<tt, EXPR_SUBTRACTION, tt, tt>* ex = new Expr<tt, EXPR_SUBTRACTION, tt, tt>;
    ex->register_left_node(v1);
    ex->register_right_node(v2);
    return *ex;
  }
  
  
  template<typename tt1, typename tt2>
  Expr<typename multiplication_res_type<tt1, tt2>::type, EXPR_MULTIPLICATION, tt1, tt2>&
  operator*(ValueNode<tt1>& v1, ValueNode< tt2 >& v2) {
    // Don't mix integers and floats and don't allow matrix multiplication here
    static_assert((is_spurv_int_type<tt1>::value && is_spurv_int_type<tt2>::value) ||
		  (is_spurv_float_type<tt1>::value && is_spurv_float_type<tt2>::value) ||
		  (is_spurv_float_type<tt1>::value && is_spurv_mat_type<tt2>::value) ||
		  (is_spurv_mat_type<tt1>::value && is_spurv_float_type<tt2>::value));
    
    typedef typename multiplication_res_type<tt1, tt2>::type res_type;
    Expr<res_type, EXPR_MULTIPLICATION, tt1, tt2>* ex = new Expr<res_type, EXPR_MULTIPLICATION, tt1, tt2>;
    ex->register_left_node(v1);
    ex->register_right_node(v2);
    return *ex;
  }

  template<int n, int m, int a, int b>
  Expr<SpurvMat<n, b>, EXPR_DOT, SpurvMat<n, m>, SpurvMat<a, b> >&
  operator*(ValueNode<SpurvMat<n, m> >& v1, ValueNode<SpurvMat<a, b> >& v2) {
    static_assert(m == a || (m == 1 && b == 1));
    Expr<SpurvMat<n, b>, EXPR_DOT, SpurvMat<n, m>, SpurvMat<a, b> >* ex =
      new Expr<SpurvMat<n, b>, EXPR_DOT, SpurvMat<n, m>, SpurvMat<a, b> >;
    ex->register_left_node(v1);
    ex->register_right_node(v2);
    return *ex;
  }

  template<typename tt1>
  Expr<tt1, EXPR_MULTIPLICATION, tt1, float_s>& operator*(ValueNode<tt1>& v1, const float& f) {
    static_assert(is_spurv_mat_type<tt1>::value || is_spurv_float_type<tt1>::value);
    
    Constant<float>* c = new Constant<float>(f);
    Expr<tt1, EXPR_MULTIPLICATION, tt1, float_s>* ex = new Expr<tt1, EXPR_MULTIPLICATION, tt1, float_s>;

    ex->register_left_node(v1);
    ex->register_right_node(*c);
    return *ex;
  }

  template<typename tt1>
  Expr<tt1, EXPR_MULTIPLICATION, tt1, float_s>& operator*(const float& f, ValueNode<tt1>& v1) {
    return v1 * f;
  }
 
  template<int n, typename...Types>
  struct NthType {
    using type = typename std::tuple_element<n, std::tuple<Types...> >::type;
  };
  
  enum SpurvShaderType {
    SPURV_SHADER_VERTEX,
    SPURV_SHADER_FRAGMENT,
  };

  enum BuiltinVariableIndex {
    BUILTIN_POSITION,
    BUILTIN_POINT_SIZE,
    BUILTIN_CLIP_DISTANCE,
    BUILTIN_CULL_DISTANCE
  };

  template<SpurvShaderType type, typename... InputTypes>
  class SpurvShader {
    struct UniformEntry {
      int set_no, bind_no;
      int id;
      DSpurvType ds;
    };

    struct InputVariableEntry {
      int id;
      DSpurvType ds;
      int pointer_id;

      void* value_node;
      
      InputVariableEntry() { id = -1; }
    };

    template<typename s_type>
    struct BuiltinEntry {
      ValueNode<s_type>* value_node;
      int pointer_id;
    };
    
    // We use this to reset the type ids (stored for each type) after compilation
    std::vector<int*> defined_type_ids;
    
    std::vector<UniformEntry> uniform_entries;
    std::vector<InputVariableEntry> input_entries;
    std::vector<uint32_t> output_pointer_ids;

    int glsl_id;
    int entry_point_id;
    int entry_point_declaration_size_index;
    int id_max_bound_index; // Will be constant, but oh well
    
    void output_preamble(std::vector<uint32_t>& binary);
    void output_shader_header_begin(std::vector<uint32_t>& binary);
    void output_used_builtin_ids(std::vector<uint32_t>& bin);
    void output_shader_header_decorate_begin(std::vector<uint32_t>& bin);

    template<typename tt>
    void output_shader_header_output_variables(std::vector<uint32_t>& binary,
					       int n, ValueNode<tt>& val);
    template<typename tt, typename... NodeTypes>
    void output_shader_header_output_variables(std::vector<uint32_t>& binary, int n,
					       ValueNode<tt>& val, NodeTypes... args);

    template<typename tt>
    void output_shader_header_decorate_output_variables(std::vector<uint32_t>& binary, int n,
							ValueNode<tt>& val);
    template<typename tt, typename... NodeTypes>
    void output_shader_header_decorate_output_variables(std::vector<uint32_t>& binary, int n,
							ValueNode<tt>& val, NodeTypes... args);

    template<typename tt>
    void output_type_definitions(std::vector<uint32_t>& binary, ValueNode<tt>& val);

    template<typename tt, typename... NodeTypes>
    void output_type_definitions(std::vector<uint32_t>& binary, ValueNode<tt>& val,
				 NodeTypes... args);

    void output_main_function_begin(std::vector<uint32_t>& res);
    void output_main_function_end(std::vector<uint32_t>& res);
    
    template<typename tt>
    void output_output_definitions(std::vector<uint32_t>& res, int n, ValueNode<tt>& node);

    template<typename tt, typename... NodeType>
    void output_output_definitions(std::vector<uint32_t>& res, int n, ValueNode<tt>& node,
				   NodeType... args);

    template<int n>
    void output_input_pointers(std::vector<uint32_t>& res);
    
    template<int n, typename CurrType, typename... RestTypes>
    void output_input_pointers(std::vector<uint32_t>& res);

    template<typename tt, typename... NodeTypes>
    void output_output_pointers(std::vector<uint32_t>& res, int n, ValueNode<tt>& val, NodeTypes... args);

    template<typename tt>
    void output_output_pointers(std::vector<uint32_t>& res, int n, ValueNode<tt>& val);

    void output_used_builtin_pointers(std::vector<uint32_t>& res);
    
    void output_builtin_output_definitions(std::vector<uint32_t>& res);
    
    int get_num_defined_builtins();
    
    // Builtin outputs
    BuiltinEntry<vec4_s>* builtin_vec4_0; // Vertex: Position
    BuiltinEntry<float_s>* builtin_float_0; // Vertex: PointSize
    BuiltinEntry<arr_1_float_s>* builtin_arr_1_float_0; // Vertex: Clip Distance
    BuiltinEntry<arr_1_float_s>* builtin_arr_1_float_1; // Vertex: Cull Distance
    
    int get_new_value_id() {
      return global_id_counter++;
    }

    void cleanup_ids();
    
  public:
    SpurvShader();

    template<BuiltinVariableIndex ind, typename tt>
    void setBuiltinOutput(ValueNode<tt>& val);
    
    template<typename tt>
    ValueNode<tt>& addUniform(int set_no, int bind_no);

    template<int n, typename First>
    auto& getInputVariable();
    
    template<int n, int c, typename First, typename... Rest>
    auto& getInputVariable();
    
    template<int n>
    auto& getInputVariable();

    template<typename... NodeTypes>
    void compileToSpirv(std::vector<uint32_t>& res, NodeTypes&... args);

  };

  template<SpurvShaderType type, typename... InputTypes>
  template<typename tt>
  void SpurvShader<type, InputTypes...>::output_shader_header_output_variables(std::vector<uint32_t>& binary,
									       int n, ValueNode<tt>& arg0) {
    static_assert(is_spurv_type<tt>::value);
    binary.push_back(this->output_pointer_ids[n]);
  }
  
  template<SpurvShaderType type, typename... InputTypes>
  template<typename tt, typename... NodeTypes>
  void SpurvShader<type, InputTypes...>::output_shader_header_output_variables(std::vector<uint32_t>& binary,
									       int n, ValueNode<tt>& arg0, NodeTypes... args) {
    static_assert(is_spurv_type<tt>::value);
    binary.push_back(this->output_pointer_ids[n]);
    output_shader_header_output_variables(binary, n + 1, args...);
  }

  template<SpurvShaderType type, typename... InputTypes>
  template<typename tt>
  void SpurvShader<type, InputTypes...>::output_shader_header_decorate_output_variables(std::vector<uint32_t>& bin, int n,
									 ValueNode<tt>& arg0) {
    bin.push_back((4 << 16) | 71);
    bin.push_back(this->output_pointer_ids[n]);
    bin.push_back(30);
    bin.push_back(n);
  }
									 
  
  template<SpurvShaderType type, typename... InputTypes>
  template<typename tt, typename... NodeTypes>
  void SpurvShader<type, InputTypes...>::output_shader_header_decorate_output_variables(std::vector<uint32_t>& bin, int n,
									 ValueNode<tt>& arg0, NodeTypes... args) {
    bin.push_back((4 << 16) | 71);
    bin.push_back(arg0->getID());
    bin.push_back(30);
    bin.push_back(n);

    output_shader_header_decorate_output_variables(bin, n + 1, args...);
    
  }
  
  template<SpurvShaderType type, typename... InputTypes>
  int SpurvShader<type, InputTypes...>::get_num_defined_builtins() {
    int num = 0;
    if(this->builtin_vec4_0) {
      num++;
    }
    if(this->builtin_float_0) {
      num++;
    }
    if(this->builtin_arr_1_float_0) {
      num++;
    }
    if(this->builtin_arr_1_float_1) {
      num++;
    }

    return num;
  }

  template<SpurvShaderType type, typename... InputTypes>
  void SpurvShader<type, InputTypes...>::output_used_builtin_ids(std::vector<uint32_t>& bin) {
    if(this->builtin_vec4_0) {
      bin.push_back(this->builtin_vec4_0->pointer_id);
    }
    if(this->builtin_float_0) {
      bin.push_back(this->builtin_float_0->pointer_id);
    }
    if(this->builtin_arr_1_float_0) {
      bin.push_back(this->builtin_arr_1_float_0->pointer_id);
    }
    if(this->builtin_arr_1_float_1) {
      bin.push_back(this->builtin_arr_1_float_1->pointer_id);
    }
  }
  
  template<SpurvShaderType type, typename... InputTypes>
  SpurvShader<type, InputTypes...>::SpurvShader() {
    builtin_vec4_0 = nullptr;
    builtin_float_0 = nullptr;
    builtin_arr_1_float_0 = nullptr;
    builtin_arr_1_float_1 = nullptr;

    input_entries = std::vector<InputVariableEntry>(sizeof...(InputTypes));
    
  }
  
  template<SpurvShaderType type, typename... InputTypes>
  void SpurvShader<type, InputTypes...>::output_preamble(std::vector<uint32_t>& binary) {
    binary.push_back(0x07230203); // Magic number
    binary.push_back(0x00010000); // Version number (1.0.0)
    binary.push_back(0x124);      // Generator's magic number (not officially registered)
    this->id_max_bound_index = binary.size();
    binary.push_back(0); // We'll set this later
    binary.push_back(0x0); // For instruction schema (whatever that means)
  }

  template<SpurvShaderType type, typename... InputTypes>
  template<typename tt>
  void SpurvShader<type, InputTypes...>::output_type_definitions(std::vector<uint32_t>& bin, ValueNode<tt>& val) {
    
    printf("Ensuring type %s\n", typeid(tt).name());
    val.ensure_type_defined(bin, this->defined_type_ids);
  }

  
  template<SpurvShaderType type, typename... InputTypes>
  template<typename tt, typename... NodeTypes>
  void SpurvShader<type, InputTypes...>::output_type_definitions(std::vector<uint32_t>& bin, ValueNode<tt>& val, NodeTypes... args) {
    printf("Ensuring type %s\n", typeid(tt).name());
    val.ensure_type_defined(bin, this->defined_type_ids);

    this->output_type_definitions(bin, args...);
  }


  template<SpurvShaderType type, typename... InputTypes>
  void SpurvShader<type, InputTypes...>::output_shader_header_begin(std::vector<uint32_t>& bin) {
    // capability Shader
    add(bin, (2 << 16) | 17);
    add(bin, 1);

    // GLSL = ext_inst_import "GLSL.std.450"
    std::string glsl_import_str = "GLSL.std.450";
    int length = string_word_length(glsl_import_str);
    add(bin, ((2 + length) << 16) | 11);
    
    this->glsl_id = this->get_new_value_id();
    add(bin, this->glsl_id);
    add(bin, glsl_import_str);

    // memory_model Logical GLSL450
    add(bin, (3 << 16) | 14);
    add(bin, 0);
    add(bin, 1);

    // entry_point vertex_shader main "main" input/output_variables
    this->entry_point_declaration_size_index = bin.size(); // Add size afterwards
    add(bin, 15);

    std::string entry_point_name = "main";

    if constexpr(type == SPURV_SHADER_VERTEX) {
	add(bin, 0); // Vertex
      } else if constexpr(type == SPURV_SHADER_FRAGMENT) {
	add(bin, 4); // Fragment
      } else {
      printf("Shader type not yet accounted for in output_shader_header_begin\n");
      exit(-1);
    }

    this->entry_point_id = this->get_new_value_id();
    add(bin, this->entry_point_id);
    add(bin, entry_point_name);

    for(uint i = 0; i < this->input_entries.size(); i++) {
      printf("Num input entries: %d\n", input_entries.size());
      if (this->input_entries[i].id == -1) {
	printf("Input variable not initialized");
	exit(-1);
      }
      printf("Adding to entry point: Input var pointer id %d\n", this->input_entries[i].pointer_id);
      add(bin, this->input_entries[i].pointer_id);
    }

    output_used_builtin_ids(bin);
  }
  
  template<SpurvShaderType type, typename... InputTypes>
  void SpurvShader<type, InputTypes...>::output_shader_header_decorate_begin(std::vector<uint32_t>& bin) {
    if constexpr(type == SPURV_SHADER_VERTEX) {
	if(this->builtin_vec4_0) {
	  // Decorate <builtin> Builtin Position
	  add(bin, (4 << 16) | 71);
	  add(bin, this->builtin_vec4_0->pointer_id);
	  add(bin, 11);
	  add(bin, 0);
	}
    
	if(this->builtin_float_0) {
	  add(bin, (4 << 16) | 71);
	  add(bin, this->builtin_float_0->pointer_id);
	  add(bin, 11);
	  add(bin, 1);
	}
	if(this->builtin_arr_1_float_0) {
	  add(bin, (4 << 16) | 71);
	  add(bin, this->builtin_arr_1_float_0->pointer_id);
	  add(bin, 11);
	  add(bin, 3);
	}
	if(this->builtin_arr_1_float_1) {
	  add(bin, (4 << 16) | 71);
	  add(bin, this->builtin_arr_1_float_1->pointer_id);
	  add(bin, 11);
	  add(bin, 4);
	}
      } else {
      if(this->builtin_vec4_0 || this->builtin_float_0 ||
	 this->builtin_arr_1_float_0 || this->builtin_arr_1_float_1) {
	printf("Builtin variables for other than Vertex shader must still be implemented\n");
	exit(-1);
      }
    }

    // Decorate <input_var> Location <index>
    for(uint i = 0; i < input_entries.size(); i++) {
      add(bin, (4 << 16) | 71);
      add(bin, input_entries[i].id);
      add(bin, 30);
      add(bin, i);
    }

    for(uint i = 0; i < uniform_entries.size(); i++) {
      printf("Uniforms not properly implemented yet\n");
      exit(-1);
    }
  }
  
  template<SpurvShaderType type, typename... InputTypes>
  template<BuiltinVariableIndex ind, typename tt>
  void SpurvShader<type, InputTypes...>::setBuiltinOutput(ValueNode<tt>& val) {
    if constexpr(type == SPURV_SHADER_VERTEX) {
	if constexpr(ind == BUILTIN_POSITION) {
        
	    static_assert(std::is_same<tt, vec4_s>::value, "Position must be vec4_s");
	    if(this->builtin_vec4_0) {
	      printf("Cannot set builtin multiple times");
	      exit(-1);
	    }
	    this->builtin_vec4_0 = new BuiltinEntry<vec4_s>;
	    this->builtin_vec4_0->value_node = &val;
	    this->builtin_vec4_0->pointer_id = get_new_value_id();
	  } else if constexpr(ind == BUILTIN_POINT_SIZE) {
	    static_assert(std::is_same<tt, float_s>::value, "PointSize must be float_s");
	    
	    if(this->builtin_float_0) {
	      printf("Cannot set builtin multiple times");
	      exit(-1);
	    }
	    this->builtin_float_0 = new BuiltinEntry<float_s>;
	    this->builtin_float_0->value_node = &val;
	    this->builtin_float_0->pointer_id = get_new_value_id();
	  } else if constexpr(ind == BUILTIN_CLIP_DISTANCE) {
	    static_assert(std::is_same<tt, arr_1_float_s>::value, "Clip Distance must be arr_1_float_s");
	    
	    if(this->builtin_arr_1_float_0) {
	      printf("Cannot set builtin multiple times");
	      exit(-1);
	    }
	    this->builtin_arr_1_float_0 = new BuiltinEntry<arr_1_float_s>;
	    this->builtin_arr_1_float_0->value_node = &val;
	    this->builtin_arr_1_float_0->pointer_id = get_new_value_id();
	  } else if constexpr(ind == BUILTIN_CULL_DISTANCE) {
	    static_assert(std::is_same<tt, arr_1_float_s>::value, "Cull Distance must be arr_1_float_s");
	    
	    if(this->builtin_arr_1_float_1) {
	      printf("Cannot set builtin multiple times");
	      exit(-1);
	    }
	    this->builtin_arr_1_float_1 = new BuiltinEntry<arr_1_float_s>;
	    this->builtin_arr_1_float_1->value_node = &val;
	    this->builtin_arr_1_float_1->pointer_id = get_new_value_id();
	  } else {
	  printf("This builtin has not yet been defined in setBuiltinOutput");
	  exit(-1);
	}
      } else {
      printf("This shader type is not yet supported in setBuiltinOutput\n");
      exit(-1);
    }
  }
      

  /* template<>
  template<>
  void SpurvShader<SPURV_SHADER_VERTEX>::setBuiltinOutput<BUILTIN_POINT_SIZE, float_s>(ValueNode<float_s>& val) {
    if(this->builtin_float_0) {
      printf("Cannot set builtin multiple times");
      exit(-1);
    }
    this->builtin_float_0 = new BuiltinEntry<float_s>;
    this->builtin_float_0->value_node = &val;
    this->builtin_float_0->pointer_id = get_new_value_id();
  }

  template<>
  template<>
  void SpurvShader<SPURV_SHADER_VERTEX>::setBuiltinOutput<BUILTIN_CLIP_DISTANCE, arr_1_float_s>(ValueNode<arr_1_float_s>& val) {
    if(this->builtin_arr_1_float_0) {
      printf("Cannot set builtin multiple times");
      exit(-1);
    }
    this->builtin_arr_1_float_0 = new BuiltinEntry<arr_1_float_s>;
    this->builtin_arr_1_float_0->value_node = &val;
    this->builtin_arr_1_float_0->pointer_id = get_new_value_id();
  }

  template<>
  template<>
  void SpurvShader<SPURV_SHADER_VERTEX>::setBuiltinOutput<BUILTIN_CULL_DISTANCE, arr_1_float_s>(ValueNode<arr_1_float_s>& val) {
    if(this->builtin_arr_1_float_1) {
      printf("Cannot set builtin multiple times");
      exit(-1);
    }
    this->builtin_arr_1_float_1 = new BuiltinEntry<arr_1_float_s>;
    this->builtin_arr_1_float_1->value_node = &val;
    this->builtin_arr_1_float_1->pointer_id = get_new_value_id();
    } */

   template<SpurvShaderType type, typename... InputTypes>
  template<typename tt>
  ValueNode<tt>& SpurvShader<type, InputTypes...>::addUniform(int set_no, int bind_no) {
    UniformEntry entry;
    
    entry.set_no = set_no;
    entry.bind_no = bind_no;
    tt::get_dspurvtype(&(entry.ds));

    this->uniform_entries.push_back(entry);

    UniformVar<tt> *uv = new UniformVar<tt>(set_no, bind_no);

    entry.id = uv->getID();
    
    return *uv;
  }
  
  template<SpurvShaderType type, typename... InputTypes>
  template<int n>
  auto& SpurvShader<type, InputTypes...>::getInputVariable() {
    return this->getInputVariable<n, 0, InputTypes...>();
  }

  template<SpurvShaderType type, typename... InputTypes>
  template<int n, typename First>
   auto& SpurvShader<type, InputTypes...>::getInputVariable() {
    using input_type = First;
    static_assert(is_spurv_type<input_type>::value, "Supplied input values must be SpurvType");

    if(input_entries[n].ds.kind == SPURV_TYPE_INVALID) {
      
      input_type::get_dspurvtype(&(input_entries[n].ds));
      input_entries[n].pointer_id = this->get_new_value_id();
	
	
      InputVar<input_type> *iv = new InputVar<input_type>(n, input_entries[n].pointer_id);
	
      input_entries[n].id = iv->getID();
      input_entries[n].value_node = (void*)iv;
      return *iv;
    } else {
      return *(InputVar<input_type>*)input_entries[n].value_node; // Sketchy, but oh well
    }
  }
  
  template<SpurvShaderType type, typename... InputTypes>
  template<int n, int curr, typename First, typename... Rest>
  auto& SpurvShader<type, InputTypes...>::getInputVariable() {
    using input_type = First;
    static_assert(is_spurv_type<input_type>::value, "Supplied input values must be SpurvType");

    if constexpr(n > curr) {
	return getInputVariable<n, curr + 1, Rest...>();
      } else {
      return getInputVariable<n, First>();
    }
  }

  template<SpurvShaderType type, typename... InputTypes>
  void SpurvShader<type, InputTypes...>::output_main_function_begin(std::vector<uint32_t>& res) {
    SpurvType<SPURV_TYPE_VOID>::ensure_defined(res, this->defined_type_ids);

    int void_function_type = this->get_new_value_id();
    
    // OpTypeFunction <result_id> <result type> <result_id> 
    add(res, (3 << 16) | 33);
    add(res, void_function_type);
    add(res, SpurvType<SPURV_TYPE_VOID>::getID());

    // OpFunction <result type> <result_id> <function_control> <function_type>
    add(res, (5 << 16) | 54);
    add(res, SpurvType<SPURV_TYPE_VOID>::getID());
    add(res, entry_point_id);
    add(res, 0);
    add(res, void_function_type);

    // OpLabel <result_id>
    add(res, (2 << 16) | 248);
    add(res, this->get_new_value_id());
    
  }

  template<SpurvShaderType type, typename... InputTypes>
  void SpurvShader<type, InputTypes...>::output_main_function_end(std::vector<uint32_t>& res) {
    // OpReturn
    add(res, (1 << 16) | 253);

    // OpFunctionEnd
    add(res, (1 << 16) | 56);
  }

  template<SpurvShaderType type, typename... InputTypes>
  template<typename tt, typename... NodeTypes>
  void SpurvShader<type, InputTypes...>::output_output_definitions(std::vector<uint32_t>& res, int n,
								   ValueNode<tt>& node, NodeTypes... args) {
    node.define(res);

    // OpStore
    add(res, (3 << 16) | 62);
    add(res, this->output_pointer_ids[n]);
    add(res, node.getID());

    this->output_output_definitions(res, n + 1, args...);
  }

  template<SpurvShaderType type, typename... InputTypes>
  template<typename tt>
  void SpurvShader<type, InputTypes...>::output_output_definitions(std::vector<uint32_t>& res, int n,
								   ValueNode<tt>& node) {
    node.define(res);

    // OpStore
    add(res, (3 << 16) | 62);
    add(res, this->output_pointer_ids[n]);
    add(res, node.getID());
  }
  

  template<SpurvShaderType type, typename... InputTypes>
  template<int n>
  void SpurvShader<type, InputTypes...>::output_input_pointers(std::vector<uint32_t>& res) {
    return;
  }
      
  template<SpurvShaderType type, typename... InputTypes>
  template<int n, typename CurrInput, typename... RestInput>
  void SpurvShader<type, InputTypes...>::output_input_pointers(std::vector<uint32_t>& res) {
    SpurvPointer<SPURV_STORAGE_INPUT, CurrInput>::ensure_defined(res, this->defined_type_ids);

    // OpVariable ...
    add(res, (4 << 16) | 59);
    add(res, SpurvPointer<SPURV_STORAGE_INPUT, CurrInput>::getID());
    add(res, this->input_entries[n].pointer_id);
    add(res, SPURV_STORAGE_INPUT);
      
    this->output_input_pointers<n + 1, RestInput...>(res);
  }

  template<SpurvShaderType type, typename... InputTypes>
  template<typename tt, typename... NodeTypes>
  void SpurvShader<type, InputTypes...>::output_output_pointers(std::vector<uint32_t>& res, int n,
								ValueNode<tt>& val, NodeTypes... args) {
    SpurvPointer<SPURV_STORAGE_OUTPUT, tt>::ensure_defined(res, this->defined_type_ids);

    printf("Outputting output pointer with id %d\n", this->output_pointer_ids[n]);
    
    // OpVariable...
    add(res, (4 << 16) | 59);
    add(res, SpurvPointer<SPURV_STORAGE_OUTPUT, tt>::id);
    add(res, this->output_pointer_ids[n]);
    add(res, SPURV_STORAGE_OUTPUT);

    this->output_output_pointers(res, n + 1, args...);
  }

  template<SpurvShaderType type, typename... InputTypes>
  template<typename tt>
  void SpurvShader<type, InputTypes...>::output_output_pointers(std::vector<uint32_t>& res, int n, ValueNode<tt>& val) {
    SpurvPointer<SPURV_STORAGE_OUTPUT, tt>::ensure_defined(res, this->defined_type_ids);
    printf("Outputting output pointer with id %d\n", this->output_pointer_ids[n]);
    
    // OpVariable...
    add(res, (4 << 16) | 59);
    add(res, SpurvPointer<SPURV_STORAGE_OUTPUT, tt>::getID());
    add(res, this->output_pointer_ids[n]);
    add(res, SPURV_STORAGE_OUTPUT);
  }

  template<SpurvShaderType type, typename... InputTypes>
  void SpurvShader<type, InputTypes...>::output_used_builtin_pointers(std::vector<uint32_t>& res){
    if(this->builtin_vec4_0) {
      SpurvPointer<SPURV_STORAGE_OUTPUT, vec4_s>::ensure_defined(res, this->defined_type_ids);
      
      // OpVariable
      add(res, (4 << 16) | 59);
      add(res, SpurvPointer<SPURV_STORAGE_OUTPUT, vec4_s>::getID());
      add(res, this->builtin_vec4_0->pointer_id);
      add(res, SPURV_STORAGE_OUTPUT);
    }
    if(this->builtin_float_0) {
      SpurvPointer<SPURV_STORAGE_OUTPUT, float_s>::ensure_defined(res, this->defined_type_ids);
      
      // OpVariable
      add(res, (4 << 16) | 59);
      add(res, SpurvPointer<SPURV_STORAGE_OUTPUT, float_s>::getID());
      add(res, this->builtin_float_0->pointer_id);
      add(res, SPURV_STORAGE_OUTPUT);
    }
    if(this->builtin_arr_1_float_0) {
      SpurvPointer<SPURV_STORAGE_OUTPUT, arr_1_float_s>::ensure_defined(res, this->defined_type_ids);
      
      // OpVariable
      add(res, (4 << 16) | 59);
      add(res, SpurvPointer<SPURV_STORAGE_OUTPUT, arr_1_float_s>::getID());
      add(res, this->builtin_arr_1_float_0->pointer_id);
      add(res, SPURV_STORAGE_OUTPUT);
    }
    if(this->builtin_arr_1_float_1) {
      SpurvPointer<SPURV_STORAGE_OUTPUT, arr_1_float_s>::ensure_defined(res, this->defined_type_ids);
      
      // OpVariable
      add(res, (4 << 16) | 59);
      add(res, SpurvPointer<SPURV_STORAGE_OUTPUT, arr_1_float_s>::getID());
      add(res, this->builtin_arr_1_float_1->pointer_id);
      add(res, SPURV_STORAGE_OUTPUT);
    }
  }
  
  template<SpurvShaderType type, typename... InputTypes>
  void SpurvShader<type, InputTypes...>::output_builtin_output_definitions(std::vector<uint32_t>& res) {
    if(this->builtin_vec4_0) {
      this->builtin_vec4_0->value_node->define(res);
      
      // OpStore
      add(res, (3 << 16) | 62);
      add(res, this->builtin_vec4_0->pointer_id);
      add(res, this->builtin_vec4_0->value_node->getID());
      
    }
    if(this->builtin_float_0) {
      this->builtin_float_0->value_node->define(res);
      
      // OpStore
      add(res, (3 << 16) | 62);
      add(res, this->builtin_float_0->pointer_id);
      add(res, this->builtin_float_0->value_node->getID());
    }
    if(this->builtin_arr_1_float_0) {
      this->builtin_arr_1_float_0->value_node->define(res);
      
      // OpStore
      add(res, (3 << 16) | 62);
      add(res, this->builtin_arr_1_float_0->pointer_id);
      add(res, this->builtin_arr_1_float_0->value_node->getID());
    }
    if(this->builtin_arr_1_float_1) {
      this->builtin_arr_1_float_1->value_node->define(res);
      
      // OpStore
      add(res, (3 << 16) | 62);
      add(res, this->builtin_arr_1_float_1->pointer_id);
      add(res, this->builtin_arr_1_float_1->value_node->getID());
    }
  }
  
  template<SpurvShaderType type, typename...InputTypes>
  void SpurvShader<type, InputTypes...>::cleanup_ids() {
    for(uint i = 0; i < this->defined_type_ids.size(); i++) {
      *(this->defined_type_ids[i]) = -1;
    }
  }
  
  template<SpurvShaderType type, typename... InputTypes>
  template<typename... NodeTypes>
  void SpurvShader<type, InputTypes...>::compileToSpirv(std::vector<uint32_t>& res, NodeTypes&... args) {

    this->output_preamble(res);
    this->output_shader_header_begin(res);
    constexpr int num_args = sizeof...(NodeTypes);

    res[this->entry_point_declaration_size_index] |= ( 3 + 2 + this->input_entries.size() + num_args + get_num_defined_builtins()) << 16;

    for(int i = 0; i < num_args; i++) {
      this->output_pointer_ids.push_back(this->get_new_value_id());
    }
    
    this->output_shader_header_output_variables(res, 0, args...);

    this->output_shader_header_decorate_begin(res);

    this->output_shader_header_decorate_output_variables(res, 0, args...);

    this->output_type_definitions(res, args...);

    this->output_input_pointers<0, InputTypes...>(res);

    this->output_output_pointers(res, 0, args...);

    this->output_used_builtin_pointers(res);

    this->output_main_function_begin(res);
        
    this->output_output_definitions(res, 0, args...);
    this->output_builtin_output_definitions(res);

    this->output_main_function_end(res);

    res[this->id_max_bound_index] = global_id_counter;

    this->cleanup_ids();

    global_id_counter = 1;
  }
  
};

#endif // ndef INCLUDED_SPURV_HPP
