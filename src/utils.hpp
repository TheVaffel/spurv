#ifndef __SPURV_UTILS
#define __SPURV_UTILS

#include "declarations.hpp"

#include <vector>
#include <cstdint>
#include <string>

namespace spurv {

  /*
   * Static util class for Spurv
   */
  
  class SUtils {
    static int getNewID();
    static int getCurrentID();
    static void resetID();

    static void add(std::vector<uint32_t>& res, int a);
    static void add(std::vector<uint32_t>& binary, std::string str);
    static int stringWordLength(std::string);

    template<typename First, typename... Types>
    static void ensureDefinedRecursive(std::vector<uint32_t>& bin,
				       std::vector<SDeclarationState*>& declaration_states);
    
    template<typename First, typename... Types>
    static void addIDsRecursive(std::vector<uint32_t>& bin);

    template<typename tt>
    struct num_elements;

    template<typename... Types>
    struct sum_num_elements;

    template<typename First, typename... Types>
    static constexpr bool has_only_1_comps(First&& ft, Types&&... args);

    template<typename First, typename... Types>
    static constexpr bool has_only_n_comps(int n, First&& ft, Types&&... args);
    
    template<typename First, typename... Types>
    static constexpr bool isSTypeRecursive();

    template<typename First, typename... Types>
    static void getDSTypesRecursive(DSType *pp);

    struct PWrapperBase {
      virtual void exterminate() = 0;
    };

    template<typename tt>
    struct PWrapper : public PWrapperBase {
      tt* pp;
      virtual void exterminate();
    };
    
    static std::vector<PWrapperBase*> allocated_values;
    
    static void clearAllocations();
    

    template<typename First, typename... Types>
    static constexpr int getSumSize();

    
    SUtils() = delete;
    
    static int global_id_counter;

    static int GLSL_id;

    static int getGLSLID();
    static void setGLSLID(int id);

    template<STypeKind kind, int n, int m, int l, int k, int j, typename... InnerTypes>
    friend class SType;
    
    template<SShaderType type, typename... InputTypes>
    friend class SShader;

    friend class SBool;
    
    template<int n>
    friend class SFloat;

    template<int n, int s>
    friend class SInt;

    template<int n, int m, typename inner>
    friend class SMat;

    template<int n, SStorageClass storage, typename inner>
    friend class SArr;

    template<SStorageClass storage, typename inner>
    friend class SRunArr;

    template<SStorageClass n, typename inn>
    friend class SPointer;

    template<typename tt>
    friend class SValue;
    
    template<typename tt, SExprOp ex, typename tt2, typename tt3>
    friend class SExpr;

    template<typename tt>
    friend class Constant;

    template<typename tt>
    friend class InputVar;

    friend class SUniformBindingBase;

    friend class SConstantRegistry;

    template<SStorageClass storage, typename... InnerTypes>
    friend class SStructBinding;
    
    template<typename... InnerTypes>
    friend class SUniformBinding;

    template<typename tt>
    friend class SUniformConstant;

    template<SStorageClass storage, typename tt>
    friend class SUniformVar;

    template<SDecoration decor, typename... InnerTypes>
    friend class SStruct;

    template<int dims, int depth, int arrayed, int multisamp, int sampled>
    friend class SImage;
    
    template<int d>
    friend class STexture;

    template<int n, int m, typename inner>
    friend class ConstructMatrix;

    template<typename tt>
    friend class SelectConstruct;

    friend class SPointerBase;

    template<typename tt, SStorageClass storage>
    friend class SPointerVar;

    template<typename tt, SStorageClass storage>
    friend class SAccessChain;

    template<typename tt>
    friend class SLocal;
    
    template<typename tt, SStorageClass stind>
    friend class SLoadedVal;

    template<typename tt>
    friend class SGLSLHomoFun;

    friend class SEventRegistry;

    template<typename tt>
    friend class SLoadEvent;

    template<typename tt>
    friend class SStoreEvent;

    template<typename tt>
    friend class SImageStoreEvent;

    friend class SIfThen;
    
    friend class SForBeginEvent;
    
    friend class SForEndEvent;
    
    friend class SForLoop;
    
  public:

    // So that it can be accessed by global operaters
    template<typename tt, typename... Types>
    static tt* allocate(Types&&... args);
  
    // Utility to get nth type from a parameter pack of types
    template<int n, typename...Types>
    struct NthType;

    static void binaryPrettyPrint(const std::vector<uint32_t>& bin);

  };

};
#endif // __SPURV_UTILS
