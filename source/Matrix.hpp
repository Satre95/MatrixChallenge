#pragma once

#include <stdexcept>
#include <cstdlib>
#include <iostream>
#include <vector>
#ifdef USE_INTRINSICS
#include "xmmintrin.h"
#include "pmmintrin.h"
#endif

#define ROUND_UP(NUM, FACTOR) ((((NUM) + (FACTOR) - 1) / (FACTOR)) * (FACTOR))
#define CHECK_PTR(PTR) ((uintptr_t)PTR % 0x10 == 0)

template <class T>
class Matrix
{
public:
    Matrix(size_t numRows = 4, size_t numCols = 4, T defaultValue = 0);
    // Matrix(const Matrix & other);
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

    /// The actual amount of rows allocated, for 16-byte alignment.
    size_t m_rowsAllocated = 0;
    /// The actual amount of columns allocated for 16-byte alignment.
    size_t m_columnsAllocated = 0;

	/**
	 * The array holding the elements.
	 * 1D array is more efficient than a 2D array with 
	 * separately allocated lines.
	 */
	T * m_data = nullptr;

    /// A raw pointer holding the first addr of the aligned allocated mem. Only used for
    /// allocation and deallocation.
    void * m_rawData = nullptr;
    /// The size of the row in bytes, including padding for 16-byte alignment.
    size_t m_rowSize = 0;
    /// The size of the column in bytes, including padding for 16-byte alignment.
    size_t m_columnSize = 0;
	
};

template <class T>
Matrix<T>::Matrix(size_t numRows, size_t numCols, T defaultValue): m_rows(numRows), m_columns(numCols) {
    if(numRows * numCols == 0) throw std::invalid_argument("Error. Cannot create matrix with 0 dimension(s)");

    //SSE2 requires floating point mem to be aligned on 16-byte boundary, so round up.
    numRows = ROUND_UP(numRows, 16);
    numCols = ROUND_UP(numCols, 16);
    m_columnsAllocated = numCols; m_rowsAllocated = numRows;
    m_columnSize = ROUND_UP(numRows * sizeof(T), 16);
    m_rowSize = ROUND_UP(numCols * sizeof(T), 16);

    size_t byteAmount = m_rowSize * m_rows * m_columnSize * m_columns;

    m_rawData = std::malloc(byteAmount);
    m_data = (T*)(((uintptr_t)m_rawData + 15) & ~(uintptr_t)0x0F);
	std::fill(m_data, m_data + (numRows * numCols), defaultValue);

    std::cout << "Raw Data Ptr: " << m_rawData << std::endl;
    std::cout << "Type Data Ptr: " << m_data << std::endl;
    std::cout << "m_data % 16 = " << (uintptr_t)m_data % 16 << std::endl;
}

// template <class T>
// Matrix<T>::Matrix(const Matrix<T> & other): m_rows(other.m_rows), m_columns(other.m_columns) {
//     //SSE2 requires floating point mem to be aligned on 16-byte boundary, so round up.
//     size_t byteAmount = m_rows * m_columns * sizeof(T);
//     //Round up to be a multiple of 16.
//     byteAmount = ((byteAmount + 15) / 16) * 16;
//     m_rawData = std::malloc(byteAmount);
//     m_data = (T*)(((uintptr_t)m_rawData + 15) & ~(uintptr_t)0x0F);
//     std::copy(other.m_data, other.m_data + (m_rows * m_columns), m_data);
// }

template <class T>
Matrix<T>::~Matrix() {
	// std::free(m_data);
    std::free(m_rawData);
    m_data = nullptr;
    m_rawData = nullptr;
}

template <class T>
size_t Matrix<T>::Index(const size_t & row, const size_t & col) const {
    return col * m_rowsAllocated + row; // Matrix is stored in COLUMN major
}

template <class T>
const T & Matrix<T>::Get(size_t row, size_t col) const {
	if(row >= m_rows || col >= m_columns)
        throw std::invalid_argument( "Invalid element coordinate" );
    auto index = Index(row, col);
    auto ptr = m_data + index;
    // if(!CHECK_PTR(ptr)) 
    //     std::cout << "UNALIGNED PTR!! Get" << std::endl;

	return *(m_data + Index(row, col));
}

template <class T>
T & Matrix<T>::Get(size_t row, size_t col) {
    if(row >= m_rows || col >= m_columns)
        throw std::invalid_argument( "Invalid element coordinate" );
    auto index = Index(row, col);
    auto ptr = m_data + index;
    // if(!CHECK_PTR(ptr)) 
    //     std::cout << "UNALIGNED PTR!! Get" << std::endl;

    return *(m_data + Index(row, col));
}

template <class T>
std::vector<T*> Matrix<T>::GetRow(size_t rowIndex) {
    // This method is called only by fns that error check, so no need to check rowIndex
    std::vector<T*> row;
    row.reserve(m_columns);
    for(size_t i = 0; i < m_columns; i++)
        row.push_back(&Get(rowIndex, i));
    
    return row;
}

// Since Matrix is stored in column major, just return a pointer to the first element in the column.
template <class T>
T* Matrix<T>::GetColumn(size_t colIndex) {
    // This method is called only by fns that error check, so no need to check colIndex
    auto ptr = m_data + colIndex * m_rowsAllocated;
    if(!CHECK_PTR(ptr)) 
        std::cout << "UNALIGNED PTR!! Get Col" << std::endl;
    return ptr;
}

// Since Matrix is stored in column major, just return a pointer to the first element in the column.
template <class T>
const T* Matrix<T>::GetColumn(size_t colIndex) const {
    // This method is called only by fns that error check, so no need to check colIndex
    auto ptr = m_data + colIndex * m_rowsAllocated;
    if(!CHECK_PTR(ptr)) 
        std::cout << "UNALIGNED PTR!! Get Col" << std::endl;
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

    //Since matrices are stored in column major, need to copy row to 
    //contiguous 16-byte aligned memory. Recycle the array
    void * rawRowData = std::malloc(m_columnSize + 15);
    T * rowData = (T*)(((uintptr_t)rawRowData + 15) & ~(uintptr_t)0x0F);

    // #pragma omp parallel for
    for (size_t i = 0; i < m_rows; i++) {
        for (size_t j = 0; j < rhs.m_columns; j++) {
            auto rowA = GetRow(i);
            //Copy the row elements into a contiguous array.
            for(size_t h = 0; h < m_columns; h++)
                rowData[h] = *(rowA.at(h));
            //Get the column data.           
            const T* colB = rhs.GetColumn(j);

            if(!CHECK_PTR(colB))
                std::cout << "UNALIGNED POINTER!!" << std::endl;
            
            T res = 0;
            size_t k = 0;
            
            for(k = 0; k + 3 < m_columns; k+=4) {
                __m128 colVec = _mm_load_ps(colB + k);
                __m128 rowVec = _mm_load_ps(rowData + k);
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

                res += static_cast<T>(temp);
               
            }
            for(; k < m_columns; k++)
                res += static_cast<T>(rowData[k] * colB[k]);
            
            result(i, j) = res;
        }
    }

    std::free(rawRowData);
    
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

