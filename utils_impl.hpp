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
  
  int SUtils::global_id_counter = 1;

  int SUtils::getNewID() {
    return SUtils::global_id_counter++;
  }

  int SUtils::getCurrentID() {
    return SUtils::global_id_counter;
  }

  void SUtils::resetID() {
    SUtils::global_id_counter = 1;
  }

  int SUtils::stringWordLength(const std::string str) {
    return (str.length() + 1 + 3 ) / 4; // Make room for terminating zero, round up to 4-byte words
  }

  void SUtils::add(std::vector<uint32_t>& binary, int a) {
    binary.push_back(a);
  }

  void SUtils::add(std::vector<uint32_t>& binary, std::string str) {
    int len = str.length(); // Space for null terminator
    int n = SUtils::stringWordLength(str);

    const char* pp = str.c_str();
    for(int i = 0; i < n - 1; i++) {
      SUtils::add(binary, *((int32_t*)(pp + 4 * i)));
    }

    int left = len - (n - 1) * 4;
    char last_int[4];
    for(int i = 0; i < left; i++) {
      last_int[i] = str[(n - 1) * 4 + i];
    }
  
    for(int i = left; i < 4; i++) {
      last_int[i] = 0; // Pad with zeros
    }

    SUtils::add(binary, *(int32_t*)last_int);
  }

};

#endif // __SPURV_UTILS_IMPL
