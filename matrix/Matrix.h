#include <vector>
#include <exception>
#include <iostream>
//#include <boost/numeric/ublas/matrix.hpp>

template <typename Type>
class Matrix {
   private:
      std::vector< std::vector<Type> > elements_;
      unsigned rows_;
      unsigned cols_;

   public:
      Matrix() {
         // set to 1x1 matrix with one zero element
         *this = Matrix(1, 1, 0);
      }

      Matrix(unsigned cols, unsigned rows, Type initValue = 0) : cols_(cols), rows_(rows) {
         // set all values to 0
         elements_.resize(cols, std::vector<Type>(rows, initValue));
      }

      Matrix(const Matrix& that) {
         // resize appropriately
         *this = Matrix(that.cols(), that.rows());
         for (int c = 0; c < this->cols(); c++) {
            for (int r = 0; r < this->rows(); r++) {
               this->elements_[c][r] = that.elements_[c][r];
            }
         }
      }

      void initialise(std::vector<Type> elements) {
         if (elements.size() < cols_ * rows()) 
            throw "Not enough elements passed to initialise()";
         if (elements.size() > cols_ * rows()) 
            throw "Too many elements passed to initialise()";

         for (int r = 0; r < rows(); r++) {
            for (int c = 0; c < cols(); c++) {
               (*this)(c, r) = elements[r*cols() + c];
            }
         }
      }
      
      static Matrix identity(unsigned size) {
         // square matrix of zeroes
         Matrix output(size, size);
         for (int i = 0; i < size; i++) 
            output.elements_[i][i] = 1;
         return output;
      }

      template <typename Output_Type>
      Matrix<Output_Type> convert() {
         Matrix<Output_Type> output(this->cols(), this->rows());
         for (int c = 0; c < output.cols(); c++) {
            for (int r = 0; r < output.rows(); r++) {
               output(c, r) = static_cast<Output_Type>((*this)(c, r));
            }
         }
         return output;
      }

      void print(const char* name) const {
         std::cout << "\n" << name << ":\n";
         for (int r = 0; r < rows(); r++) {
            for (int c = 0; c < cols(); c++) {
               std::cout << elements_[c][r] << ' ';
            }
            std::cout << '\n';
         }
      }

      unsigned rows() const { return rows_; }
      unsigned cols() const { return cols_; }
      
      Type& operator()(unsigned col, unsigned row) { 
         if (row > rows_) {
            printf("\nRow number %d is larger than row count %d\n", row, rows_);
         } else if (col > cols_) {
            printf("\nColumn number %d is larger than column count %d\n", col, cols_);
         } else {
            return elements_[col][row];
         }
         throw "Exiting...";
      }
      
      static bool can_add(const Matrix& m1, const Matrix& m2) {
         return (m1.rows() == m2.rows()) && (m1.cols() == m2.cols());
      }

      static bool can_multiply(const Matrix& m1, const Matrix& m2) {
         return m1.cols() == m2.rows();
      }

      Matrix operator+(const Matrix& that) {
         if (!can_add(*this, that)) 
            throw "Use error checking when adding!\n";

         Matrix output(*this);
         for (int c = 0; c < cols(); c++) {
            for (int r = 0; r < rows(); r++) {
               output(c, r) = this->elements_[c][r] + that.elements_[c][r];
            }
         }
         return output;
      }

      Matrix& operator+=(const Matrix& that) {
         *this = *this + that;
      }

      Matrix operator-() const { // unary negation operator
         Matrix output(*this);
         for (int c = 0; c < output.cols(); c++) {
            for (int r = 0; r < output.rows(); r++) {
               output(c, r) = -1 * output(c, r);
            }
         }
         return output;
      }

      Matrix operator-(const Matrix& that) { // binary subtraction operator
         return *this + (-that);
      }

      Matrix& operator-=(const Matrix& that) {
         *this = *this - that;
      }

      Type dot(std::vector<Type> v1, std::vector<Type> v2) {
         if (v1.size() != v2.size()) 
            throw "Passed two differently sized vectors to dot()";

         Type result = 0;
         for (int i = 0; i < v1.size(); i++)
            result += v1[i] * v2[i];
         return result;
      }

