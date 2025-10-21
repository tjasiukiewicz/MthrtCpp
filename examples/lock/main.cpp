#include <thread>
#include <iostream>
#include <chrono>
#include <mutex>
#include <cassert>

int counter = 65535;
std::mutex mtx;

void worker(int delta) {
	for(auto i = 0; i < 100; ++i) {
		std::this_thread::sleep_for(std::chrono::milliseconds(20));

		//mtx.lock(); // Tak nie nalezy!!!
		{
			std::lock_guard<std::mutex> _(mtx);
			counter += delta;
		}
		//mtx.unlock(); // Ojoj!!!
	}
}

int main() {
	mtx.unlock(); // opcjonalne w tym przykladzie

	auto thr1 = std::thread(worker, 1);
	auto thr2 = std::thread(worker, -1);

	thr2.join();
	thr1.join();

	assert(counter == 65535);
}
