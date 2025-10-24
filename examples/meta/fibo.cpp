#include <iostream>

template<int Index>
struct Fibonacci {
	constexpr static int value = Fibonacci<Index - 2>::value + Fibonacci<Index - 1>::value;
};

template<>
struct Fibonacci<1> {
	constexpr static int value = 1;
};

template<>
struct Fibonacci<0> {
	constexpr static int value = 0;
};

int main() {
	std::cout << Fibonacci<7>::value << '\n';
}
