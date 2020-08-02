
#include "types_impl.hpp"

namespace spurv {

  /*
   * SDeclarationState constructor
   */
  
  
  SDeclarationState::SDeclarationState() : id(-1), is_defined(false) {}
  
  /*
   * NullType member functions
   */
  
  void NullType::ensure_defined(std::vector<uint32_t>&,
			     std::vector<int32_t*>&) {}
  void NullType::define(std::vector<uint32_t>&) {}

  
  /*
   * DSType member functions
   */
  
  bool DSType::operator==(const DSType& ds) const {
    bool a = true;
    a = a && this->kind == ds.kind;
    a = a && this->a0 == ds.a0;
    a = a && this->a1 == ds.a1;
    a = a && this->a2 == ds.a2;
    a = a && this->a3 == ds.a3;
    a = a && this->a4 == ds.a4;
    
    if(this->inner_types.size() == ds.inner_types.size()) {
      for(int i = 0; i < this->inner_types.size(); i++) {
	a = a && (this->inner_types[i] == ds.inner_types[i]);
      }
      
      return a;
    } else {
      return false;
    }
  }

  bool DSType::operator!=(const DSType& ds) const {
    return !(*this == ds);
  }

  
  /*
   * Void member functions
   */
  
  template<>
  void SType<STypeKind::KIND_VOID>::define(std::vector<uint32_t>& bin) {
    SType<STypeKind::KIND_VOID>::ensureInitID();
    SType<STypeKind::KIND_VOID>::declareDefined();
    
    SUtils::add(bin, (2 << 16) | 19);
    SUtils::add(bin, SType<STypeKind::KIND_VOID>::getID());
    
  }

  template<>
  void SType<STypeKind::KIND_VOID>::ensure_defined(std::vector<uint32_t>& bin,
					std::vector<SDeclarationState*>& declaration_states) {
    if( !SType<STypeKind::KIND_VOID>::isDefined()) {
      define(bin);
      declaration_states.push_back(&(SType<STypeKind::KIND_VOID>::declarationState));
    }
  }


  /*
   * Bool member functions
   */

  void SBool::define(std::vector<uint32_t>& bin) {
    SType<STypeKind::KIND_BOOL>::ensureInitID();
    SType<STypeKind::KIND_BOOL>::declareDefined();

    SUtils::add(bin, (2 << 16) | 20);
    SUtils::add(bin, SType<STypeKind::KIND_BOOL>::getID());
  }

  void SBool::ensure_defined(std::vector<uint32_t>& bin,
					std::vector<SDeclarationState*>& declaration_states) {
    if(!SType<STypeKind::KIND_BOOL>::isDefined()) {
      define(bin);
      declaration_states.push_back(&(SType<STypeKind::KIND_BOOL>::declarationState));
    }
  }

  constexpr int SBool::getSize() {
    return 32; // For maximum portability (maybe). Specs don't say how large this is
  }
				
};
