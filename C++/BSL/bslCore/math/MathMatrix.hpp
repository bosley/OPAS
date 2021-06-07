#ifndef BSLCORE_MATH_MATRIX_HPP
#define BSLCORE_MATH_MATRIX_HPP
#include <iomanip> 
#include <iostream>
#include <vector>
#include <exception>

namespace MATH
{
    namespace {

        inline void throw_runtime(std::string msg) {

            std::__throw_runtime_error(msg.c_str());
        }
    }

    //!
    //! \class Matrix
    //! \brief Basic matrix class
    //!
    template <class T>
    class Matrix {

    public:

        static constexpr int    DEFAULT_OSTREAM_WIDTH = 10;
        static constexpr double DEFAULT_COMPARISON_EPSILON = 1e-6;

        //!
        //! \brief Construct matrix 
        //!
        Matrix() : nrow(0), ncol(0), viewWidth(DEFAULT_OSTREAM_WIDTH) {

        }

        //!
        //! \brief Construct matrix given size
        //!
        Matrix(uint64_t row, uint64_t col) : nrow(row), ncol(col), viewWidth(DEFAULT_OSTREAM_WIDTH) {

        }
        //!
        //! \brief Construct matrix with default value
        //!
        Matrix(uint64_t row, uint64_t col, T val) : nrow(row), ncol(col), viewWidth(DEFAULT_OSTREAM_WIDTH) {

            reserve(val);
        }

        //!
        //! \brief Change width setting for ostream
        //!
        void setOstreamWidth(int width) {

            if(width < 0) {
                width = 0;
            }

            viewWidth = width;
        }

        //!
        //! \brief Get a row from the matrix
        //!
        std::vector<T&> getRow(uint64_t row) {

            std::vector<T&> crow;

            crow.reserve(nrow);

            for(uint64_t i = 0; i < ncol; i++) {
                crow.push_back(mat[row][i]);
            }

            return crow;
        }

        //!
        //! \brief Get a column from the matrix
        //!
        std::vector<T&> getCol(uint64_t col) {

            std::vector<T&> column;

            column.reserve(nrow);

            for(uint64_t i = 0; i < nrow; i++) {
                column.push_back(mat[i][col]);
            }

            return column;
        }

        //!
        //! \brief Fill the matrix with a value
        //!
        void fill(T val) {

            for(uint64_t i = 0; i < nrow; i++) {
                for(uint64_t j = 0; j < ncol; j++) {
                    mat[i][j] = val;
                }
            }
        }

        //!
        //! \brief Access row via []
        //!
        std::vector<T> & operator [] (uint64_t row) {

            return mat[row];
        }

        //!
        //! \brief Check for equality with another matrix
        //!
        bool operator ==(const Matrix<T> & other) {

            if(nrow != other.nrow || ncol != other.ncol) {
                return false;
            }

            for(uint64_t i = 0; i < nrow; i++) {

                for(uint64_t j = 0; j < ncol; j++) {

                    if (!(std::abs(mat[i][j] - other.mat[i][j] ) < DEFAULT_COMPARISON_EPSILON)) {

                        return false;
                    }
                }
            }

            return true;
        }

        //!
        //! \brief Add a matrix
        //!
        Matrix<T> operator +(const Matrix<T> & other) {

            if(other.nrow != nrow || other.ncol != other.ncol) {
                throw_runtime("Matrix must have same dimentions to perform operation [+]");
            }

            Matrix<T> result(nrow, ncol);

            for(uint64_t i = 0; i < nrow; i++) {

                std::vector<T> currentRow;

                for(uint64_t j = 0; j < ncol; j++) {
                    currentRow.push_back(
                        mat[i][j] + other.mat[i][j]
                    );
                }
                result.mat.push_back(currentRow);
            }

            return result;
        }

        //!
        //! \brief Add a scalar
        //!
        Matrix<T> operator +(const T & scalar) {

            Matrix<T> result(nrow, ncol);

            for(uint64_t i = 0; i < nrow; i++) {

                std::vector<T> currentRow;

                for(uint64_t j = 0; j < ncol; j++) {
                    currentRow.push_back(
                        mat[i][j] + scalar
                    );
                }
                result.mat.push_back(currentRow);
            }

            return result;
        }

        //!
        //! \brief Subtract a matrix
        //!
        Matrix<T> operator -(const Matrix<T> & other) {

            if(other.nrow != nrow || other.ncol != other.ncol) {
                throw_runtime("Matrix must have same dimentions to perform operation [-]");
            }

            Matrix<T> result(nrow, ncol);

            for(uint64_t i = 0; i < nrow; i++) {

                std::vector<T> currentRow;

                for(uint64_t j = 0; j < ncol; j++) {
                    currentRow.push_back(
                        mat[i][j] - other.mat[i][j]
                    );
                }
                result.mat.push_back(currentRow);
            }

            return result;
        }

        //!
        //! \brief Subtract a scalar
        //!
        Matrix<T> operator -(const T & scalar) {

            Matrix<T> result(nrow, ncol);

            for(uint64_t i = 0; i < nrow; i++) {

                std::vector<T> currentRow;

                for(uint64_t j = 0; j < ncol; j++) {
                    currentRow.push_back(
                        mat[i][j] - scalar
                    );
                }
                result.mat.push_back(currentRow);
            }

            return result;
        }

