#include <iostream>
#include <glm/glm.hpp>
#include "Matrix.hpp"

using namespace std;
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
