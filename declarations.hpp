#ifndef __SPURV_DECLARATIONS
#define __SPURV_DECLARATIONS

#include <vector>
#include <cstdint> // uint32_t
#include <string>

namespace spurv {

  /*
   * Enums
   */
  
   enum SpurvTypeKind {
    SPURV_TYPE_INVALID,
    SPURV_TYPE_VOID,
    SPURV_TYPE_INT,
    SPURV_TYPE_FLOAT,
    SPURV_TYPE_MAT,
    SPURV_TYPE_ARR,
    SPURV_TYPE_POINTER,
    SPURV_TYPE_STRUCT
  };
  
  enum SpurvShaderType {
    SPURV_SHADER_VERTEX,
    SPURV_SHADER_FRAGMENT,
  };

  enum SpurvStorageClass {
    SPURV_STORAGE_UNIFORM_CONSTANT = 0,
    SPURV_STORAGE_INPUT = 1,
    SPURV_STORAGE_UNIFORM = 2,
    SPURV_STORAGE_OUTPUT = 3
    // ... There are more, but perhaps not needed just now
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

   enum BuiltinVariableIndex {
    BUILTIN_POSITION,
    BUILTIN_POINT_SIZE,
    BUILTIN_CLIP_DISTANCE,
    BUILTIN_CULL_DISTANCE
  };

  
  /*
   * Classes
   */

  struct DSpurvType;
  
  template<SpurvTypeKind kind, int arg0, int arg1, typename inner_type>
  class SpurvType;

  template<SpurvShaderType type, typename... InputTypes>
  class SpurvShader;

  template<int n>
  class SpurvFloat;

  template<int n, int s>
  class SpurvInt;

  template<int n, int m>
  class SpurvMat;

  template<int n, typename inner>
  class SpurvArr;

  template<SpurvStorageClass n, typename inn>
  class SpurvPointer;
  
  template<typename tt>
  struct ValueNode;

  template<typename tt, ExpressionOperation op,
	   typename t1, typename t2>
  struct Expr;
  
  /*
   * Static util class for Spurv
   */
  class Utils {
    
    static int getNewID();
    static int getCurrentID();
    static void resetID();

    static void add(std::vector<uint32_t>& res, int a);
    static void add(std::vector<uint32_t>& binary, std::string str);
    static int stringWordLength(std::string);

    template<typename First, typename... Types>
    static void ensureDefinedRecursive(std::vector<uint32_t>& bin,
				       std::vector<int*>& ids);
    
    template<typename First, typename... Types>
    static void addIDsRecursive(std::vector<uint32_t>& bin);

    template<typename First, typename... Types>
    static constexpr bool isSpurvTypeRecursive();

    template<typename First, typename... Types>
    static void getDSpurvTypesRecursive(DSpurvType *pp);
    
    template<int n, typename...Types>
    struct NthType;
    
    Utils() = delete;
    
    static int global_id_counter;

    template<SpurvTypeKind kind, int n, int m, typename inner>
    friend class SpurvType;
    
    template<SpurvShaderType type, typename... InputTypes>
    friend class SpurvShader;

    template<int n>
    friend class SpurvFloat;

    template<int n, int s>
    friend class SpurvInt;

    template<int n, int m>
    friend class SpurvMat;

    template<int n, typename inner>
    friend class SpurvArr;

    template<SpurvStorageClass n, typename inn>
    friend class SpurvPointer;

    template<typename tt>
    friend class ValueNode;
    
    template<typename tt, ExpressionOperation ex, typename tt2, typename tt3>
    friend class Expr;

    template<typename tt>
    friend class Constant;

    template<typename tt>
    friend class InputVar;
    
  };


  /*
   * NullType
   */
  
  class NullType {
  public:
    static void ensure_defined(std::vector<uint32_t>&,
			       std::vector<int32_t*>&);
    static void define(std::vector<uint32_t>&);
  };
  
  
  /*
   * DSpurvType - Dynamic (runtime) representation of a SpurvType
   */
  
