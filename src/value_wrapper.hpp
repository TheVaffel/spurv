#ifndef __SPURV_VALUE_WRAPPER
#define __SPURV_VALUE_WRAPPER

#include "declarations.hpp"
#include "types.hpp"

namespace spurv {

  
    
  /*
   * RequireOneSpurvValue - A concept requiring one Spurv type among
   * the two provided types
   */
  
  template<typename S, typename T>
  concept constexpr bool RequireOneSpurvValue =
    (is_spurv_value<typename std::remove_reference<S>::type>::value ||
     is_spurv_value<typename std::remove_reference<T>::type>::value);

  /*
   * RequireUnambiguousUnwrappable - A concept requiring the two arguments to be
   * of the same type and having an unambiguous unwrapped type
   */

  template<typename S, typename T>
  concept constexpr bool RequireUnambiguousUnwrappable =
    (RequireOneSpurvValue<S, T> ||
    std::is_same<typename MapSType<typename std::remove_reference<S>::type >::type,
     typename MapSType<typename std::remove_reference<T>::type >::type>::value);
  

  /*
   * Conceptification of not is_spurv_value
   */
  
  template<typename S>
  concept constexpr bool NoSpurvValue =
    !is_spurv_value<typename std::remove_reference<S>::type>::value;


  /*
   * Concept seeing if two spurv values are the same
   */
  
  template<typename S, typename T>
  concept constexpr bool SameSpurvType =
    is_spurv_type<S>::value && is_spurv_type<T>::value && std::is_same<S, T>::value;


  /*
   * Concept seeing if first is a spurv value of the second type
   */
  
  template<typename S, typename T>
  concept constexpr bool IsSpurvValueOf =
    is_spurv_type<T>::value &&
    is_spurv_value<typename std::remove_reference<S>::type>::value &&
    std::is_base_of<SValue<T>, typename std::remove_reference<S>::type>::value;
  
  /*
   * Value wrapping - Simplifying handling primitive input constants and input SValues the same way
   */
  
  struct SValueWrapper {
    
    // does_wrap - check if a type can be unwrapped

    template<typename S, typename T>
    struct does_wrap {
      using ss = typename std::remove_reference<S>::type;
      using tt = typename std::remove_reference<T>::type;
      
      static constexpr bool value =
	std::is_base_of<SValue<tt>, ss>::value ||
	std::is_convertible<ss, typename InvMapSType<tt>::type>::value;
    };
    
    // unwrapped_type - the type of an unwrapped value

    template<typename>
    struct unwrapped_type {
      using type = void;
    }; 

    template<typename tt>
    struct unwrapped_type<SValue<tt> > { using type = tt; };

    template<int n, int m>
    struct unwrapped_type<ConstructMatrix<n, m> > { using type = SMat<n, m>; };

    template<typename T1, typename T2, typename T3, SExprOp s>
    struct unwrapped_type<SExpr<T1, s, T2, T3> > {
      using type = T1;
    };

    template<typename tt>
    struct unwrapped_type<InputVar<tt> > { using type = tt; };
    
    // Accessible functions
  
    template<typename S, typename T>
    requires NoSpurvValue<S>
    static SValue<T>& unwrap_to(S &val);

    template<typename S, typename T>
    requires IsSpurvValueOf<S, T>
    static SValue<T>& unwrap_to(S& val);

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

    // Another variant of the above function, not ensuring that any of the two inputs are SValues
    // so that it can be used in conditionals 
    
    template<typename S, typename T>
    struct unambiguous_unwrapped_require_spurv_type_unsafe {
      using ss = typename std::remove_reference<S>::type;
      using tt = typename std::remove_reference<T>::type;
      using type = typename std::conditional<is_spurv_value<ss>::value,
					     typename unwrapped_type<ss>::type,
					     typename unwrapped_type<tt>::type>::type;
    };

    

    // Get unwrapped type where we accept both primitives and spurv values
    template<typename S, typename T>
    requires RequireUnambiguousUnwrappable<S, T>
    struct unambiguous_unwrapped_allow_primitives {
      using ss = typename std::remove_reference<S>::type;
      using tt = typename std::remove_reference<T>::type;

      using type = typename std::conditional<is_spurv_value<ss>::value ||
					     is_spurv_value<tt>::value,
					     typename unambiguous_unwrapped_require_spurv_type_unsafe<S, T>::type,
					     typename unwrapped_from_primitives<S, T>::type>::type; 
    };
  };
};


#endif // __SPURV_VALUE_WRAPPER
