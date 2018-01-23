#pragma once

template <class T>
class Matrix
{
public:
	Matrix(size_t w, size_t h);
	~Matrix();
	
private:
	/// Converts the 2D element coord to a 1D index
	size_t Index(const size_t & x, const size_t & y) const;


	/// The # of columns in this matrix.
	const size_t m_width;
	/// The # of rows in this matrix.
	const size_t m_height;

	/**
	 *  The array holding the elements.
	 * 1D array is more efficient than a 2D array with separately allocated
	 * lines.
	 */
	T * m_data = nullptr;
	
};

template <class T>
Matrix<T>::Matrix(size_t w, size_t h): m_width(w), m_height(h) {}

template <class T>
Matrix<T>::~Matrix() {
	delete[] m_data;
}

template <class T>
size_t Matrix<T>::Index(const size_t & x, const size_t & y) const {
	return x + m_width * y;
}