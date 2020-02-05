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
    KIND_POINTER,
    KIND_STRUCT,
    KIND_IMAGE,
    KIND_TEXTURE
  };
  
  enum class SShaderType {
    SHADER_VERTEX,
    SHADER_FRAGMENT,
  };

  enum SStorageClass {
    STORAGE_UNIFORM_CONSTANT = 0,
    STORAGE_INPUT = 1,
    STORAGE_UNIFORM = 2,
    STORAGE_OUTPUT = 3,
    STORAGE_FUNCTION = 7,
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
    EXPR_LOOKUP, // Sampling, indexing etc..
    EXPR_EQUAL,
    EXPR_NOTEQUAL,
    EXPR_LESSTHAN,
    EXPR_GREATERTHAN,
    EXPR_LESSOREQUAL,
    EXPR_GREATEROREQUAL
  };

   enum SBuiltinVariable {
    BUILTIN_POSITION,
    BUILTIN_POINT_SIZE,
    BUILTIN_CLIP_DISTANCE,
    BUILTIN_CULL_DISTANCE
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

  template<int n>
  class SFloat;

  template<int n, int s>
  class SInt;

  template<int n, int m>
  class SMat;

  template<int n, typename inner>
  class SArr;

  class SBool;

  template<SStorageClass n, typename inn>
  class SPointer;
  
  template<typename tt>
  class SValue;

  template<typename tt, SExprOp op,
	   typename t1, typename t2>
  class SExpr;

  class SUniformBindingBase;

  template<typename... InnerTypes>
  class SUniformBinding;

  template<typename tt>
  class SUniformVar;

  template<typename... InnerTypes>
  class SStruct;

  template<typename tt>
  class Constant;
  
  template<int n, int m>
  class ConstructMatrix;

  template<typename tt>
  class SelectConstruct;
  
  class SDeclarationState;
  
  class SConstantRegistry;
  
  class SUtils;
  
};
#endif // __SPURV_DECLARATIONS
