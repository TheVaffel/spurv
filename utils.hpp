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
    
    template<int n, typename...Types>
    struct NthType;

    template<typename First, typename... Types>
    constexpr int getSumSize();
    
    SUtils() = delete;
    
    static int global_id_counter;

    template<STypeKind kind, int n, int m, typename... InnerTypes>
    friend class SType;
    
    template<SShaderType type, typename... InputTypes>
    friend class SShader;

    template<int n>
    friend class SFloat;

    template<int n, int s>
    friend class SInt;

    template<int n, int m>
    friend class SMat;

    template<int n, typename inner>
    friend class SArr;

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

    template<typename... InnerTypes>
    friend class SUniformBinding;

    template<typename tt>
    friend class SUniformVar;

    template<typename... InnerTypes>
    friend class SStruct;

  public:

    // So that it can be accessed by global operaters
    template<typename tt, typename... Types>
    static tt* allocate(Types... args);
  
    
  };

};
#endif // __SPURV_UTILS
