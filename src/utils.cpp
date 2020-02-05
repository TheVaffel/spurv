#include "utils.hpp"

#include <vector>

namespace spurv {

  
  std::vector<SUtils::PWrapperBase*> SUtils::allocated_values;

  void SUtils::clearAllocations() {
    for(unsigned int i = 0; i < SUtils::allocated_values.size(); i++) {
      SUtils::allocated_values[i]->exterminate();
      delete SUtils::allocated_values[i];
    }
    SUtils::allocated_values.clear();
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
    return (int)(str.length() + 1 + 3 ) / 4; // Make room for terminating zero, round up to 4-byte words
  }

  void SUtils::add(std::vector<uint32_t>& binary, int a) {
    binary.push_back(a);
  }

  void SUtils::add(std::vector<uint32_t>& binary, std::string str) {
    int len = (int)str.length(); // Space for null terminator
    int n = SUtils::stringWordLength(str);

    const char* pp = str.c_str();
    for(int64_t i = 0; i < ((int64_t)n) - 1LL; i++) {
      SUtils::add(binary, *((int32_t*)(pp + 4 * i)));
    }

    int left = len - (n - 1) * 4;
    char last_int[4];
    for(int64_t i = 0; i < left; i++) {
      last_int[i] = str[(size_t)(n - 1LL) * 4 + i];
    }
  
    for(int i = left; i < 4; i++) {
      last_int[i] = 0; // Pad with zeros
    }

    SUtils::add(binary, *(int32_t*)last_int);
  }

};
