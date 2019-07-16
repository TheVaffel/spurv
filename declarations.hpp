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
   * Classes - declared to ease dependency graph
   */

  struct DSpurvType;
  
  template<SpurvTypeKind kind, int arg0, int arg1, typename... InnerTypes>
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

  class SpurvUniformBindingBase;

  template<typename... InnerTypes>
  class SpurvUniformBinding;

  template<typename tt>
  class UniformVar;

  template<typename... InnerTypes>
  class SpurvStruct;

  struct TypeDeclarationState;
  
  class ConstantRegistry;
  
  class Utils;
  
};
#endif // __SPURV_DECLARATIONS
