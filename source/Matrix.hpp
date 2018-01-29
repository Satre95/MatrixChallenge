#pragma once

#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#include <type_traits>
#ifdef USE_INTRINSICS
#include "xmmintrin.h"
#include "pmmintrin.h"
#include "emmintrin.h"
#include "smmintrin.h"
#endif

template <class T>
class Matrix
{
public:
    Matrix(size_t numRows = 4, size_t numCols = 4, T defaultValue = 0);
    Matrix(const Matrix & other);
	~Matrix();
	
    /// Fetches the element at the given coordinates
    const T & operator()(size_t row, size_t col) const { return Get(row, col); }
    /// Fetches the element at the given coordinates
    T & operator()(size_t row, size_t col) { return Get(row, col); }
	/// Fetches the element at the given coordinates
	const T & Get(size_t row, size_t col) const;
	/// Fetches the element at the given coordinates
	T & Get(size_t row, size_t col);
    
    size_t Rows() const { return m_rows; }
    size_t Columns() const { return m_columns; }
    Matrix operator*(const Matrix & rhs);
    /// Returns the transpose of this matrix
    Matrix Transpose() const;   
private:
	/// Converts the 2D element coord to a 1D index
	size_t Index(const size_t & x, const size_t & y) const;
    /// Returns the vector of pointers to elements in the row at the given index.
    std::vector<T*> GetRow(size_t row);
    /// Returns a pointer to the first element in a column.
    T* GetColumn(size_t col);
    const T* GetColumn(size_t col) const;

    /// The # of rows in this matrix.
    const size_t m_rows;
	/// The # of columns in this matrix.
	const size_t m_columns;

	/**
	 * The array holding the elements.
	 * 1D array is more efficient than a 2D array with 
	 * separately allocated lines.
	 */
	T * m_data = nullptr;
};

template <class T>
Matrix<T>::Matrix(size_t numRows, size_t numCols, T defaultValue): m_rows(numRows), m_columns(numCols) {
    if(numRows * numCols == 0) throw std::invalid_argument("Error. Cannot create matrix with 0 dimension(s)");
    
    m_data = new T[numRows * numCols];
	std::fill(m_data, m_data + (numRows * numCols), defaultValue);
}

template <class T>
Matrix<T>::Matrix(const Matrix<T> & other): m_rows(other.m_rows), m_columns(other.m_columns) {
    m_data = new T[m_rows * m_columns];
    std::copy(other.m_data, other.m_data + (m_rows * m_columns), m_data);
}

template <class T>
Matrix<T>::~Matrix() {
    delete[] m_data;
    m_data = nullptr;
}

template <class T>
size_t Matrix<T>::Index(const size_t & row, const size_t & col) const {
    return col * m_rows + row; // Matrix is stored in COLUMN major
}

template <class T>
const T & Matrix<T>::Get(size_t row, size_t col) const {
	if(row >= m_rows || col >= m_columns)
        throw std::invalid_argument( "Invalid element coordinate" );
    return m_data[Index(row, col)];
}

template <class T>
T & Matrix<T>::Get(size_t row, size_t col) {
    if(row >= m_rows || col >= m_columns)
        throw std::invalid_argument( "Invalid element coordinate" );
    return m_data[Index(row, col)];
}

template <class T>
std::vector<T*> Matrix<T>::GetRow(size_t rowIndex) {
    // This method is called only by fns that error check, so no need to check rowIndex
    std::vector<T*> row;
    row.reserve(m_columns);
    for(size_t i = 0; i < m_columns; i++) {
        auto ptr = &Get(rowIndex, i);
        row.push_back(ptr);
    }
    
    return row;
}

// Since Matrix is stored in column major, just return a pointer to the first element in the column.
template <class T>
T* Matrix<T>::GetColumn(size_t colIndex) {
    // This method is called only by fns that error check, so no need to check colIndex
    auto ptr = m_data + colIndex * m_rows;
    return ptr;
}

// Since Matrix is stored in column major, just return a pointer to the first element in the column.
template <class T>
const T* Matrix<T>::GetColumn(size_t colIndex) const {
    // This method is called only by fns that error check, so no need to check colIndex
    auto ptr = m_data + colIndex * m_rows;
    return ptr;
}

#ifdef USE_INTRINSICS //Version of the fn that use SSE2 intrinsics.

