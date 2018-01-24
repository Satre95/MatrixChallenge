#pragma once

#include <stdexcept>

template <class T>
class Matrix
{
public:
    Matrix(size_t numRows, size_t numCols, T defaultValue = 0);
	~Matrix();
	
	/// Fetches the element at the given coordinates
	const T & Get(size_t row, size_t col) const;
	/// Fetches the element at the given coordinates
	T & Get(size_t row, size_t col);
    size_t Rows() { return m_columns; }
    size_t Columns() { return m_rows; }
    Matrix operator*(const Matrix & rhs);
    
private:
	/// Converts the 2D element coord to a 1D index
	size_t Index(const size_t & x, const size_t & y) const;
    
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
	m_data = new T[numRows * numCols];
	std::fill(m_data, m_data + (numRows * numCols), defaultValue);
}

template <class T>
Matrix<T>::~Matrix() {
	delete[] m_data;
}

template <class T>
size_t Matrix<T>::Index(const size_t & row, const size_t & col) const {
    return col * m_rows + row; // Matrix is stored in COLUMN major
}

template <class T>
const T & Matrix<T>::Get(size_t row, size_t col) const {
	if(row >= m_rows || col >= m_columns) throw std::invalid_argument( "Invalid element coordiante" );
	return m_data[Index(row, col)];
}

template <class T>
T & Matrix<T>::Get(size_t row, size_t col) {
    if(row >= m_rows || col >= m_columns) throw std::invalid_argument( "Invalid element coordiante" );
    return m_data[Index(row, col)];
}

template <class T>
Matrix<T> Matrix<T>::operator*(const Matrix<T> & rhs) {
    //My # of columns (width) must equal # of rows (height) in other matrix.
    if(m_columns != rhs.m_rows)
        throw std::invalid_argument("Invalid argument. Width of first matrix must match height of second matrix");
    
    //First pass attempt.
    //TODO: Optimize across threads (OpenMP) & implement vector processing.
    
    //Note that the length of each row is num columns and vice versa.
    
    //Iterate over the elements of the result matrix.
    Matrix<T> result(m_rows, rhs.m_columns);
    
    
    return result;
}
