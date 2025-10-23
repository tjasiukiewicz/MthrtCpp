#include <future>
#include <iostream>
#include <thread>

void foo(int a) {
	std::cout << "I'm foo()\n";
}

int main() {
	auto pk1 = std::packaged_task<void(int)>([](int a) { std::cout << "P1" << a << "\n"; });
	auto f1 = pk1.get_future();

	auto thr1 = std::thread(std::move(pk1), 32);
	thr1.join();

	f1.get();

	std::cout << pk1.valid() << '\n';

	pk1 = std::packaged_task<void(int)>(foo);
	pk1(23);
	std::cout << pk1.valid() << '\n';
	//f1 = pk1.get_future();

	//pk1(33);

}
