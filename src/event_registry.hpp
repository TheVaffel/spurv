#ifndef __SPURV_LOAD_STORE_REGISTRY
#define __SPURV_LOAD_STORE_REGISTRY

#include "declarations.hpp"
#include "control_flow.hpp"

namespace spurv {


  /*
   * STimeEventBase - Base class for events like loads and stores
   */
  
  class STimeEventBase {
  protected:
    
    int event_num;
    bool is_written;
    void ensure_written(std::vector<uint32_t>& bin);
    
    virtual void ensure_type_defined(std::vector<uint32_t>& bin,
				     std::vector<SDeclarationState*>& declaration_states);
    virtual void write_binary(std::vector<uint32_t>& bin) = 0;
    virtual bool stores_to_pointer(int n);

    STimeEventBase(int event_num);

    friend class SEventRegistry;

    template<typename tt>
    friend class SLocal;
  };


  /*
   * SLoadEvent - Represents a load event (duh)
   */
  
  template<typename tt>
  class SLoadEvent : public STimeEventBase {
    int pointer_id;
    SValue<tt>* val_p;
    
    virtual void ensure_type_defined(std::vector<uint32_t>& bin,
				     std::vector<SDeclarationState*>& declaration_states);
    virtual void write_binary(std::vector<uint32_t>& bin);

    SLoadEvent(int event_num, int pointer_id);

    friend class SEventRegistry;
    
    template<typename t1>
    friend class SLocal;
  };


  /*
   * SStoreEvent - Represents a store event (duh)
   */
  
  template<typename tt>
  class SStoreEvent : public STimeEventBase {
    
    SLocal<tt>* pointer;
    SValue<tt>* val_p;

    virtual void ensure_type_defined(std::vector<uint32_t>& bin,
				     std::vector<SDeclarationState*>& declaration_states);

    virtual void write_binary(std::vector<uint32_t>& bin);
    virtual bool stores_to_pointer(int n);
    
    SStoreEvent(int event_num, SLocal<tt>* pointer);
    
    friend class SEventRegistry;
    
    template<typename t1>
    friend class SLocal;
  };


  /*
   * SForBeginEvent - Represents the first clause of a for loop
   */

  class SForBeginEvent : public STimeEventBase {
    SForLoop* loop;

    SForBeginEvent(int evnum, SForLoop* loop);
    
    virtual void ensure_type_defined(std::vector<uint32_t>& bin,
				     std::vector<SDeclarationState*>& declaration_states);
    virtual void write_binary(std::vector<uint32_t>& bin);

    friend class SUtils;

    friend class SEventRegistry;
  };

  
  /*
   * SForEndEvent - Represents the last clause of a for loop
   */
  
  class SForEndEvent : public STimeEventBase {
    SForLoop* loop;

    SForEndEvent(int evnum, SForLoop* loop);

    virtual void write_binary(std::vector<uint32_t>& bin);

    friend class SUtils;

    friend class SEventRegistry;
  };

  
  /*
   * SEventRegistry - Keeps track of load and store operations (primarily on local variables). This is important 
   * to ensure that loads and stores happen in the correct order, which is not enforced by the depth-first tree 
   * output algorithm in SShader.
   */ 
  
  
  class SEventRegistry {
    static std::vector<STimeEventBase*> events;

    template<typename tt>
    static SLoadEvent<tt>* addLoad(int pointer_id);

    template<typename tt>
    static SStoreEvent<tt>* addStore(SLocal<tt>* pointer);

    static void addForBegin(SForLoop* loop);

    static void addForEnd(SForLoop* loop);

    static void write_type_definitions(std::vector<uint32_t>& bin,
				       std::vector<SDeclarationState*>& declaration_states);
    static void write_events(std::vector<uint32_t>& bin);

    template<typename tt>
    static void ensure_predecessor_written(SLoadEvent<tt>* load,
					   std::vector<uint32_t>& bin);
    
    static void clear();
    
    template<SShaderType, typename... InnerTypes>
    friend class SShader;

    template<typename tt>
    friend class SLocal;
  };

};
#endif // __SPURV_LOAD_STORE_REGISTRY
