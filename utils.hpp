#ifndef __SPURV_UTILS
#define __SPURV_UTILS

namespace spurv {

  /*
   * Global util functions
   */
  
  template<int n, typename...Types>
  struct Utils::NthType {
    using type = typename std::tuple_element<n, std::tuple<Types...> >::type;
  };

  template<typename FirstType, typename... InnerTypes>
  void Utils::ensureDefinedRecursive(std::vector<uint32_t>& bin,
				     std::vector<TypeDeclarationState*>& declaration_states) {
    FirstType::ensure_defined(bin, declaration_states);

    if constexpr(sizeof...(InnerTypes) > 0) {
	Utils::ensureDefinedRecursive<InnerTypes...>(bin, declaration_states);
      }
  }

  template<typename FirstType, typename... InnerTypes>
  void Utils::addIDsRecursive(std::vector<uint32_t>& bin) {
    Utils::add(bin, FirstType::getID());

    if constexpr(sizeof...(InnerTypes) > 0) {
	Utils::addIDsRecursive<InnerTypes...>(bin);
      }
  }

  template<typename FirstType, typename... InnerTypes>
  constexpr bool Utils::isSpurvTypeRecursive() {
    if constexpr(is_spurv_type<FirstType>::value == false) {
	return false;
      }
    
    if constexpr(sizeof...(InnerTypes) > 0) {
	return Utils::isSpurvTypeRecursive<InnerTypes...>();
      }

    return true;
  }

  template<typename FirstType, typename... InnerTypes>
  void Utils::getDSpurvTypesRecursive(DSpurvType *pp) {
    FirstType::getDSpurvType(pp);
    if constexpr(sizeof...(InnerTypes) > 0) {
	Utils::getDSpurvTypesRecursive<InnerTypes...>(pp + 1);
      }
  }
  
  int Utils::global_id_counter = 1;

  int Utils::getNewID() {
    return Utils::global_id_counter++;
  }

  int Utils::getCurrentID() {
    return Utils::global_id_counter;
  }

  void Utils::resetID() {
    Utils::global_id_counter = 1;
  }

  int Utils::stringWordLength(const std::string str) {
    return (str.length() + 1 + 3 ) / 4; // Make room for terminating zero, round up to 4-byte words
  }

  void Utils::add(std::vector<uint32_t>& binary, int a) {
    binary.push_back(a);
  }

  void Utils::add(std::vector<uint32_t>& binary, std::string str) {
    int len = str.length(); // Space for null terminator
    int n = Utils::stringWordLength(str);

    const char* pp = str.c_str();
    for(int i = 0; i < n - 1; i++) {
      Utils::add(binary, *((int32_t*)(pp + 4 * i)));
    }

    int left = len - (n - 1) * 4;
    char last_int[4];
    for(int i = 0; i < left; i++) {
      last_int[i] = str[(n - 1) * 4 + i];
    }
  
    for(int i = left; i < 4; i++) {
      last_int[i] = 0; // Pad with zeros
    }

    Utils::add(binary, *(int32_t*)last_int);
  }
}

#endif // __SPURV_UTILS
