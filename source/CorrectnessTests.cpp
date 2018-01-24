#include <iostream>
#include <Eigen/Dense>
#include "Matrix.hpp"

using namespace std;
typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> EigenMat;

bool compareMatrices(const Matrix<float> & A, const EigenMat & B);

int main() {
	Matrix<float> m1(4, 10, 113.123123123f);
    Matrix<float> m2(10, 4, 12.f);
    Matrix<float> m3(11, 4, 10.f);
    
    auto res1 = m1 * m2;
    try {
        auto res2 = m1 * m3;
    } catch (invalid_argument & e) {
        cout << "Caught exception" << endl;
    }

    
	return 0;
}

bool compareMatrices(const Matrix<float> & A, const EigenMat & B) {
    for (size_t i = 0; i < A.Rows(); i++) {
        for (size_t j = 0; j < A.Columns(); j++) {
            if(A(i, j) != B(i, j)) return false;
        }
    }
    return true;
}