      Matrix operator*(const Matrix& that) {
         if (!can_multiply(*this, that)) 
            throw "Use error checking when multiplying!\n";

         Matrix output = Matrix(that.cols(), this->rows());
         
         for (int r = 0; r < output.rows(); r++) {
            std::vector<Type> dotRow;
            for (int i = 0; i < this->cols(); i++) dotRow.push_back( this->elements_[i][r] );
            for (int c = 0; c < output.cols(); c++) {
               std::vector<Type> dotCol;
               for (int i = 0; i < that.rows(); i++) dotCol.push_back( that.elements_[c][i] );
               output(c, r) = dot(dotRow, dotCol);
            }
         }

         return output;
      }

      template <typename Factor>
      Matrix operator*(Factor factor) {
         Matrix output(*this);
         for (int c = 0; c < cols(); c++) {
            for (int r = 0; r < rows(); r++) {
               output(c, r) *= factor;
            }
         }
         return output;
      }

      template <typename Factor>
      Matrix& operator*=(Factor factor) {
         *this = *this * factor;
      }

      template <typename Factor>
      Matrix operator/(Factor factor) {
         return *this * (1.f / (double)factor);
      }

      template <typename Factor>
      Matrix& operator/=(Factor factor) {
         *this = *this / factor;
      }

      bool operator==(const Matrix& that) {
         if (this->cols() != that.cols() || this->rows() != that.rows())
            return false;

         for (int c = 0; c < this->cols(); c++) {
            for (int r = 0; r < this->rows(); r++) {
               // allowing for fp-precision numbers
               if (abs(this->elements_[c][r] - that.elements_[c][r]) < 0.0001 )
                  return false;
            }
         }
         return true;
      }

      bool operator!=(const Matrix& that) {
         return !(*this == that);
      }

      Matrix submatrix(int colToIgnore, int rowToIgnore) {
         Matrix output(cols()-1, rows()-1);
         for (int c = 0; c < output.cols(); c++) {
            for (int r = 0; r < output.rows(); r++) {
               if (c >= colToIgnore && r >= rowToIgnore)
                  output(c, r) = (*this)(c+1, r+1);
               else if (c >= colToIgnore) 
                  output(c, r) = (*this)(c+1, r);
               else if (r >= rowToIgnore)
                  output(c, r) = (*this)(c, r+1);
               else 
                  output(c, r) = (*this)(c, r);
            }
         }
         return output;
      }

      Matrix transpose() {
         Matrix output(cols(), rows());
         for (int c = 0; c < output.cols(); c++) {
            for (int r = 0; r < output.rows(); r++) {
               output(c, r) = (*this)(r, c);
            }
         }
         return output;
      }

      Type determinant() {
         if (rows() != cols())
            throw "rows != cols in determinant()";

         // if we cant reduce it any more
         if (rows() == 2) {
            return (*this)(0,0) * (*this)(1,1) - (*this)(1,0) * (*this)(0,1);
         }

         Type det = 0;
         for (int i = 0; i < cols(); i++) {
            int factor = (i % 2 == 0) ? 1 : -1;
            det += factor * (*this)(i, 0) * this->submatrix(i, 0).determinant();
         }
         return det;
      }

      Matrix adjoint() {
         Matrix output(cols(), rows());
         for (int c = 0; c < output.cols(); c++) {
            for (int r = 0; r < output.rows(); r++) {
               output(c, r) = this->submatrix(c, r).determinant();

               bool both_even = (c % 2 == 0) && (r % 2 == 0);
               bool both_odd = (c % 2 == 1) && (r % 2 == 1);
               bool both_equal = (c == r);
               output(c, r) *= (both_equal || both_even || both_odd) ? 1 : -1;
            }
         }
         return output;
      }

      Matrix<float> inverse() {
         Type det = this->determinant();
         if (det == 0)
            throw "Determinant = 0, so there is no inverse";

         auto transpose = this->transpose();
         auto inverse = transpose.adjoint();
         Matrix<float> float_inverse = inverse.convert<float>();
         float_inverse /= det;
         return float_inverse;
      }
};