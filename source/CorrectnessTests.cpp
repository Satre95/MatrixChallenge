#include <iostream>
#include <Eigen/Dense>
#include <utility>
#include "Matrix.hpp"
#include "Rand.hpp"

using namespace std;
typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> EigenMat;

std::mt19937 Rand::sBase( time(nullptr) );
std::uniform_real_distribution<float> Rand::sFloatGen;

bool operator==(const Matrix<float> & A, const EigenMat & B);
pair<Matrix<float>, EigenMat> generateRandomMatrix(int rowsMin = 10, int rowsMax = 20, int colsMin = 10, int colsMax = 20);

void testMultiplication();
void testInvalidMultiplication();

char sectionBreak[81];

int main() {
    std::fill(sectionBreak, sectionBreak + 79, '*');
    sectionBreak[79] = '\n';
    sectionBreak[80] = '\0';

    cout << "This program tests the correctness of my Matrix multiplication & transpose functions." << endl;
    cout << "It does not measure execution time. See the profile program for execution timing." << endl;
    cout << "The Eigen mathematics library is used for verification." << endl;
    

    cout << sectionBreak;
    testMultiplication();
    cout << sectionBreak;
    // testInvalidMultiplication();
    // cout << sectionBreak;
    
	return 0;
}

void testMultiplication() {
    cout << "Testing multiplication of arbitrary size matrices." << endl;
    cout << "Multiplication is guaranteed to be possible for this test." << endl;

    // Make the matrices.    
    auto pair1 = generateRandomMatrix(200, 300, 200, 300);
    Matrix<float> & A = pair1.first;
    EigenMat & ACond = pair1.second;
    
    auto pair2 = generateRandomMatrix(A.Columns(), A.Columns(), 200, 300);
    Matrix<float> & B = pair2.first;
    EigenMat & BCond = pair2.second;

    cout <<"\tMatrix A is " << A.Rows() << 'x' << A.Columns() << endl;
    cout <<"\tMatrix B is " << B.Rows() << 'x' << B.Columns() << endl;
    
    cout << endl;
    
    //Multiply!
    try {
        Matrix<float> result = A * B;
        EigenMat resultsCond = ACond * BCond;
        
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
    cout << "This tests the case where the number of columns in matrix A " << endl;
    cout << "is not equal to the number of columns in B, so multiplication " << endl;
    cout << "is not possible." << endl;

    // Make the matrices.    
    auto pair1 = generateRandomMatrix(200, 400, 200, 400);
    Matrix<float> & A = pair1.first;
    EigenMat & ACond = pair1.second;
    
    auto pair2 = generateRandomMatrix(A.Columns() - 1, A.Columns() - 1, 200, 500);
    Matrix<float> & B = pair2.first;
    EigenMat & BCond = pair2.second;

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

bool operator==(const Matrix<float> & A, const EigenMat & B) {
    for (size_t i = 0; i < A.Rows(); i++) {
        for (size_t j = 0; j < A.Columns(); j++) {
            if(A(i, j) != B(i, j)) return false;
        }
    }
    return true;
}

pair<Matrix<float>, EigenMat> generateRandomMatrix(int rowsMin, int rowsMax, int colsMin, int colsMax) {
    int rows = Rand::randInt(rowsMin, rowsMax);
    int columns = Rand::randInt(colsMin, colsMax);

    Matrix<float> A(rows, columns);
    EigenMat ACond(rows, columns);

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < columns; j++) {
            auto var = Rand::randInt(100);
            A(i, j) = var; ACond(i,j) = var;
        }
    }

    return make_pair(A, ACond);
}