  struct DSpurvType {
    // NB: If adding more parameters, remember to add more arguments to SpurvType and
    // is_spurv_type below as well
    SpurvTypeKind kind;
    int a0, a1;
    
    // NB: This will be a list if kind == SPURV_TYPE_STRUCT
    DSpurvType* inner_type;
    
    constexpr DSpurvType() : kind(SPURV_TYPE_INVALID),
      a0(0), a1(0), inner_type(nullptr) { }

    ~DSpurvType() ;
    
    bool operator==(const DSpurvType& ds) const;
  };


  /*
   * SpurvType - The mother of them all
   */
  
  template<SpurvTypeKind kind, int arg0 = 0, int arg1 = 0, typename inner_type = NullType>
  class SpurvType {
  protected:
    static int id;
  public:
    char name[32];
    constexpr SpurvType() {
      static_assert(kind == SPURV_TYPE_VOID); id = -1;
    }

    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<int*>& ids);
    static void ensure_defined_dependencies(std::vector<uint32_t>& bin, std::vector<int*>& ids);
    static void define(std::vector<uint32_t>& bin);
    
    static void getDSpurvType(DSpurvType* type);

    static int getID();
  };

  /*
   * SpurvInt - Representation of integers
   */

  template<int n, int signedness>
  class SpurvInt : public SpurvType<SPURV_TYPE_INT, n, signedness> {
  public:
    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<int*>& ids);
    static void define(std::vector<uint32_t>& bin);
  };
  

  /*
   * SpurvFloat - Representation of floats
   */
  
  template<int n>
  class SpurvFloat : public SpurvType<SPURV_TYPE_FLOAT, n> {
  public:
    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<int*>& ids);
    static void define(std::vector<uint32_t>& bin);
  };
  

  /*
   * SpurvMat - Representation of vectors and matrices
   */
  
  template<int n, int m>
  class SpurvMat : public SpurvType<SPURV_TYPE_MAT, n, m> {
  public:
    static void ensure_defined_dependencies(std::vector<uint32_t>& bin,
					    std::vector<int*>& ids);
    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<int*>& ids);
    static void define(std::vector<uint32_t>& bin);
  };

  
  /*
   * Static explicit type checks - more general (more checks below)
   */
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


  /*
   * SpurvArr - Representation of arrays
   */
  template<int n, typename tt>
  class SpurvArr : public SpurvType<SPURV_TYPE_ARR, n, 0, tt >  {
    constexpr SpurvArr() {
      static_assert(is_spurv_type<tt>::value, "Inner type of SpurvArr must be a spurv type");
    }

  public:    
    static void ensure_defined_dependencies(std::vector<uint32_t>& bin,
					    std::vector<int*>& ids);
    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<int*>& ids);
    static void define(std::vector<uint32_t>& bin);
  };

  /*
   * SpurvPointer - Representation of pointers
   */
  template<SpurvStorageClass storage, typename tt>
  class SpurvPointer : public SpurvType<SPURV_TYPE_POINTER, (int)storage, 0, tt> {
  public:
    static void ensure_defined_dependencies(std::vector<uint32_t>& bin,
					    std::vector<int*>& ids);
    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<int*>& ids);
    static void define(std::vector<uint32_t>& bin);
  };


  /*
   * SpurvStruct - Representation of structs
   */
  
  template<typename... InnerTypes>
  class SpurvStruct : public SpurvType<SPURV_TYPE_STRUCT, 0, 0, NullType> {
  public:
    static void ensure_defined_dependencies(std::vector<uint32_t>& bin,
					    std::vector<int*>& ids);
    static void ensure_defined(std::vector<uint32_t>& bin, std::vector<int*>& ids);
    static void define(std::vector<uint32_t>& bin);
    static void getDSpurvType(DSpurvType* type);
  };

    
  /*
   * More specialized type checks
   */
  template<typename>
  struct is_spurv_mat_type : std::false_type {};
  
  template<int n, typename tt>
  struct is_spurv_type<SpurvArr<n, tt> > : std::true_type { static_assert(is_spurv_type<tt>::value); };

  template<SpurvStorageClass storage, typename tt>
  struct is_spurv_type<SpurvPointer<storage, tt> > : std::true_type { static_assert(is_spurv_type<tt>::value); };

  template<typename... InnerTypes>
  struct is_spurv_type<SpurvStruct<InnerTypes...> > : std::true_type{ static_assert(Utils::isSpurvTypeRecursive<InnerTypes...>); };

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
  

  /*
   * Small type mapper
   */
  template<typename tt>
  struct MapSpurvType;


  /*
   * ValueNode - The mother of all nodes in the syntax trees
   */
  
  template<typename tt>
  struct ValueNode {
    static_assert(is_spurv_type<tt>::value);
  protected:
    uint id;
    bool defined;
    uint ref_count;
  public:

    ValueNode();
    
    virtual void print_nodes_post_order(std::ostream& str) const;

    int getID() const;
    void declareDefined();
    bool isDefined() const;
    void incrementRefCount();
    
    virtual void define(std::vector<uint32_t>& res) = 0;

    virtual void ensure_type_defined(std::vector<uint32_t>& res, std::vector<int32_t*>& ids);
      
    // Only deletes tree if ref_count reaches zero
    virtual void unref_tree() = 0;
  };


  /*
   * Constant - Represents nodes that have known value at shader compilation time
   */
  template<typename tt>
  struct Constant : public ValueNode<typename MapSpurvType<tt>::type> {
    Constant(const tt& val);

    virtual void unref_tree();

    virtual void define(std::vector<uint32_t>& res);
    
    tt value;
  };


  /*
   * Var - Base class for input attributes and uniforms
   */

  template<typename tt>
  struct Var : public ValueNode<tt> {
    std::string name;
    Var();
    Var(std::string _name);
    virtual void unref_tree();
  };


  /*
   * UniformVar - Represents uniforms (duh)
   */
  
  template<typename tt>
  struct UniformVar : public Var<tt> {
    int set_no, bind_no;
    
    UniformVar(int s, int b) ;
  };


  /*
   * InputVar - Represents input attributes
   */
  
  template<typename tt>
  struct InputVar : public Var<tt> {
    int input_no;
    int pointer_id;
    
    InputVar(int n, int pointer_id);

    virtual void define(std::vector<uint32_t>& res);
  };


  /*
   * Types defined by default
   */
  
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

  
  /*
   * Expr - Nodes that represent branches of the syntax tree
   */
  template<typename tt, ExpressionOperation op, typename tt2 = void_s, typename tt3 = void_s>
  struct Expr : public ValueNode<tt> {
    static_assert(is_spurv_type<tt>::value);
    static_assert(is_spurv_type<tt2>::value);
    static_assert(is_spurv_type<tt3>::value);
    
    // Enforce Expr to be at most binary (surely, this won't come back and bite me later...)
    ValueNode<tt2>* v1;
    ValueNode<tt3>* v2;

    Expr();

    void register_left_node(ValueNode<tt2>& node);
    void register_right_node(ValueNode<tt3>& node);

    // Since expressions are returned as references from e.g.
    // binary operations, if you assign a normal (non-reference) variable to an expression resulting
    // from such an operation, it will (if I understand correctly) call the copy constructor, and thus
    // we get a new object put on the stack, which we should not delete, although all its children are
    // heap-allocated, so they should be deleted. Also, this means the top-most node gets no references
    // and is lost in the void
    // Thus, we avoid copies
    Expr(const Expr<tt, op, tt2, tt3>& e) = delete;

    virtual void print_nodes_post_order(std::ostream& str) const ;
    virtual void unref_tree();
    virtual void ensure_type_defined(std::vector<uint32_t>& res, std::vector<int32_t*>& ids);
    virtual void define(std::vector<uint32_t>& res);
  };

  
  /*
   * SpurvShader - The object responsible for IO and compilation of the shader
   */
  
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
      
      InputVariableEntry();
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
    void output_shader_header_end(std::vector<uint32_t>& binary);
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

  
};
#endif // __SPURV_DECLARATIONS
