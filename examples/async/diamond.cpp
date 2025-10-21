#include <future>
#include <iostream>

/* 
 * w1 -> w2 \
 *    \> w3 -> w4
 */

void worker1() {
	std::cout << "Worker1\n";
}

void worker2(std::shared_future<void> f) {
	f.get();
	std::cout << "Worker2\n";
}

void worker3(std::shared_future<void> f) {
	f.get();
	std::cout << "Worker3\n";
}

void worker4(std::shared_future<void> f1, std::shared_future<void> f2) {
	f1.get();
	f2.get();
	std::cout << "Worker4\n";
}

int main() {
	std::shared_future<void> ftr1 = std::async(std::launch::async, worker1);
	std::shared_future<void> ftr2 = std::async(std::launch::async, worker2, ftr1);
	std::shared_future<void> ftr3 = std::async(std::launch::async, worker3, ftr1);
	auto ftr4 = std::async(std::launch::async, worker4, ftr2, ftr3);


	ftr4.get();
}
