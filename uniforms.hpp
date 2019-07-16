#ifndef __SPURV_UNIFORMS
#define __SPURV_UNIFORMS

#include "declarations.hpp"
#include "utils.hpp"

namespace spurv { 

  /*
   * SpurvUniformBindingBase - base class to make it possible for SpurvShader to keep track of uniforms
   */

  class SpurvUniformBindingBase {

  protected:
    int set_no, binding_no;
    int pointer_id;
    
  public:
    SpurvUniformBindingBase(int s, int b);

    int getSetNo();
    int getBindingNo();
    int getPointerID();

    virtual void definePointer(std::vector<uint32_t>& bin,
			       std::vector<TypeDeclarationState*>& declaration_states) = 0;
    virtual void decorateType(std::vector<uint32_t>& bin) = 0;
  };


  /*
   * SpurvUniformBinding - represents a single uniform binding of a descriptor set
   */
  
  template<typename... InnerTypes>
  class SpurvUniformBinding : public SpurvUniformBindingBase {
    
  private:
    std::vector<void*> value_pointers;
    
  public:  
    SpurvUniformBinding(int sn, int bn);
    
    template<int n>
    ValueNode<typename Utils::NthType<n, InnerTypes...>::type >& getBinding();

    virtual void definePointer(std::vector<uint32_t>& bin,
			       std::vector<TypeDeclarationState*>& declaration_states);
    virtual void decorateType(std::vector<uint32_t>& bin);
  };


};

#endif // __SPURV_UNIFORMS
