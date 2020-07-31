#ifndef __SPURV_POINTERS
#define __SPURV_POINTERS

#include "values.hpp"

#include <iostream>

namespace spurv {

  /*
   * SPointerBase - base for pointers
   */

  class SPointerBase {
  protected:

    unsigned int id;
    bool is_defined;

    SPointerBase();
    
    void ensure_defined(std::vector<uint32_t>& res);
    
    virtual void define(std::vector<uint32_t>& res) = 0;
    virtual void ensure_type_defined(std::vector<uint32_t>& res,
				     std::vector<SDeclarationState*>& declaration_states) = 0;

    int getID();
    
    virtual int getChainLength() = 0;
    virtual void outputChainNumber(std::vector<uint32_t>& res) = 0;

    template<typename tt, SStorageClass storage>
    friend class SAccessChain;

    template<typename tt>
    friend class SStoreEvent;

    friend class SVariableRegistry;

    template<typename tt>
    friend class SVariableEntry;
  };
  

  /*
   * SPointerTypeBase - base for pointers parameterized by type
   */

  template<typename tt>
  class SPointerTypeBase : public SPointerBase {
  public:
    template<typename t1>
    void store(t1&& val);

    virtual SValue<tt>& load() = 0;
  };


  /*
   * Utility structs for determining result types of indexing pointers
   */


  template<typename tt, int n>
  struct member_access_result { using type = void; };

  template<typename... inner_types, int n>
  struct member_access_result<SStruct<inner_types...>, n > { using type = typename SUtils::NthType<n, inner_types...>::type; };


  template<typename tt>
  struct index_access_result { using type = void; };

  template<typename inner_type, SStorageClass storage, int n>
  struct index_access_result<SArr<n, storage, inner_type> > { using type = inner_type; };

  template<typename inner_type, SStorageClass storage>
  struct index_access_result<SRunArr<storage, inner_type> > { using type = inner_type; };
  
  
  /*
   * SPointerVar - represents variable / pointer
   */

  template<typename tt, SStorageClass storage>
  class SPointerVar : public SPointerTypeBase<tt> /*, public SAccessChainBase */ {
  protected:
    virtual void define(std::vector<uint32_t>& res);
    virtual void ensure_type_defined(std::vector<uint32_t>& res,
				     std::vector<SDeclarationState*>& declaration_states);

    virtual int getChainLength();
    virtual void outputChainNumber(std::vector<uint32_t>& res);

    
  public:
    virtual SValue<tt>& load();
    
    template<int n>
    SAccessChain<typename member_access_result<tt, n>::type, storage>& member();

    template<typename tind>
    SAccessChain<typename index_access_result<tt>::type, storage>& operator[](tind&& ind);
    
    friend class SUtils;

    friend class SLoadedVal<tt, storage>;

    template<SShaderType tp, typename... InputTypes>
    friend class SShader;

    template<SStorageClass stind, typename... InnerTypes>
    friend class SStructBinding;

    template<typename type>
    friend class SUniformConstant;
  };


  /*
   * SAccessChain - represents a link in an access chain
   */

  template<typename tt, SStorageClass storage>
  class SAccessChain : public SPointerVar<tt, storage> {

    SPointerBase* acb;
    SValue<int_s>* index_value;

    template<typename tind>
    SAccessChain(SPointerBase* parent, tind&& index);

    virtual void define(std::vector<uint32_t>& res);
    virtual int getChainLength();
    virtual void outputChainNumber(std::vector<uint32_t>& res);

    virtual void ensure_type_defined(std::vector<uint32_t>& res,
				     std::vector<SDeclarationState*>& declaration_states);

    friend class SUtils;
  };

  
  /*
   * SLoadedVal - represents values loaded from SPointerLoad
   */

  template<typename tt, SStorageClass storage>
  class SLoadedVal : public SValue<tt> {

    SLoadedVal(SPointerVar<tt, storage>* pointer);

    SPointerVar<tt, storage>* pointer;
    
    virtual void define(std::vector<uint32_t>& res);
    virtual void ensure_type_defined(std::vector<uint32_t>& res,
				     std::vector<SDeclarationState*>& declaration_states);
    
    friend class SUtils;
    
  };
  
};


#endif // __SPURV_POINTERS
