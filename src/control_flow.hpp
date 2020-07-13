#ifndef __SPURV_CONTROL_FLOW
#define __SPURV_CONTROL_FLOW

#include "types.hpp"

namespace spurv {

  /*
   * SForLoop - class representing a loop construct
   */
  
  class SForLoop {
    SForLoop(int start, int end);
    
    int label_merge, label_check, label_body, label_increment, label_post;
    int start, end;

    SLocal<int_s>* iterator_pointer;
    SValue<int_s>* iterator_val;

    Constant<int>* start_constant;
    Constant<int>* end_constant;
    Constant<int>* increment_constant;

    void write_type_definitions(std::vector<uint32_t>& bin,
				std::vector<SDeclarationState*>& declaration_states);
    void write_start(std::vector<uint32_t>& bin);
    void write_end(std::vector<uint32_t>& bin);
    
    template<SShaderType type, typename... InputTypes>
      friend class SShader;

    friend class SForBeginEvent;
    friend class SForEndEvent;

    friend class SUtils;
    
  };
  
};


#endif // __SPURV_CONTROL_FLOW
