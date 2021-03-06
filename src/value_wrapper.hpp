#ifndef __SPURV_VALUE_WRAPPER
#define __SPURV_VALUE_WRAPPER

#include "declarations.hpp"
#include "types.hpp"

#ifdef WIN32
#define BOOL_CONCEPT concept
#else // for GCC
#define BOOL_CONCEPT concept bool constexpr
#endif

namespace spurv {

  
    
  /*
   * RequireOneSpurvValue - A concept requiring one Spurv type among
   * the two provided types
   */
  
  template<typename S, typename T>
  BOOL_CONCEPT RequireOneSpurvValue =
    (is_spurv_value<typename std::remove_reference<S>::type>::value ||
     is_spurv_value<typename std::remove_reference<T>::type>::value);

  template<typename S, typename T>
  // requires !RequireOneSpurvValue<S, T>
  BOOL_CONCEPT SameSpurvTypeNonSpurv = !(is_spurv_value<typename std::remove_reference<S>::type>::value ||
      is_spurv_value<typename std::remove_reference<T>::type>::value) &&
      (std::is_same<typename MapSType<typename std::remove_reference<S>::type>::type,
					    typename MapSType<typename std::remove_reference<T>::type>::type>::value);

  
  /*
   * Concept seeing if two spurv values are the same
   */
  
  template<typename S, typename T>
  BOOL_CONCEPT SameSpurvType =
    is_spurv_type<S>::value && is_spurv_type<T>::value && std::is_same<S, T>::value;
  
  /*
   * RequireUnambiguousUnwrappable - A concept requiring the two arguments to be
   * of the same type and having an unambiguous unwrapped type
   */

  template<typename S, typename T>
  BOOL_CONCEPT RequireUnambiguousUnwrappable =
    (RequireOneSpurvValue<S, T> || SameSpurvType<S, T>);
  

  /*
   * Conceptification of not is_spurv_value
   */
  
  template<typename S>
  BOOL_CONCEPT NoSpurvValue =
    !is_spurv_value<typename std::remove_reference<S>::type>::value;
    

  /*
   * Concept seeing if first is a spurv value of the second type
   */
  
  template<typename S, typename T>
  BOOL_CONCEPT IsSpurvValueOf =
    is_spurv_type<T>::value &&
    is_spurv_value<typename std::remove_reference<S>::type>::value &&
    std::is_base_of<SValue<T>, typename std::remove_reference<S>::type>::value;
  

  /*
   * Concept checking whether the first argument can be cast to the second type
   */
  
  template<typename S, typename T>
  BOOL_CONCEPT IsSpurvCastable =
    is_spurv_value<typename std::remove_reference<S>::type>::value &&
    is_spurv_castable<typename std::remove_reference<S>::type::type,
		      typename std::remove_reference<T>::type>::value;

  
  /*
   * Some utils used in expression_impl and values_impl
   */

  template<typename tt1, typename tt2>
  struct get_spurv_value { using type = typename std::conditional<is_spurv_value<tt1>::value,
								  tt1, tt2>::type; };

  // Just returns the second type if the first is a spurv value
  template<typename tt1, typename tt2>
  struct get_not_spurv_value { using type = typename std::conditional<is_spurv_value<tt1>::value, tt2, tt1>::type; };

  
  /*
   * Value wrapping - Simplifying handling primitive input constants and input SValues the same way
   */
  
  struct SValueWrapper {
    
    // does_wrap - check if a type can be unwrapped

    template<typename S, typename T>
    struct does_wrap;
    
    // unwrapped_type - the type of an unwrapped value

    template<typename>
    struct unwrapped_type {
      using type = void;
    }; 

    template<typename tt>
    struct unwrapped_type<SValue<tt> > { using type = tt; };

    template<int n, int m, typename inner>
    struct unwrapped_type<ConstructMatrix<n, m, inner> > { using type = SMat<n, m, inner>; };

    template<typename T1, typename T2, typename T3, SExprOp s>
    struct unwrapped_type<SExpr<T1, s, T2, T3> > { using type = T1; };

    template<typename tt>
    struct unwrapped_type<SGLSLHomoFun<tt> > { using type = tt; };

    template<typename tt>
    struct unwrapped_type<InputVar<tt> > { using type = tt; };

    template<typename tt>
    struct unwrapped_type<Constant<tt> > { using type = typename MapSType<tt>::type; };

    template<typename tt, SStorageClass storage>
    struct unwrapped_type<SPointerVar<tt, storage> > { using type = tt; };

    template<SStorageClass storage, typename tt>
    struct unwrapped_type<SUniformVar<storage, tt> > { using type = tt; };

    template<typename tt>
    struct unwrapped_type<SLocal<tt> > { using type = tt; };

