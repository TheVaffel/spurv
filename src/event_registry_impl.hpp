#ifndef __SPURV_EVENT_REGISTRY_IMPL
#define __SPURV_EVENT_REGISTRY_IMPL

#include "event_registry.hpp"

namespace spurv {

  /*
   * SEventRegistry member
   */

  std::vector<STimeEventBase*> SEventRegistry::events = std::vector<STimeEventBase*>();
  
  
  /*
   * STimeEventBase member functions
   */

  STimeEventBase::STimeEventBase(int event_num) {
    this->event_num = event_num;
    this->is_written = false;
  }

  void STimeEventBase::ensure_type_defined(std::vector<uint32_t>& bin,
					   std::vector<SDeclarationState*>& states) { }

  void STimeEventBase::ensure_written(std::vector<uint32_t>& bin) {
    if(!this->is_written) {
      this->is_written = true;

      this->write_binary(bin);
    }
  }

  bool STimeEventBase::stores_to_pointer(int n) {
    return false;
  }

  
  /*
   * SLoadEvent member functions
   */

  template<typename tt>
  SLoadEvent<tt>::SLoadEvent(int event_id,
			     int pointer_id) : STimeEventBase(event_id), pointer_id(pointer_id) {
  }

  template<typename tt>
  void SLoadEvent<tt>::ensure_type_defined(std::vector<uint32_t>& bin,
					   std::vector<SDeclarationState*>& declaration_states) {
    val_p->ensure_type_defined(bin, declaration_states);
  }

  template<typename tt>
  void SLoadEvent<tt>::write_binary(std::vector<uint32_t>& bin) {
    val_p->ensure_defined(bin);
  }

  
  /*
   * SStoreEvent member functions
   */

  template<typename tt>
  SStoreEvent<tt>::SStoreEvent(int event_id,
			       SLocal<tt>* pointer) : STimeEventBase(event_id) {
    is_written = false;
    this->pointer = pointer;
  }

  template<typename tt>
  void SStoreEvent<tt>::ensure_type_defined(std::vector<uint32_t>& bin,
					    std::vector<SDeclarationState*>& declaration_states) {
    this->pointer->ensure_type_defined(bin, declaration_states);
    this->val_p->ensure_type_defined(bin, declaration_states);
  }

  template<typename tt>
  void SStoreEvent<tt>::write_binary(std::vector<uint32_t>& bin) {
    this->pointer->ensure_defined(bin);
    this->val_p->ensure_defined(bin);
    
    // OpStore
    SUtils::add(bin, (3 << 16) | 62);
    SUtils::add(bin, this->pointer->getID());
    SUtils::add(bin, this->val_p->getID());
  }

  template<typename tt>
  bool SStoreEvent<tt>::stores_to_pointer(int id) {
    return this->pointer->getID() == id;
  }


  /*
   * SForBeginEvent member functions
   */

  SForBeginEvent::SForBeginEvent(int evnum, SForLoop* loop) : STimeEventBase(evnum) {
    this->loop = loop;
  }

  void SForBeginEvent::write_binary(std::vector<uint32_t>& bin) {
    loop->write_start(bin);
  }

  void SForBeginEvent::ensure_type_defined(std::vector<uint32_t>& bin,
					   std::vector<SDeclarationState*>& declaration_states) {
    this->loop->write_type_definitions(bin, declaration_states);
  }


  /*
   * SForEndEvent member functions
   */

  SForEndEvent::SForEndEvent(int evnum, SForLoop* loop) : STimeEventBase(evnum) {
    this->loop = loop;
  }

  void SForEndEvent::write_binary(std::vector<uint32_t>& bin) {
    loop->write_end(bin);
  }
  
  
  /*
   * SEventRegistry member functions
   */

  template<typename tt>
  SLoadEvent<tt>* SEventRegistry::addLoad(int pointer_id) {
    SLoadEvent<tt>* sl = new SLoadEvent<tt>(SEventRegistry::events.size(), pointer_id);

    SEventRegistry::events.push_back(sl);

    return sl;
  }


  template<typename tt>
  SStoreEvent<tt>* SEventRegistry::addStore(SLocal<tt>* pointer) {
    SStoreEvent<tt>* sl = new SStoreEvent<tt>(SEventRegistry::events.size(), pointer);

    SEventRegistry::events.push_back(sl);

    return sl;
  }

 
  
  template<typename tt>
  void SEventRegistry::ensure_predecessor_written(SLoadEvent<tt>* load,
						  std::vector<uint32_t>& bin) {
    int num = load->event_num;
    int pi = load->pointer_id;
    for(int i = num - 1; i > 0; i--) {
      if(SEventRegistry::events[i]->stores_to_pointer(pi)) {
	SEventRegistry::events[i]->ensure_written(bin);

	break;
      }
    }
  }
};

#endif // __SPURV_EVENT_REGISTRY_IMPL
