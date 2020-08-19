#ifndef __SPURV_EVENT_REGISTRY_IMPL
#define __SPURV_EVENT_REGISTRY_IMPL

#include "event_registry.hpp"

namespace spurv {

  
  /*
   * SDeclarationEvent member functions
   */

  template<typename tt>
  SDeclarationEvent<tt>::SDeclarationEvent(int event_num, SValue<tt>* val) : STimeEventBase(event_num) {
    this->value = val;
  }

  template<typename tt>
  void SDeclarationEvent<tt>::ensure_type_defined(std::vector<uint32_t>& bin,
					      std::vector<SDeclarationState*>& declaration_states) {
    this->value->ensure_type_defined(bin, declaration_states);
  }

  template<typename tt>
  void SDeclarationEvent<tt>::write_binary(std::vector<uint32_t>& bin) {
    this->value->ensure_defined(bin);
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
			       SPointerTypeBase<tt>* pointer) : STimeEventBase(event_id) {
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
   * SImageStoreEvent member functions
   */

  template<typename im_type>
  SImageStoreEvent<im_type>::SImageStoreEvent(int event_id,
					      SValue<im_type>& image,
					      SValue<typename lookup_index<im_type>::type>& coord,
					      SValue<typename lookup_result<im_type>::type>& value) : STimeEventBase(event_id) {
    this->image = &image;
    this->coord = &coord;
    this->value = &value;
  }

  template<typename im_type>
  void SImageStoreEvent<im_type>::ensure_type_defined(std::vector<uint32_t>& bin,
						      std::vector<SDeclarationState*>& declaration_states) {
    this->image->ensure_type_defined(bin, declaration_states);
    this->coord->ensure_type_defined(bin, declaration_states);
    this->value->ensure_type_defined(bin, declaration_states);
  }

  template<typename im_type>
  void SImageStoreEvent<im_type>::write_binary(std::vector<uint32_t>& bin) {
    this->image->ensure_defined(bin);
    this->coord->ensure_defined(bin);
    this->value->ensure_defined(bin);

    // OpImageWrite
    SUtils::add(bin, (4 << 16) | 99);
    SUtils::add(bin, this->image->getID());
    SUtils::add(bin, this->coord->getID());
    SUtils::add(bin, this->value->getID());
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
  SStoreEvent<tt>* SEventRegistry::addStore(SPointerTypeBase<tt>* pointer) {
    SStoreEvent<tt>* sl = new SStoreEvent<tt>(SEventRegistry::events.size(), pointer);

    SEventRegistry::events.push_back(sl);

    return sl;
  }

  template<typename im_type>
  SImageStoreEvent<im_type>* SEventRegistry::addImageStore(SValue<im_type>& image,
							   SValue<typename lookup_index<im_type>::type>& ind,
							   SValue<typename lookup_result<im_type>::type>& val) {
    SImageStoreEvent<im_type>* sise = new SImageStoreEvent<im_type>(SEventRegistry::events.size(), image, ind, val);

    SEventRegistry::events.push_back(sise);
    return sise;
  }

  template<typename tt>
  void SEventRegistry::addDeclaration(SValue<tt>* pointer) {
    SDeclarationEvent<tt>* de = new SDeclarationEvent(SEventRegistry::events.size(), pointer);
    SEventRegistry::events.push_back(de);
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
