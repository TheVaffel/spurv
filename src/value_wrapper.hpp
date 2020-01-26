#ifndef __SPURV_VALUE_WRAPPER
#define __SPURV_VALUE_WRAPPER

#include "declarations.hpp"
#include "types.hpp"

namespace spurv {
  
  /*
   * Value wrapping - Simplifying handling primitive input constants and input SValues the same way
   */

  struct SValueWrapper {
    
    // does_wrap - check if a type can be unwrapped
    
    template<typename, typename>
    struct does_wrap : std::false_type {};

    template<typename T>
    struct does_wrap<SValue<T>, T> : std::true_type {};

    template<typename t1, SExprOp op, typename t2, typename t3>
    struct does_wrap<SExpr<t1, op, t2, t3>, t1> : std::true_type {};
    
    template<int n>
    struct does_wrap<float, SFloat<n>> : std::true_type {};

    template<int n, int m>
    struct does_wrap<ConstructMatrix<n, m>, SMat<n, m>> : std::true_type {};


    // unwrapped_type - the type of an unwrapped value

    template<typename>
    struct unwrapped_type { };

    template<typename tt>
    struct unwrapped_type<SValue<tt> > { using type = tt; };

    template<int n, int m>
    struct unwrapped_type<ConstructMatrix<n, m> > { using type = SMat<n, m>; };

    
    // Accessible functions
    
    template<typename T>
    static SValue<T>& unwrap_value(SValue<T> &val);

    template<typename S>
    static SValue<typename MapSType<S>::type >& unwrap_value(const S& val);

    template<typename S, typename T>
    static SValue<T>& unwrap_to(S &val);
  };

  template<>
  struct SValueWrapper::unwrapped_type<float> {
    using type = SFloat<32>;
  };

  template<typename T1, typename T2, typename T3, SExprOp s>
  struct SValueWrapper::unwrapped_type<SExpr<T1, s, T2, T3> > {
    using type = T1;
  };
};

#endif // __SPURV_VALUE_WRAPPER
