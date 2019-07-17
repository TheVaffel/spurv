#ifndef __SPURV_UNIFORMS
#define __SPURV_UNIFORMS

#include "declarations.hpp"
#include "utils.hpp"

namespace spurv { 

  /*
   * SUniformBindingBase - base class to make it possible for SShader to keep track of uniforms
   */

  class SUniformBindingBase {

  protected:
    int set_no, binding_no;
    int pointer_id;
    
  public:
    SUniformBindingBase(int s, int b);

    int getSetNo();
    int getBindingNo();
    int getPointerID();

    virtual void definePointer(std::vector<uint32_t>& bin,
			       std::vector<SDeclarationState*>& declaration_states) = 0;
    virtual void decorateType(std::vector<uint32_t>& bin) = 0;
  };


  /*
   * SUniformBinding - represents a single uniform binding of a descriptor set
   */
  
  template<typename... InnerTypes>
  class SUniformBinding : public SUniformBindingBase {
    
  private:
    std::vector<void*> value_pointers;
    
  public:  
    SUniformBinding(int sn, int bn);
    
    template<int n>
    SValue<typename SUtils::NthType<n, InnerTypes...>::type >& member();

    virtual void definePointer(std::vector<uint32_t>& bin,
			       std::vector<SDeclarationState*>& declaration_states);
    virtual void decorateType(std::vector<uint32_t>& bin);
  };


};

#endif // __SPURV_UNIFORMS
