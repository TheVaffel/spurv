#ifndef __SPURV_VALUE_WRAPPER_IMPL
#define __SPURV_VALUE_WRAPPER_IMPL

#include "value_wrapper.hpp"

namespace spurv {
  
  template<typename S, typename T>
  requires NoSpurvValue<S>
  SValue<T>& SValueWrapper::unwrap_to(S &val) {
    using ss = typename std::remove_reference<S>::type;
    using tt = typename std::remove_reference<T>::type;
    static_assert(does_wrap<ss, tt>::value);

    return T::cons(static_cast<typename InvMapSType<tt>::type>(val));
  }

  template<typename S, typename T>
  requires IsSpurvCastable<S, T>
  SValue<T>& SValueWrapper::unwrap_to(S& val) {
    using ss = typename std::remove_reference<S>::type;
    using tt = typename std::remove_reference<T>::type;
    static_assert(does_wrap<ss, tt>::value);

    return cast<tt>(val);
  }
  
  template<typename S, typename T>
  requires IsSpurvValueOf<S, T>
  SValue<T>& SValueWrapper::unwrap_to(S& val) {
    return val;
  }
};

#endif // __SPURV_VALUE_WRAPPER_IMPL
