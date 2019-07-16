#ifndef __SPURV_UTILS
#define __SPURV_UTILS

namespace spurv {

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
				       std::vector<TypeDeclarationState*>& declaration_states);
    
    template<typename First, typename... Types>
    static void addIDsRecursive(std::vector<uint32_t>& bin);

    template<typename First, typename... Types>
    static constexpr bool isSpurvTypeRecursive();

    template<typename First, typename... Types>
    static void getDSpurvTypesRecursive(DSpurvType *pp);
    
    template<int n, typename...Types>
    struct NthType;

    template<typename First, typename... Types>
    constexpr int getSumSize();
    
    Utils() = delete;
    
    static int global_id_counter;

    template<SpurvTypeKind kind, int n, int m, typename... InnerTypes>
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

    friend class SpurvUniformBindingBase;

    friend class ConstantRegistry;

    template<typename... InnerTypes>
    friend class SpurvUniformBinding;

    template<typename tt>
    friend class UniformVar;

    template<typename... InnerTypes>
    friend class SpurvStruct;
    
  };

};
#endif // __SPURV_UTILS
