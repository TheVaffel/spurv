#include "spurv.hpp"

#include <iostream>

using namespace std;

int main() {
  // spurv::SExpr<spurv::mat2_s, spurv::EXPR_ADDITION, spurv::mat2_s, spurv::mat2_s>& expr =
  
  spurv::SShader<spurv::SHADER_VERTEX, spurv::float_s> shader;
  
  spurv::SValue<spurv::float_s>& in0 = shader.input<0>();
  spurv::SValue<spurv::float_s>& expr = in0 * (3.f * 3.f * 4.f + 5.f);
  
  cout << "Here is the id of the expression:" << endl;
  cout << expr << endl;

  cout << "The end!" << endl;

  expr.unref_tree();
  
  return 0;
}
