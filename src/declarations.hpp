#ifndef __SPURV_DECLARATIONS
#define __SPURV_DECLARATIONS

#include <vector>
#include <cstdint> // uint32_t
#include <string>
#include <map>

namespace spurv {

  /*
   * Enums
   */
  
   enum class STypeKind {
    KIND_INVALID,
    KIND_BOOL,
    KIND_VOID,
    KIND_INT,
    KIND_FLOAT,
    KIND_MAT,
    KIND_ARR,
    KIND_RUN_ARR,
    KIND_POINTER,
    KIND_STRUCT,
    KIND_IMAGE,
    KIND_TEXTURE
  };
  
  enum class SShaderType {
    SHADER_VERTEX,
    SHADER_FRAGMENT,
    SHADER_COMPUTE,
  };

  enum SStorageClass {
    STORAGE_UNIFORM_CONSTANT = 0,
    STORAGE_INPUT = 1,
    STORAGE_UNIFORM = 2,
    STORAGE_OUTPUT = 3,
    STORAGE_FUNCTION = 7,
    STORAGE_STORAGE_BUFFER = 12,
    // ... There are more, but perhaps not needed just now
  };
  
  enum SExprOp {
    EXPR_NEGATIVE,
    EXPR_ADDITION,
    EXPR_SUBTRACTION,
    EXPR_MULTIPLICATION,
    EXPR_DIVISION,
    EXPR_DOT,
    EXPR_CROSS,
    EXPR_EXP,
    EXPR_SQRT,
    EXPR_POW,
    EXPR_MOD,
    EXPR_REM,
    EXPR_LOOKUP, // Sampling, indexing etc..
    EXPR_CAST,
    EXPR_EQUAL,
    EXPR_NOTEQUAL,
    EXPR_LESSTHAN,
    EXPR_GREATERTHAN,
    EXPR_LESSOREQUAL,
    EXPR_GREATEROREQUAL
  };

   enum SBuiltinVariable {
    BUILTIN_POSITION = 0,
    BUILTIN_POINT_SIZE = 1,
    BUILTIN_CLIP_DISTANCE = 3,
    BUILTIN_CULL_DISTANCE = 4,
    BUILTIN_INSTANCE_INDEX = 43,
    BUILTIN_VERTEX_INDEX = 42,
    BUILTIN_FRAG_COORD = 15,
    BUILTIN_GLOBAL_INVOCATION_ID = 28
  };

  enum SControlType {
    CONTROL_IF,
    CONTROL_FOR
  };
  
  enum SExtension {
    EXTENSION_STORAGE_BUFFER = 0,
    EXTENSION_END
  };


  // These are not implemented functions, but a list of
  // easily implementable ones. They may be implemented
  // at a later point
  enum GLSLFunction {
    GLSL_ROUND = 1,
    GLSL_ROUND_EVEN,
    GLSL_TRUNC,
    GLSL_FABS,
    GLSL_SABS,
    GLSL_FSIGN,
    GLSL_SSIGN,
    GLSL_FLOOR,
    GLSL_CEIL,
    GLSL_FRACT,
    GLSL_RADIANS,
    GLSL_DEGREES,
    GLSL_SIN,
    GLSL_COS,
    GLSL_TAN,
    GLSL_ASIN,
    GLSL_ACOS,
    GLSL_ATAN,
    GLSL_SINH,
    GLSL_COSH,
    GLSL_TANH,
    GLSL_ASINH,
    GLSL_ACOSH,
    GLSL_ATANH,
    GLSL_ATAN2,
    GLSL_POW,
    GLSL_EXP,
    GLSL_LOG,
    GLSL_EXP2,
    GLSL_LOG2,
    GLSL_SQRT,
    GLSL_INVSQRT,
    GLSL_FMIN = 37,
    GLSL_UMIN,
    GLSL_SMIN,
    GLSL_FMAX,
    GLSL_UMAX,
    GLSL_SMAX,
    GLSL_CROSS = 68,
    GLSL_NORMALIZE,
    GLSL_REFLECT = 71,
  };

  enum class SDecoration {
			  NONE = 0,
			  BLOCK = 1,
			  // Later decorations could include alignments etc.
  };
  

  /*
   * Classes - declared to ease dependency graph
   */

  struct DSType;

  // I am really sorry for this. Do tell if you know a better way
  template<STypeKind kind, int arg0 = 0, int arg1 = 0,
	   int arg2 = 0, int arg3 = 0, int arg4 = 0, typename... InnerTypes>
  class SType;

  template<SShaderType type, typename... InputTypes>
  class SShader;

  struct IOVariableBase;

  template<int n>
  class SFloat;

  template<int n, int s>
  class SInt;

  template<int n, int m, typename inner>
  class SMat;

  template<int n, SStorageClass str, typename inner>
  class SArr;

  template<SStorageClass str, typename inner>
  class SRunArr;

  class SBool;

  template<SStorageClass n, typename inn>
  class SPointer;
  
  template<typename tt>
  class SValue;

  template<typename tt, SExprOp op,
	   typename t1, typename t2>
  class SExpr;

  class SUniformBindingBase;

  template<SStorageClass storage, typename... InnerTypes>
  class SStructBinding;
  
  template<typename... InnerTypes>
  class SUniformBinding;

  template<SStorageClass storage, typename tt>
  class SUniformVar;

  template<SDecoration decoration, typename... InnerTypes>
  class SStruct;

  template<typename tt>
  class SLoadEvent;

  template<typename tt>
  class SStoreEvent;

  template<typename tt>
  class SLocal;

  template<typename tt>
  class Constant;
  
  template<int n, int m, typename inner>
  class ConstructMatrix;

  template<typename tt>
  class SelectConstruct;
  
  class SDeclarationState;
  
  class SConstantRegistry;

  class SIfThen;
  
  class SForLoop;

  class SVariableRegistry;

  class SVariableEntryBase;

  template<typename tt>
  class SVariableEntry;

  class SPointerBase;
  
  template<typename tt>
  class SPointerTypeBase;
  
  template<typename tt, SStorageClass stind>
  class SPointerVar;

  template<typename tt, SStorageClass stind>
  class SAccessChain;
  
  template<typename tt, SStorageClass stind>
  class SLoadedVal;

  
  class SUtils;
  
};
#endif // __SPURV_DECLARATIONS
