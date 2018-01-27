#include <iostream>
#include <utility>
#include <chrono>
#include "Matrix.hpp"
#include "Rand.hpp"

using namespace std;

std::mt19937 Rand::sBase( time(nullptr) );
std::uniform_real_distribution<float> Rand::sFloatGen;
char sectionBreak[81];

Matrix<float> generateRandomMatrix(int rowsMin, int rowsMax, int colsMin, int colsMax);
void profileLargeMatrixMultiplication();
void profileLargeMatrixTranspose();

int main() {
    std::fill(sectionBreak, sectionBreak + 79, '=');
    sectionBreak[79] = '\n';
    sectionBreak[80] = '\0';


	cout << "This program measures the execution time of my Matrix class." << endl;
	cout << sectionBreak;
	profileLargeMatrixMultiplication();
	cout << sectionBreak;
	profileLargeMatrixTranspose();
	cout << sectionBreak;
	return 0;
}

void profileLargeMatrixMultiplication() {
	cout << "This function measures the execution time of very large matrix multiplication" << endl;
	const int rows = 1600;
	const int columns = 2000;

	auto A = generateRandomMatrix(rows, rows, columns, columns);
	auto B = generateRandomMatrix(columns, columns, rows, rows);

	cout << "Matrix A is " << A.Rows() << "x" << A.Columns() << endl;
	cout << "Matrix B is " << B.Rows() << "x" << B.Columns() << endl;

	auto begin = chrono::high_resolution_clock::now();
	auto result = A * B;
	auto end = chrono::high_resolution_clock::now();

	cout << endl;
	cout << "Multiplication of a " <<  A.Rows() << "x" << A.Columns() << " matrix and a "
		<< B.Rows() << "x" << B.Columns() << " matrix took " 
		<< chrono::duration_cast<chrono::microseconds>(end - begin).count() / 1000.f
		<< " ms" << endl; 
}

void profileLargeMatrixTranspose() {
	cout << "This function measures the execution time of very large matrix transposition" << endl;
	const int rows = 10;
	const int columns = 20;

	auto A = generateRandomMatrix(rows, rows, columns, columns);
	cout << "Matrix A is " << A.Rows() << "x" << A.Columns() << endl;

	auto begin = chrono::high_resolution_clock::now();
	auto B = A.Transpose();
	auto end = chrono::high_resolution_clock::now();

	cout << "The transpose of matrix A is " << B.Rows() << "x" << B.Columns() << endl;
	cout << endl;
	cout << "Transpose took " << chrono::duration_cast<chrono::microseconds>(end - begin).count() / 1000.f
		<< " ms" << endl;


}

Matrix<float> generateRandomMatrix(int rowsMin, int rowsMax, int colsMin, int colsMax) {
    int rows = Rand::randInt(rowsMin, rowsMax);
    int columns = Rand::randInt(colsMin, colsMax);

    Matrix<float> A(rows, columns);

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < columns; j++) {
            A(i,j) = Rand::randInt(100);
        }
    }

    return A;
}
