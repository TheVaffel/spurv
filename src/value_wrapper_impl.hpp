#ifndef __SPURV_VALUE_WRAPPER_IMPL
#define __SPURV_VALUE_WRAPPER_IMPL

#include "value_wrapper.hpp"

namespace spurv {

  template<typename T>
  SValue<T>& SValueWrapper::unwrap_value(SValue<T> &val) {
    return val;
  }

  /* template<typename S>
  SValue<typename MapSType<S>::type >& SValueWrapper::unwrap_value(const S& val) {
    return *SUtils::allocate<Constant<S> >(val);
    } */

  template<typename S, typename T>
  SValue<T>& SValueWrapper::unwrap_to(S &val) {
    static_assert(does_wrap<S, T>::value);

    return &T::cons(val); // SUtils::allocate<Constant<S> >(val);
  }
};

#endif // __SPURV_VALUE_WRAPPER_IMPL
