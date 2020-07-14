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
   * SDeclarationEvent - Represents a variable declaration / definition
   */

  template<typename tt>
  class SDeclarationEvent : public STimeEventBase {
    SValue<tt>* value;

    SDeclarationEvent(int event_num, SValue<tt> *val);
    
    virtual void ensure_type_defined(std::vector<uint32_t>& bin,
				     std::vector<SDeclarationState*>& declaration_states);
    virtual void write_binary(std::vector<uint32_t>& bin);

    friend class SEventRegistry;
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
   * SIfEvent - represents the beginning of an if-statement
   */

  class SIfEvent : public STimeEventBase {
    SIfThen* ifthen;

    SIfEvent(int evnum, SIfThen* ifthen);

    virtual void ensure_type_defined(std::vector<uint32_t>& bin,
				     std::vector<SDeclarationState*>& declaration_states);
    virtual void write_binary(std::vector<uint32_t>& bin);

    friend class SEventRegistry;
    
    template<SShaderType type, typename... InputTypes>
    friend class SShader;
  };


  /*
   * SElseEvent - represents an else-statement
   */

  class SElseEvent : public STimeEventBase {
    SIfThen* ifthen;
    
    SElseEvent(int evnum, SIfThen* ifthen);

    virtual void ensure_type_defined(std::vector<uint32_t>& bin,
				     std::vector<SDeclarationState*>& declaration_states);
    virtual void write_binary(std::vector<uint32_t>& bin);

    friend class SEventRegistry;
    
    template<SShaderType type, typename... InputTypes>
    friend class SShader;
  };

  
  /*
   * SEndIfEvent - represents an endif-statement
   */

  class SEndIfEvent : public STimeEventBase {
    SIfThen* ifthen;
    SEndIfEvent(int evnum, SIfThen* ifthen);

    virtual void ensure_type_defined(std::vector<uint32_t>& bin,
				     std::vector<SDeclarationState*>& declaration_states);
    virtual void write_binary(std::vector<uint32_t>& bin);

    friend class SEventRegistry;

    template<SShaderType type, typename... InputTypes>
    friend class SShader;
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

    template<typename tt>
    static void addDeclaration(SValue<tt>* pointer);
    
    static void addIf(SIfThen* ifthen);
    static void addElse(SIfThen* ifthen);
    static void addEndIf(SIfThen* ifthen);
    
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

    template<typename tt>
    friend class SValue;
  };

};
#endif // __SPURV_LOAD_STORE_REGISTRY