template <class T>
Matrix<T> Matrix<T>::operator*(const Matrix<T> & rhs) {
    //My # of columns (width) must equal # of rows (height) in other matrix.
    if(m_columns != rhs.m_rows)
        throw std::invalid_argument("Invalid argument. Width (columns) of first matrix must match height (rows) of second matrix");
    
    //Note that the length of each row is num columns and vice versa.
    Matrix<T> result(m_rows, rhs.m_columns);

   #pragma omp parallel for
    for (size_t i = 0; i < m_rows; i++) {
        T * rowData = new T[m_columns];
        
        for (size_t j = 0; j < rhs.m_columns; j++) {
            std::vector<T*> rowA = GetRow(i);
            //Copy the row elements into a contiguous array.
            for(size_t h = 0; h < m_columns; h++) {
                auto elemPtr = rowA.at(h);
                rowData[h] = *elemPtr;
            }
            //Get the column data.           
            const T* colB = rhs.GetColumn(j);
            
            T res = 0;
            size_t k = 0;
            
            //Need to handle each type separately for SIMD.
            if(std::is_same<float, T>::value) {
                //Process using SIMD
                for(; k + 3 < m_columns; k+=4) {
                    __m128 colVec = _mm_loadu_ps((float *)(colB + k));
                    __m128 rowVec = _mm_loadu_ps((float *)(rowData + k));
                    __m128 prodSum = _mm_mul_ps(colVec, rowVec);
                    //The _mm_hadd_ps only adds adjacent elements, so run twice to get
                    //full horizontal add.
                    //prodsum = { p3, p2, p1, p0 }
                    prodSum = _mm_hadd_ps(prodSum, prodSum);
                    //prodSum = { p3 + p2, p1 + p0, p3 + p2, p1 + p0 }
                    prodSum = _mm_hadd_ps(prodSum, prodSum);
                    //prodSum = { p3 + p2 + p1 + p0, p3 + p2 + p1 + p0, p3 + p2 + p1 + p0, p3 + p2 + p1 + p0 }
                    //Other sums are superfluous but don't incur penalty.
                    float temp;
                    _mm_store_ss(&temp, prodSum);
                    res += temp;
                    
                }
            } else if(std::is_same<double, T>::value) {
                for(; k + 1 < m_columns; k+=2) {
                    __m128d colVec = _mm_loadu_pd((double *)(colB + k));
                    __m128d rowVec = _mm_loadu_pd((double *)(rowData + k));
                    __m128d prodSum = _mm_mul_pd(colVec, rowVec);
                    prodSum = _mm_hadd_pd(prodSum, prodSum);
                    double temp;
                    _mm_store_sd(&temp, prodSum);
                    res += temp;
                    
                }
            }
            //Unfortunately, Intel SSE intrinsics for integer multiplication & addition
            //are not as robust as floating point. Given time, I believe there is a way
            //to write an vector SIMD implementation for integer matrices that is faster
            //than scalar arithmetic, but it isn't trivial, so I've tabled that for later.
            /*
            else if(std::is_same<int, T>::value) {
                for (; k + 3 < m_columns; k+=4) {
                    __m128i colVec = _mm_load_si128((__m128i*)colB + k);
                    __m128i rowVec = _mm_load_si128((__m128i*)rowData + k);
                    __m128i product = _mm_mullo_epi32(colVec, rowVec);
                    //Unfortunately, there is not horizontal add fn for ints, so
                    //need to extract and sum scalar wise
                    int tempVec[4];
                    _mm_storeu_si128((__m128i*)&tempVec, product);
                    for(size_t m = 0; m < 4; m++)
                        res += tempVec[m];
                }
            }
            */
            
            //Process the remainder values using standard scalar arithmetic.
            for(; k < m_columns; k++)
                res += (rowData[k] * colB[k]);
            
            result(i, j) = res;
            rowA.clear();
        }
        delete[] rowData;
    }
    
    return result;
}

#else

template <class T>
Matrix<T> Matrix<T>::operator*(const Matrix<T> & rhs) {
    //My # of columns (width) must equal # of rows (height) in other matrix.
    if(m_columns != rhs.m_rows)
        throw std::invalid_argument("Invalid argument. Width (columns) of first matrix must match height (rows) of second matrix");
    
    //Note that the length of each row is num columns and vice versa.
    //LHS = A, RHS = B
    Matrix<T> result(m_rows, rhs.m_columns);
    #pragma omp parallel for
    for (size_t i = 0; i < m_rows; i++) {
        for (size_t j = 0; j < rhs.m_columns; j++) {
            auto rowA = GetRow(i);
            const T* colB = rhs.GetColumn(j);
            
            T res = 0;
            for(size_t k = 0; k < m_columns; k++) {
                res += ( *(rowA.at(k)) * colB[k]);
            }
            result(i, j) = res;
        }
    }
    
    return result;
}

#endif

template <class T>
Matrix<T> Matrix<T>::Transpose() const {
    Matrix<T> transpose(m_columns, m_rows);

    #pragma omp parallel for
    for (int i = 0; i < m_rows; i++)
    {
        for (int j = 0; j < m_columns; j++)
        {
            transpose(j,i) = Get(i, j);   
        }
    }

    return transpose;
}

template <class T>
std::ostream & operator<<(std::ostream & out, const Matrix<T> & m) {
    for (size_t i = 0; i < m.Rows(); i++) {
        for(size_t j = 0; j < m.Columns(); j++) {
            out << m(i, j) << ' ';
        }
        out << std::endl;
    }
    
    return out;
}

