#ifndef __SPURV_VALUE_WRAPPER
#define __SPURV_VALUE_WRAPPER

#include "declarations.hpp"
#include "types.hpp"

namespace spurv {
  
  /*
   * Value wrapping - Simplifying handling primitive input constants and input SValues the same way
   */

  struct SValueWrapper {

    // is_unambiguous_wrapper - check if a type is a spurv value and
    // has an unambiguous wrapped type

    template<typename T>
    struct is_unambiguous_wrapper : std::false_type {};

    template<typename tt>
    struct is_unambiguous_wrapper<SValue<tt> > : std::true_type {};

    template<typename t1, SExprOp op, typename t2, typename t3>
    struct is_unambiguous_wrapper<SExpr<t1, op, t2, t3> > : std::true_type {};

    template<typename tt>
    struct is_unambiguous_wrapper<InputVar<tt> > : std::true_type {};

    template<int n, int m>
    struct is_unambiguous_wrapper<ConstructMatrix<n, m> > : std::true_type {};
    
    
    // does_wrap - check if a type can be unwrapped
    
    template<typename, typename>
    struct does_wrap : std::false_type {};

    template<typename T>
    struct does_wrap<SValue<T>, T> : std::true_type {};

    template<typename t1, SExprOp op, typename t2, typename t3>
    struct does_wrap<SExpr<t1, op, t2, t3>, t1> : std::true_type {};

    template<typename t1>
    struct does_wrap<InputVar<t1>, t1> : std::true_type {};
    
    template<int n>
    struct does_wrap<float, SFloat<n>> : std::true_type {};

    template<int n, int m>
    struct does_wrap<ConstructMatrix<n, m>, SMat<n, m>> : std::true_type {};

    template<typename t1, typename t2, typename t3>
    struct does_both_wrap {
      static constexpr bool value = does_wrap<t1, t3>::value && does_wrap<t2, t3>::value;
    };
    
    // unwrapped_type - the type of an unwrapped value

    template<typename>
    struct unwrapped_type : std::false_type { } ; 

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
    
    template<typename T>
    static SValue<T>& unwrap_value(SValue<T> &val);

    /* template<typename S>
       static SValue<typename MapSType<S>::type >& unwrap_value(const S& val); */

    template<typename S, typename T>
    static SValue<T>& unwrap_to(S &val);

    
    // Get type the unwrapped type where only one type is guaranteed to have an
    // unambiguous type

    template<typename S, typename T>
    struct unambiguous_unwrapped_type {

      using ss = typename std::remove_reference<S>::type;
      using tt = typename std::remove_reference<T>::type;
      
      using type = typename std::conditional<is_unambiguous_wrapper<ss>::value,
					     typename unwrapped_type<ss>::type,
					     typename unwrapped_type<tt>::type>::type;
    };
					     /* std::conditional<std::is_same<typename unwrapped_type<T>::type,
									   std::false_type>::value,
							      std::false_type,
							      typename unwrapped_type<T>::type>,
					     typename unwrapped_type<S>::type>::type;
      static_assert(!is_unambiguous_wrapper<S>::value && !is_unambiguous_wrapper<T>::value,
      "None of the types in unambiguous_unwrapped_type has unambiguous wrapped type");
      }; 
    // template<typename S, typename T>
    // struct unambiguous_unwrapped_type { using type = typename unwrapped_type<T>::type: };
      
    }; */

  /* template<>
  struct SValueWrapper::unwrapped_type<float> {
    using type = SFloat<32>;
    }; */

  };
};


#endif // __SPURV_VALUE_WRAPPER
