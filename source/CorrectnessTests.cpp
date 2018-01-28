#include <iostream>
#include <Eigen/Dense>
#include <utility>
#include "Matrix.hpp"
#include "Rand.hpp"

using namespace std;
template<class T>
using EigenMat = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>;

std::mt19937 Rand::sBase( time(nullptr) );
std::uniform_real_distribution<float> Rand::sFloatGen;

template<class T>
bool operator==(const Matrix<T> & A, const EigenMat<T> & B);
template<class T>
bool operator==(const Matrix<T> & A, const Matrix<T> & B);

template <class T>
pair<Matrix<T>, EigenMat<T>> generateRandomMatrix(int rowsMin = 10, int rowsMax = 20, int colsMin = 10, int colsMax = 20);

void testFloatMultiplication();
void testInvalidMultiplication();
void testTranspose();

char sectionBreak[81];

int main() {
    std::fill(sectionBreak, sectionBreak + 79, '=');
    sectionBreak[79] = '\n';
    sectionBreak[80] = '\0';

    cout << "This program tests the correctness of my Matrix multiplication & transpose functions." << endl;
    cout << "It does not measure execution time. See the profile program for execution timing." << endl;
    cout << "The Eigen mathematics library is used for verification." << endl;
    

    cout << sectionBreak;
    testFloatMultiplication();
    cout << sectionBreak;
    testInvalidMultiplication();
    cout << sectionBreak;
    testTranspose();
    cout << sectionBreak;
    
	return 0;
}

void testFloatMultiplication() {
    cout << "Testing multiplication of arbitrary size matrices." << endl;
    cout << "Multiplication is guaranteed to be possible for this test." << endl;

    // Make the matrices.    
    auto pair1 = generateRandomMatrix<float>(22,22,22,22);
    Matrix<float> & A = pair1.first;
    EigenMat<float> & ACond = pair1.second;
    
    auto pair2 = generateRandomMatrix<float>(A.Columns(), A.Columns(), 22, 22);
    Matrix<float> & B = pair2.first;
    EigenMat<float> & BCond = pair2.second;

    cout <<"\tMatrix A is " << A.Rows() << 'x' << A.Columns() << endl;
    cout <<"\tMatrix B is " << B.Rows() << 'x' << B.Columns() << endl;
    
    cout << endl;
    
    //Multiply!
    try {
        Matrix<float> result = A * B;
        EigenMat<float> resultsCond = ACond * BCond;
        
        //Verify.
        if(result == resultsCond)
            cout << "\tTest Passed!" << endl;
        else
            cout << "\tTest Failed!" << endl;
    } catch(std::invalid_argument & e) {
        cout << e.what() << endl;
        return;
    }
}

void testInvalidMultiplication() {
    cout << "This tests the case where the number of columns in matrix A "
        << "is not equal " << endl;
    cout << "to the number of columns in B, so multiplication is not possible." << endl;

    // Make the matrices.    
    auto pair1 = generateRandomMatrix<int>(200, 400, 200, 400);
    Matrix<int> & A = pair1.first;
    
    auto pair2 = generateRandomMatrix<int>(A.Columns() - 1, A.Columns() - 1, 200, 500);
    Matrix<int> & B = pair2.first;

    cout <<"\tMatrix A is " << A.Rows() << 'x' << A.Columns() << endl;
    cout <<"\tMatrix B is " << B.Rows() << 'x' << B.Columns() << endl;

    try {
        auto result = A * B;
        cout << "Test Failed!" << endl; // if this line is reached, error.

    } catch (std::invalid_argument & e) {
        cout << "\tSuccessfully caught exception" << endl;
        cout << endl;
        cout << "\tTest Passed!" << endl;
    }
}

void testTranspose() {
    cout << "Testing the transpose function of an arbitrarily large matrix." << endl;

    auto pair = generateRandomMatrix<int>(200, 400, 200, 400);
    Matrix<int> & A = pair.first;

    cout <<"\tMatrix A is " << A.Rows() << 'x' << A.Columns() << endl;

    auto B = A.Transpose();
    cout << "\tThe transpose of Matrix A is " << B.Rows() << "x" << B.Columns() << endl;

    cout << endl;

    //Verify transpose
    for (int i = 0; i < A.Rows(); ++i)
    {
        for (int j = 0; j < A.Columns(); ++j)
        {
            if(A(i,j) != B(j, i)) {
                cout << "\tTest Failed!" << endl;
                return;
            }
        }
    }

    //Transpose of a transpose should give back the original matrix.
    auto C = B.Transpose();
    if(!(A == C)) {
        cout << "\tTest Failed!" << endl;
        cout << "\tTranspose of a transpose should yield the orignal matrix." << endl;
        return;
    }

    cout << "\tTest Passed!" << endl;
}


template<class T>
bool operator==(const Matrix<T> & A, const EigenMat<T> & B) {
    for (size_t i = 0; i < A.Rows(); i++) {
        for (size_t j = 0; j < A.Columns(); j++) {
            if(A(i, j) != B(i, j)) return false;
        }
    }
    return true;
}

template<class T>
bool operator==(const Matrix<T> & A, const Matrix<T> & B) {
    for (size_t i = 0; i < A.Rows(); i++) {
        for (size_t j = 0; j < A.Columns(); j++) {
            if(A(i, j) != B(i, j)) return false;
        }
    }
    return true;
}

template <class T>
pair<Matrix<T>, EigenMat<T>> generateRandomMatrix(int rowsMin, int rowsMax, int colsMin, int colsMax) {
    int rows = Rand::randInt(rowsMin, rowsMax);
    int columns = Rand::randInt(colsMin, colsMax);

    Matrix<T> A(rows, columns);
    EigenMat<T> ACond(rows, columns);

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < columns; j++) {
            auto var = static_cast<T>(Rand::randInt(100));
            A(i, j) = var; ACond(i,j) = var;
        }
    }

    return make_pair(A, ACond);
}
