#ifndef __SPURV_CONTROL_FLOW
#define __SPURV_CONTROL_FLOW

#include "types.hpp"

namespace spurv {

  /*
   * SControlStructureBase - base class for different control statements (conditionals and loops)
   */

  class SControlStructureBase {
    virtual SControlType getControlType() = 0;

    template<SShaderType type, typename... InnerType>
    friend class SShader;
  };
  
  
  /*
   * SIfThen - class representing an if-statement (-chain)
   */

  class SIfThen : public SControlStructureBase {

    SValue<bool_s>* cond;
    bool hasElse;

    int ifthen_label, else_label, merge_label;
    
    SIfThen(SValue<bool_s>* cond);
    
    void write_type_definitions(std::vector<uint32_t>& bin,
				std::vector<SDeclarationState*>& declaration_states);
    void write_begin(std::vector<uint32_t>& bin);
    void write_else(std::vector<uint32_t>& bin);
    void write_end(std::vector<uint32_t>& bin);

    bool has_else(); // If has a finishing else block
    void add_else();

    virtual SControlType getControlType();

    friend class SIfEvent;
    friend class SElseEvent;
    friend class SEndIfEvent;

    template<SShaderType type, typename... InputTypes>
    friend class SShader;

    friend class SUtils;
  };
  
  
  /*
   * SForLoop - class representing a loop construct
   */
  
  class SForLoop : public SControlStructureBase {
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

    void write_break(std::vector<uint32_t>& bin);
    void write_continue(std::vector<uint32_t>& bin);

    virtual SControlType getControlType();
    
    template<SShaderType type, typename... InputTypes>
      friend class SShader;

    friend class SForBeginEvent;
    friend class SForEndEvent;

    friend class SBreakEvent;
    friend class SContinueEvent;

    friend class SUtils;
    
  };
  
};


#endif // __SPURV_CONTROL_FLOW
