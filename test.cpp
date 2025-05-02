#include "array.hpp"
#include <iostream>
using namespace nv;

template <typename T>
std::ostream& operator<<(std::ostream& os, const nv::array<T>& arr) {
	os << '[';
	for (size_t i = 0; i < arr.size(); ++i) {
		if (i > 0)
			os << ", ";
		os << arr[i];
	}
	os << ']';
	return os;
}

int main() {
	nv::array arr{1, 2, 3};
	std::cout << arr << std::endl;
	return 0;
}
