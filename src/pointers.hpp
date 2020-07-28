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

  };


  /*
   * SAccessChainBase - base (interface) for access chain pointers (which includes all pointers)
   */

  class SAccessChainBase {
  protected:
    
    virtual int getChainLength() = 0;
    virtual void outputChainNumber(std::vector<uint32_t>& res) = 0;
  };

  /*
   * SPointerTypeBase - base for pointers parameterized by type
   */

  template<typename tt>
  class SPointerTypeBase : public SPointerBase {
  protected:
    SValue<tt>& load();

    template<typename t1>
    void store(t1&& val);
  };


  /*
   * Utility structs for determining result types of indexing pointers
   */


  template<typename tt, int n>
  struct member_access_result { using type = void; };

  template<typename... inner_types, int n>
  struct member_access_result<SStruct<inner_types...> > { using type = typename SUtils::NthType<n, inner_types>::type; };


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
  class SPointerVar : public SPointerBase, public SAccessChainBase {
  protected:
    virtual void define(std::vector<uint32_t>& res);
    virtual void ensure_type_defined(std::vector<uint32_t>& res,
				     std::vector<SDeclarationState*>& declaration_states);

    virtual int getChainLength();
    virtual void outputChainNumber(std::vector<uint32_t>& res);
    
    template<int n>
    SAccessChain<typename member_access_result<tt, n>::type, storage>& member();

    template<typename tind>
    SAccessChain<typename index_access_result<tt>::type, storage>& operator[](tind&& ind);
    
    friend class SUtils;

    friend class SLoadedVal<tt, storage>;
  };


  /*
   * SAccessChain - represents a link in an access chain
   */

  template<typename tt, SStorageClass storage>
  class SAccessChain : public SPointerVar<tt, storage> {

    SAccessChainBase* acb;
    SValue<int_s>* index_value;

    template<typename tind>
    SAccessChain(SAccessChainBase* parent, tind&& index);

    virtual void define(std::vector<uint32_t>& res);
    virtual int getChainLength();
    virtual void outputChainNumber(std::vector<uint32_t>& res);

    friend class SUtils;
  };

  
  /*
   * SLoadedVal - represents values loaded from SPointerLoad
   */

  template<typename tt, SStorageClass storage>
  class SLoadedVal : public SValue<tt> {

    SLoadedVal(SPointerVal<tt, storage>* pointer);

    SPointerVal<tt, storage>* pointer;
    
    virtual void define(std::vector<uint32_t>& res);
    virtual void ensure_type_defined(std::vector<uint32_t>& res,
				     std::vector<SDeclarationState*>& declaration_states);

    
    friend class SUtils;
    
  };
  
};


#endif // __SPURV_POINTERS
