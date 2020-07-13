#include "event_registry.hpp"

#include "utils_impl.hpp"

namespace spurv {
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
