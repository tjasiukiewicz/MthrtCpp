#include <iostream>

template<int V>
struct Factorial {
	constexpr static int value = V * Factorial<V - 1>::value;
};

template<>
struct Factorial<0> {
	constexpr static int value = 1;
};

int main() {
	std::cout << Factorial<4>::value << '\n';
}
