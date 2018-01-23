#include <iostream>
#include <glm/glm.hpp>
#include "Matrix.hpp"

int main() {
	Matrix<float> m(4, 4);

	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			std::cout << m.Get(i, j) << std::endl;
    
	return 0;
}
