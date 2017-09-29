#include <iostream>
#include "Matrix.h"
using std::cout;

int main() {
   try {
      // A*X = B
      // => X = (A^-1)*B 
      Matrix<float> A(5, 5);
      A.initialise({
          3,  4,  8,  5, 4,
          5,  6,  9, 10, 1,
          7,  3,  9,  3, 2,
         12, 15, 20,  5, 9,
          9,  3,  5,  6, 8
      });

      Matrix<float> B(1, 5);
      B.initialise({
         170,
         204,
         138,
         394,
         223
      });

      // single column matrix of coefficients
      auto X = A.inverse() * B;
      X.print("X");
   } 
   catch (const char* e) { std::cout << e << '\n'; }
   return 0;
}