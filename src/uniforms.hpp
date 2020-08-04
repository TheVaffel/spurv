#ifndef __SPURV_UNIFORMS
#define __SPURV_UNIFORMS

#include "declarations.hpp"
#include "utils.hpp"
#include "values.hpp"
#include "pointers.hpp"

namespace spurv { 

  /*
   * SUniformBindingBase - base class to make it possible for SShader to keep track of uniforms
   */

  class SUniformBindingBase {
  protected:
    int set_no, binding_no;
    
  public:
    SUniformBindingBase(int s, int b);

    int getSetNo();
    int getBindingNo();

    virtual void definePointer(std::vector<uint32_t>& bin,
			       std::vector<SDeclarationState*>& declaration_states) = 0;
    virtual void decorateType(std::vector<uint32_t>& bin,
			      std::vector<bool*>& decoration_states);

    virtual void* getPointer() = 0;

    virtual int getPointerID() = 0;

    template<typename type>
    friend class SUniformConstant;
  };


  
  /*
   * SStructBinding - Base for bindings using structs (uniforms and storage buffers)
   */

  template<SStorageClass store_ind, typename... InnerTypes>
  class SStructBinding : public SUniformBindingBase {
  protected:
    SPointerVar<SStruct<SDecoration::BLOCK, InnerTypes...>, store_ind>* pointer;
  public:  
    SStructBinding(int sn, int bn);
    
    // template<int n>
    // SValue<typename SUtils::NthType<n, InnerTypes...>::type >& member();

    virtual void definePointer(std::vector<uint32_t>& bin,
			       std::vector<SDeclarationState*>& declaration_states);
    virtual void decorateType(std::vector<uint32_t>& bin,
			      std::vector<bool*>& decoration_states);

    virtual void* getPointer();
    virtual int getPointerID();
  };

  
  /*
   * SUniformBinding - represents a single uniform binding of a descriptor set
   */
  
  template<typename... InnerTypes>
  class SUniformBinding : public SStructBinding<SStorageClass::STORAGE_UNIFORM, InnerTypes...> {
    SUniformBinding(int sn, int bn);

    friend class SUtils;
  };

  
  /*
   * SStorageBuffer - represents storage buffers
   */

  template<typename... InnerTypes>
  class SStorageBuffer : public SStructBinding<SStorageClass::STORAGE_STORAGE_BUFFER, InnerTypes...> {
    SStorageBuffer(int sn, int bn);

    friend class SUtils;
  };

  
  /*
   * SUniformConstant - for uniforms that don't need structs - like textures
   */

  template<typename type>
  class SUniformConstant : public SUniformBindingBase {

    SPointerVar<type, SStorageClass::STORAGE_UNIFORM_CONSTANT>* pointer;
    
  public:
    SUniformConstant(int sn, int bn);
    
    virtual void definePointer(std::vector<uint32_t>& bin,
			       std::vector<SDeclarationState*>& declaration_states);

    // virtual void define(std::vector<uint32_t>& res);
    // virtual void ensure_type_defined(std::vector<uint32_t>& res,
    // 				     std::vector<SDeclarationState*>& declaration_states);

    virtual void* getPointer();
    virtual int getPointerID();
  };


};

#endif // __SPURV_UNIFORMS
