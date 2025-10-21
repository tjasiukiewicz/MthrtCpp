#include <thread>
#include <iostream>
#include <string>

void work1() {
	auto s = "Siema";
	std::cout << "Hello from Thread! 1\n" << s << std::endl;
}

void work2() {
	std::cout << "Hello from Thread! 2\n" << std::endl;
}

struct X {
	void operator()() {
		std::cout << "Hello from X\n";
	}
	void operator()(short a) {
		std::cout << "Hello from X myshort = " << a << '\n';
	}

	void run(double b) {
		std::cout << "Hello from X b = " << b << '\n';
	}

};

void work3(int a) {
	std::cout << "Work3 int = " << a << '\n';
}

void work4(int & a) {
	std::cout << "Work3 int = " << a << '\n';
}


int main() {
	auto x = X();
	int a = 33;

	auto thr1 = std::thread(work1);
	auto thr2 = std::thread(work2);
	auto thr3 = std::thread([]() { std::cout << "Lambda1\n"; });
	auto thr4 = std::thread(x);
	auto thr5 = std::thread(work3, 42);

	auto thr6 = std::thread(work4, std::ref(a));
	auto thr7 = std::thread(&X::run, &x, 66.9);
	auto thr8 = std::thread(x, 67);

	thr8.detach();
/*
	if (thr8.joinable()) {
		thr8.join();
	}

	thr7.join(); thr6.join(); thr5.join(); thr4.join(); thr3.join(); thr2.join(); thr1.join();
*/

	for(auto& t: {&thr8, &thr7, &thr6, &thr5, &thr4, &thr3, &thr2, &thr1} ) {
		if (t->joinable()) {
			t->join();
		}
	}
}
