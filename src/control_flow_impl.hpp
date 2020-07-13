#ifndef __SPURV_CONTROL_FLOW_IMPL
#define __SPURV_CONTROL_FLOW_IMPL

#include "control_flow.hpp"

namespace spurv {
  /*
   * SForLoop member functions
   */

  SForLoop::SForLoop(int start, int end) {
    this->label_merge = SUtils::getNewID();
    this->label_check = SUtils::getNewID();
    this->label_body = SUtils::getNewID();
    this->label_increment = SUtils::getNewID();
    this->label_post = SUtils::getNewID();

    if(end < start && end == 0) {
      this->start = 0;
      this->end = start;
    } else if(end < start) {
      printf("End value must be higher than start value in spurv for loops\n");
    } else {
      this->start = start;
      this->end = end;
    }

    this->iterator_pointer = SUtils::allocate<SLocal<int_s> >();
    this->iterator_val = SUtils::allocate<SCustomVal<int_s> >();
    this->start_constant = SUtils::allocate<Constant<int> >(this->start);
    this->end_constant = SUtils::allocate<Constant<int> >(this->end);
    this->increment_constant = SUtils::allocate<Constant<int> >(1);
  }

  void SForLoop::write_type_definitions(std::vector<uint32_t>& bin,
					std::vector<SDeclarationState*>& declaration_states) {
    SBool::ensure_defined(bin, declaration_states);
    this->iterator_pointer->ensure_type_defined(bin, declaration_states);
    this->iterator_val->ensure_type_defined(bin, declaration_states);
    this->start_constant->ensure_type_defined(bin, declaration_states);
    this->end_constant->ensure_type_defined(bin, declaration_states);
    this->increment_constant->ensure_type_defined(bin, declaration_states);
  }

  void SForLoop::write_start(std::vector<uint32_t>& bin) {
    this->iterator_pointer->ensure_defined(bin);

    // Store start value in pointer
    // OpStore <pointer_id> <val_id>
    SUtils::add(bin, (3 << 16) | 62);
    SUtils::add(bin, this->iterator_pointer->getID());
    SUtils::add(bin, this->start_constant->getID());
  
    // OpBranch <check_block>
    SUtils::add(bin, (2 << 16) | 249);
    SUtils::add(bin, this->label_merge);

    // OpLabel <check_block>
    SUtils::add(bin, (2 << 16) | 248);
    SUtils::add(bin, this->label_merge);

    // OpLoopMerge <merge_point (end)> <continue_point (increment)> <loop_control (None)>
    SUtils::add(bin, (4 << 16) | 246);
    SUtils::add(bin, this->label_post);
    SUtils::add(bin, this->label_increment);
    SUtils::add(bin, 0); // None

    // OpBranch <check>
    SUtils::add(bin, (2 << 16) | 249);
    SUtils::add(bin, this->label_check);

    // OpLabel <check>
    SUtils::add(bin, (2 << 16) | 248);
    SUtils::add(bin, this->label_check);

    // Load iterator variable and check if it has met its end goal yet
  
    // OpLoad <result_type (int)> <result id> <pointer>
    SUtils::add(bin, (4 << 16) | 61);
    SUtils::add(bin, SInt<32, 1>::getID());
    SUtils::add(bin, this->iterator_val->getID());
    SUtils::add(bin, this->iterator_pointer->getID());

    int is_within_range_id = SUtils::getNewID();
  
    // OpSLessThan <result_type> <result_id> <op1> <op2>
    SUtils::add(bin, (5 << 16) | 177);
    SUtils::add(bin, SBool::getID());
    SUtils::add(bin, is_within_range_id);
    SUtils::add(bin, this->iterator_val->getID());
    SUtils::add(bin, this->end_constant->getID());

    // OpBranchConditional <condition_id> <true_branch> <false_branch>
    SUtils::add(bin, (4 << 16) | 250);
    SUtils::add(bin, is_within_range_id);
    SUtils::add(bin, this->label_body);
    SUtils::add(bin, this->label_post);

    // OpLabel <body_id>
    SUtils::add(bin, (2 << 16) | 248);
    SUtils::add(bin, this->label_body);
  }


  void SForLoop::write_end(std::vector<uint32_t>& bin) {
    // OpBranch <increment>
    SUtils::add(bin, (2 << 16) | 249);
    SUtils::add(bin, this->label_increment);

    // OpLabel <increment>
    SUtils::add(bin, (2 << 16) | 248);
    SUtils::add(bin, this->label_increment);

    int new_load_id = SUtils::getNewID();
    
    // OpLoad <result_type (int> <result_id> <pointer>
    SUtils::add(bin, (4 << 16) | 61);
    SUtils::add(bin, SInt<32, 1>::getID());
    SUtils::add(bin, new_load_id);
    SUtils::add(bin, this->iterator_pointer->getID());
    
    int incremented_id = SUtils::getNewID();
    
    // OpIAdd <int_id> <result_id> <it_val_id> <const_1>
    SUtils::add(bin, (5 << 16) | 128);
    SUtils::add(bin, SInt<32, 1>::getID());
    SUtils::add(bin, incremented_id);
    SUtils::add(bin, new_load_id);
    SUtils::add(bin, this->increment_constant->getID());

    // OpStore <iterator_pointer> <incremented_id>
    SUtils::add(bin, (3 << 16) | 62);
    SUtils::add(bin, this->iterator_pointer->getID());
    SUtils::add(bin, incremented_id);

    // OpBranch <check>
    SUtils::add(bin, (2 << 16) | 249);
    SUtils::add(bin, this->label_merge);

    // OpLabel <post>
    SUtils::add(bin, (2 << 16) | 248);
    SUtils::add(bin, this->label_post);
  }

};
#endif // __SPURV_CONTROL_FLOW_IMPL
