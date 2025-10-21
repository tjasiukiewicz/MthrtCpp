#include <future>
#include <iostream>

void consume(std::future<int> && ftr) {
	std::cout << ftr.get() << '\n';
}

int main() {
	//std::future<int> a1 = std::async(std::launch::deferred, []() { return 42; });
	std::future<int> a1 = std::async(std::launch::async, []() { return 42; });
	//auto ftr2 = a1;
	consume(std::move(a1));
}