    template<typename tt, SStorageClass stind>
    struct unwrapped_type<SLoadedVal<tt, stind> > { using type = tt; };

    template<typename tt>
    struct unwrapped_type<SCustomVal<tt> > { using type = tt; };

    template<typename tt>
    struct unwrapped_type<SelectConstruct<tt> > { using type = tt; };

    template<typename tt>
    struct unwrapped_type<tt&> { using type = typename unwrapped_type<tt>::type; };


    // Get type from primitives
    template<typename S, typename T>
    struct unwrapped_from_primitives {
      using ss = typename std::remove_reference<S>::type;
      using tt = typename std::remove_reference<T>::type;

      using type = typename MapSType<ss>::type;

      static_assert(std::is_same<ss, tt>::value,
		    "The two primitives found are not the same type");
    };
    
    // Get type the unwrapped type where only one type is guaranteed to have an
    // unambiguous type (a.k.a it's a spurv value with a type)

    template<typename S, typename T>
    requires RequireOneSpurvValue<S, T>
    struct unambiguous_unwrapped_require_spurv_type {

      using ss = typename std::remove_reference<S>::type;
      using tt = typename std::remove_reference<T>::type;
      
      using type = typename std::conditional<is_spurv_value<ss>::value,
					     typename unwrapped_type<ss>::type,
					     typename unwrapped_type<tt>::type>::type;
    };

    
    /*
     * ToType: Converts SValue<T> to T and primitives (i.e. float) to their SType counterparts (SFloat<32>)
     * - Really wish I knew a better way of doing this... (Especially since this is mostly a copy of the
     *   code above)
     */

    
    template<typename S>
    struct ToType;
    
    template<typename S>
    struct ToType<SValue<S> > {
      using type = S;
    };

    template<int n, int m, typename inner>
    struct ToType<ConstructMatrix<n, m, inner> > {
      using type = SMat<n, m, inner>;
    };

    template<typename T1, typename T2, typename T3, SExprOp s>
    struct ToType<SExpr<T1, s, T2, T3> > {
      using type = T1;
    };

    template<typename tt>
    struct ToType<SGLSLHomoFun<tt> > {
      using type = tt;
    };

    template<typename tt>
    struct ToType<InputVar<tt> > { using type = tt; };

    template<typename tt>
    struct ToType<Constant<tt> > { using type = typename MapSType<tt>::type; };

    template<typename tt, SStorageClass storage>
    struct ToType<SPointerVar<tt, storage> > { using type = tt; };

    template<SStorageClass storage, typename tt>
    struct ToType<SUniformVar<storage, tt> > { using type = tt; };

    template<typename tt>
    struct ToType<SLocal<tt> > { using type = tt; };

    template<typename tt, SStorageClass stind>
    struct ToType<SLoadedVal<tt, stind> > { using type = tt; };

    template<typename tt>
    struct ToType<SCustomVal<tt> > { using type = tt; };

    template<typename tt>
    struct ToType<SelectConstruct<tt> > { using type = tt; };

    template<typename tt>
    struct ToType<tt&> { using type = typename ToType<tt>::type; };
    
    template<typename S>
    struct ToType {
      using type = typename MapSType<S>::type;
    };

    
    // Accessible functions
  
    template<typename S, typename T>
    requires NoSpurvValue<S>
    static SValue<T>& unwrap_to(S& val);

    template<typename S, typename T>
    requires IsSpurvCastable<S, T>
    static SValue<T>& unwrap_to(S& val);

    template<typename S, typename T>
    requires IsSpurvValueOf<S, T>
    static SValue<T>& unwrap_to(S& val);

    
    // Get unwrapped type where we accept both primitives and spurv values
    template<typename S, typename T>
    struct unambiguous_unwrapped_allow_primitives {
      using ss = typename std::remove_reference<S>::type;
      using tt = typename std::remove_reference<T>::type;

      using uw1 = typename ToType<ss>::type;
      using uw2 = typename ToType<tt>::type;

      static_assert(std::is_same<uw1, uw2>::value, "[spurv] Types need to unwrap to same spurv type");
      using type = uw1;
    };
  };

  template<typename S, typename T>
  struct SValueWrapper::does_wrap {
    using ss = typename std::remove_reference<S>::type;
    using tt = typename std::remove_reference<T>::type;
      
    static constexpr bool value =
      std::is_base_of<SValue<tt>, ss>::value ||
      (std::is_fundamental<typename InvMapSType<tt>::type>::value &&
       std::is_convertible<ss, typename InvMapSType<tt>::type>::value) ||
      is_spurv_castable<typename SValueWrapper::ToType<ss>::type, tt>::value;
  };
  
};


#endif // __SPURV_VALUE_WRAPPER