        //!
        //! \brief Multiply with a matrix
        //!
        Matrix<T> operator *(const Matrix<T> & other) { 

            if(ncol != other.nrow) {
                throw_runtime("Column-row mismatch. Column size must equal row size to perform operation [*]");
            }

            T initData = 0.0;

            Matrix<T> result(nrow, other.ncol, initData);

            for(std::size_t row = 0; row < result.mat.size(); ++row) {

                for(std::size_t col = 0; col < result.mat.at(0).size(); ++col) {

                    T sum = 0;
                    for(std::size_t inner = 0; inner < other.mat.size(); ++inner) {

                        sum += mat.at(row).at(inner) * other.mat.at(inner).at(col);
                    }
                    result.mat.at(row).at(col) = sum;
                }
            }

            return result;
        }

        //!
        //! \brief Multiply with a matrix
        //!
        Matrix<T>& operator *=(const Matrix<T> & other) { 

            if(ncol != other.nrow) {
                throw_runtime("Column-row mismatch. Column size must equal row size to perform operation [*=]");
            }

            T initData = 0.0;

            Matrix<T> result(nrow, other.ncol, initData);

            for(std::size_t row = 0; row < result.mat.size(); ++row) {

                for(std::size_t col = 0; col < result.mat.at(0).size(); ++col) {

                    T sum = 0;
                    for(std::size_t inner = 0; inner < other.mat.size(); ++inner) {

                        sum += mat.at(row).at(inner) * other.mat.at(inner).at(col);
                    }
                    result.mat.at(row).at(col) = sum;
                }
            }

            ncol = result.ncol;
            nrow = result.nrow;
            mat = result.mat;
            
            return *this;
        }

        //!
        //! \brief Multiply with a scalar
        //!
        Matrix<T> operator *(const T & scalar) {

            Matrix<T> result(nrow, ncol);

            for(uint64_t i = 0; i < nrow; i++) {

                std::vector<T> currentRow;

                for(uint64_t j = 0; j < ncol; j++) {
                    currentRow.push_back(
                        mat[i][j] * scalar
                    );
                }
                result.mat.push_back(currentRow);
            }

            return result;
        }

        //!
        //! \brief Multiply with a scalars
        //!
        Matrix<T>& operator *=(const T & scalar) {

            for(uint64_t i = 0; i < nrow; i++) {

                for(uint64_t j = 0; j < ncol; j++) {
                    mat[i][j] = mat[i][j] * scalar;
                }
            }

            return *this;
        }

        //!
        //! \brief Set matrix
        //!
        void operator =(const Matrix<T> & other) {

            nrow = other.nrow;
            ncol = other.ncol;
            mat.clear();
            mat = other.mat;
        }

        //!
        //! \brief Add matrix
        //!
        Matrix<T>& operator +=(const Matrix<T> & other) {

            if(other.nrow != nrow || other.ncol != other.ncol) {
                throw_runtime("Matrix must have same dimentions to perform operation [+=]");
            }

            for(uint64_t i = 0; i < nrow; i++) {

                for(uint64_t j = 0; j < ncol; j++) {
                    mat[i][j] = mat[i][j] + other.mat[i][j];
                }
            }
            
            return *this;
        }

        //!
        //! \brief Add scalar
        //!
        Matrix<T>& operator +=(const T & scalar) {

            for(uint64_t i = 0; i < nrow; i++) {

                for(uint64_t j = 0; j < ncol; j++) {
                    mat[i][j] = mat[i][j] + scalar;
                }
            }
            
            return *this;
        }

        //!
        //! \brief Subtract matrix
        //!
        Matrix<T>& operator -=(const Matrix<T> & other) {

            if(other.nrow != nrow || other.ncol != other.ncol) {
                throw_runtime("Matrix must have same dimentions to perform operation [-=]");
            }

            for(uint64_t i = 0; i < nrow; i++) {

                for(uint64_t j = 0; j < ncol; j++) {
                    mat[i][j] = mat[i][j] - other.mat[i][j];
                }
            }
            
            return *this;
        }

        //!
        //! \brief Subtract scalar
        //!
        Matrix<T>& operator -=(const T & scalar) {

            for(uint64_t i = 0; i < nrow; i++) {

                for(uint64_t j = 0; j < ncol; j++) {
                    mat[i][j] = mat[i][j] - scalar;
                }
            }
            
            return *this;
        }

        //!
        //! \brief Transpose matrix
        //!
        void transpose() {

            T initData = 0.0;
            Matrix<T> result(ncol, nrow, initData);

            for(uint64_t i = 0; i < nrow; i++) {

                for(uint64_t j = 0; j < ncol; j++) {

                    result.mat[j][i] = mat[i][j];

                }
            }
            mat = result.mat;
            ncol = result.ncol;
            nrow = result.nrow;
        }

        //!
        //! \brief Get a copy of the matrix
        //!
        std::vector< std::vector<T> > getMatrix() const {
            return mat;
        }

        //!
        //! \brief View matrix in stream
        //!
        friend std::ostream& operator<<(std::ostream& os, const Matrix<T>& mat) {

            for(uint64_t i = 0; i < mat.nrow; i++) {
                for(uint64_t j = 0; j < mat.ncol; j++) {
                    os << std::setw(mat.viewWidth) <<  mat.mat[i][j] << " ";
                }
                os << std::endl;
            }

            return os;
        }

    private:

        void reserve(T val) {

            for(uint64_t i = 0; i < nrow; i++) {

                std::vector<T> currentRow;
                for(uint64_t j = 0; j < ncol; j++) {
                    currentRow.push_back(val);
                }
                mat.push_back(currentRow);
            }
        }
        uint64_t nrow;
        uint64_t ncol;
        std::vector< std::vector<T> > mat;
        int viewWidth;
    };

}

#endif