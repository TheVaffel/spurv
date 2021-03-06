#ifndef __SPURV_UTILS_IMPL
#define __SPURV_UTILS_IMPL

#include "utils.hpp"
#include "values.hpp"

#include "value_wrapper.hpp"

namespace spurv {
  /*
   * Global util functions
   */
  
  template<int n, typename...Types>
  struct SUtils::NthType {
    using type = typename std::tuple_element<n, std::tuple<Types...> >::type;
  };

  template<typename FirstType, typename... InnerTypes>
  void SUtils::ensureDefinedRecursive(std::vector<uint32_t>& bin,
				     std::vector<SDeclarationState*>& declaration_states) {
    FirstType::ensure_defined(bin, declaration_states);

    if constexpr(sizeof...(InnerTypes) > 0) {
	SUtils::ensureDefinedRecursive<InnerTypes...>(bin, declaration_states);
      }
  }

  template<typename FirstType, typename... InnerTypes>
  void SUtils::addIDsRecursive(std::vector<uint32_t>& bin) {
    SUtils::add(bin, FirstType::getID());

    if constexpr(sizeof...(InnerTypes) > 0) {
	SUtils::addIDsRecursive<InnerTypes...>(bin);
      }
  }

  /*
   * Utilities for counting number of elements in values
   */

  template<typename tt>
  struct is_falg_mat : std::false_type {};

  template<int n, int m>
  struct is_falg_mat<falg::Matrix<n, m> > : std::true_type {};

  template<int n, int m>
  static int dims(falg::Matrix<n, m>& mat) {
    return n * m;
  }

  template<typename tt>
  struct SUtils::num_elements { static constexpr int value = 1; };

  template<int n, int m, typename tt>
  struct SUtils::num_elements<SMat<n, m, tt> > { static constexpr int value = n * m; };

  template<int n, int m>
  struct SUtils::num_elements<falg::Matrix<n, m> > { static constexpr int value = n * m; };

  template<typename FirstType, typename SecType, typename... InnerTypes>
  struct SUtils::sum_num_elements<FirstType, SecType, InnerTypes...> { 
      static constexpr int value = 
      SUtils::num_elements<typename SValueWrapper::ToType<FirstType>::type>::value + 
      SUtils::sum_num_elements<SecType, InnerTypes...>::value; 
  };

  template<typename FirstType>
  struct SUtils::sum_num_elements < FirstType > { 
      static constexpr int value = 
      SUtils::num_elements<typename SValueWrapper::ToType<FirstType>::type>::value; };

  template<typename First, typename... InnerTypes>
  constexpr bool SUtils::has_only_1_comps(First&& ft, InnerTypes&&... args) {
    bool s = true;
    if constexpr(sizeof...(InnerTypes) > 0) {
	s = SUtils::has_only_1_comps(args...);
      }
    
    return (SUtils::num_elements<First>::value == 1) && s;
  }

  template<typename First, typename... InnerTypes>
  constexpr bool SUtils::has_only_n_comps(int n, First&& ft, InnerTypes&&... args) {
    bool s = true;
    if constexpr(sizeof...(InnerTypes) > 0) {
	s = SUtils::has_only_n_comps(n, args...);
      }

    return (SUtils::num_elements<First>::value == n) && s;
  }

  
  template<typename FirstType, typename... InnerTypes>
  constexpr bool SUtils::isSTypeRecursive() {
    if constexpr(is_spurv_type<FirstType>::value == false) {
	return false;
      }
    
    if constexpr(sizeof...(InnerTypes) > 0) {
	return SUtils::isSTypeRecursive<InnerTypes...>();
      }

    return true;
  }

  template<typename FirstType, typename... InnerTypes>
  void SUtils::getDSTypesRecursive(DSType *pp) {
    FirstType::getDSType(pp);
    if constexpr(sizeof...(InnerTypes) > 0) {
	SUtils::getDSTypesRecursive<InnerTypes...>(pp + 1);
      }
  }

  template<typename T, typename... InnerTypes>
  constexpr int SUtils::getSumSize() {
    if constexpr( sizeof...(InnerTypes) > 0) {
	return T::getSize() + getSumSize<InnerTypes...>();
      } else {
      return T::getSize();
    }
  }

  template<typename tt, typename... Types>
  tt* SUtils::allocate(Types&&... args) {
    PWrapper<tt>* p = new PWrapper<tt>;
    p->pp = new tt(args...);
    SUtils::allocated_values.push_back(p);
    return p->pp; 
  }


  template<typename tt>
  void SUtils::PWrapper<tt>::exterminate() {
    delete this->pp;
  }

  
  // Just a shorthand (It is actually needed pretty badly for readability
  template<typename a, typename b>
  requires RequireOneSpurvValue<a, b>
  struct uwr { using type =
      typename SValueWrapper::unambiguous_unwrapped_require_spurv_type<a, b>::type;
  };
  
};

#endif // __SPURV_UTILS_IMPL
