#ifndef __SPURV_UTILS_IMPL
#define __SPURV_UTILS_IMPL

#include "utils.hpp"

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


  template<typename tt>
  struct is_falg_mat : std::false_type {};

  template<int n, int m>
  struct is_falg_mat<falg::Matrix<n, m> > : std::true_type {};

  template<int n, int m>
  static int dims(falg::Matrix<n, m>& mat) {
    return n * m;
  }
  
  template<typename FirstType, typename... InnerTypes>
  constexpr int SUtils::sum_num_elements(FirstType&& ft, InnerTypes&&... args) {
    int s = 0;
    if constexpr (sizeof...(InnerTypes) > 0) {
	s = sum_num_elements(args...);
      }
    
    if constexpr(is_spurv_value<FirstType>::value) {
	using tt = typename FirstType::type;
	if constexpr(tt::getKind() == STypeKind::KIND_MAT) {
	    return s + tt::getArg0() * tt::getArg1();
	  } else {
	  return s + 1;
	}
      } else if constexpr(is_falg_mat<FirstType>::value) {
	return s + dims(ft);
      } else {

      return s + 1;
    }
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
  tt* SUtils::allocate(Types&... args) {
    PWrapper<tt>* p = new PWrapper<tt>;
    p->pp = new tt(args...);
    SUtils::allocated_values.push_back(p);
    return p->pp; 
  }


  template<typename tt>
  void SUtils::PWrapper<tt>::exterminate() {
    delete this->pp;
  }
  
};

#endif // __SPURV_UTILS_IMPL
