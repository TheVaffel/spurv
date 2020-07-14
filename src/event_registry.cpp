#include "event_registry.hpp"

#include "utils_impl.hpp"

namespace spurv {

  
  /*
   * SIfEvent member functions
   */

  SIfEvent::SIfEvent(int evnum, SIfThen* ifthen) : STimeEventBase(evnum) {
    this->ifthen = ifthen;
  }

  void SIfEvent::ensure_type_defined(std::vector<uint32_t>& bin,
				     std::vector<SDeclarationState*>& declaration_states) {
    this->ifthen->write_type_definitions(bin, declaration_states);
  }

  void SIfEvent::write_binary(std::vector<uint32_t>& bin) {
    this->ifthen->write_begin(bin);
  }


  /*
   * SElseEvent member functions
   */
  
  SElseEvent::SElseEvent(int evnum, SIfThen* ifthen) : STimeEventBase(evnum) {
    this->ifthen = ifthen;
  }

  void SElseEvent::ensure_type_defined(std::vector<uint32_t>& bin,
				       std::vector<SDeclarationState*>& declaration_states) { }

  void SElseEvent::write_binary(std::vector<uint32_t>& bin) {
    this->ifthen->write_else(bin);
  }

  
  /*
   * SEndIfEvent member functions
   */

  SEndIfEvent::SEndIfEvent(int evnum, SIfThen* ifthen) : STimeEventBase(evnum) {
    this->ifthen = ifthen;
  }
  
  void SEndIfEvent::ensure_type_defined(std::vector<uint32_t>& bin,
					std::vector<SDeclarationState*>& declaration_states) { }

  void SEndIfEvent::write_binary(std::vector<uint32_t>& bin) {
    this->ifthen->write_end(bin);
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

  void SEventRegistry::addIf(SIfThen* ifthen) {
    SIfEvent* ie = new SIfEvent(SEventRegistry::events.size(), ifthen);
    SEventRegistry::events.push_back(ie);
  }

  void SEventRegistry::addElse(SIfThen* ifthen) {
    SElseEvent* ee = new SElseEvent(SEventRegistry::events.size(), ifthen);
    SEventRegistry::events.push_back(ee);
  }

  void SEventRegistry::addEndIf(SIfThen* ifthen) {
    SEndIfEvent* ee = new SEndIfEvent(SEventRegistry::events.size(), ifthen);
    SEventRegistry::events.push_back(ee);
  }
  
  void SEventRegistry::addForBegin(SForLoop* loop) {
    SForBeginEvent* fb = new SForBeginEvent(SEventRegistry::events.size(), loop);
    SEventRegistry::events.push_back(fb);
  }

  void SEventRegistry::addForEnd(SForLoop* loop) {
    SForEndEvent* fb = new SForEndEvent(SEventRegistry::events.size(), loop);
    SEventRegistry::events.push_back(fb);
  }


  void SEventRegistry::write_type_definitions(std::vector<uint32_t>& bin,
					      std::vector<SDeclarationState*>& declaration_states) {
    for(STimeEventBase *eb : SEventRegistry::events) {
      eb->ensure_type_defined(bin, declaration_states);
    }
  }
  
  void SEventRegistry::write_events(std::vector<uint32_t>& bin) {

    // This function traverses the event list and outputs them (together
    // with their dependencies) in order

    for(STimeEventBase *eb : SEventRegistry::events) {
      eb->ensure_written(bin);
    }
    
  }

  void SEventRegistry::clear() {
    for(STimeEventBase* b : SEventRegistry::events) {
      delete b;
    }

    SEventRegistry::events.clear();
  }

  
};
