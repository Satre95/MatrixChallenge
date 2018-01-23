#pragma once

#include <stdexcept>

template <class T>
class Matrix
{
public:
	Matrix(size_t w, size_t h);
	~Matrix();
	
	/// Fetches the element at the given coordinates
	const T & Get(size_t x, size_t y) const;
	/// Fetches the element at the given coordinates
	T & Get(size_t x, size_t y);
private:
	/// Converts the 2D element coord to a 1D index
	size_t Index(const size_t & x, const size_t & y) const;

	/// The # of columns in this matrix.
	const size_t m_width;
	/// The # of rows in this matrix.
	const size_t m_height;
	/**
	 * The array holding the elements.
	 * 1D array is more efficient than a 2D array with 
	 * separately allocated lines.
	 */
	T * m_data = nullptr;
	
};

template <class T>
Matrix<T>::Matrix(size_t w, size_t h): m_width(w), m_height(h) {
	m_data = new T[w * h];
	std::fill(m_data, m_data + (w * h), 0);
}

template <class T>
Matrix<T>::~Matrix() {
	delete[] m_data;
}

template <class T>
size_t Matrix<T>::Index(const size_t & x, const size_t & y) const {
	return x + m_width * y;
}

template <class T>
const T & Matrix<T>::Get(size_t x, size_t y) const {
	if(x >= m_width || y >= m_height) throw std::invalid_argument( "Invalid element coordiante" );
	return m_data[Index(x, y)];
}

template <class T>
T & Matrix<T>::Get(size_t x, size_t y) {
	if(x >= m_width || y >= m_height) throw std::invalid_argument( "Invalid element coordiante" );
	return m_data[Index(x, y)];
}
