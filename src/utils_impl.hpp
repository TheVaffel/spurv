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
  tt* SUtils::allocate(Types... args) {
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
