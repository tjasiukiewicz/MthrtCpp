#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <chrono>

std::mutex mtx;
std::condition_variable cv;

int counter = 0;

void decrementor() {
	for(auto i = 0U; i < 5; ++i) {
		{
			std::unique_lock<std::mutex> lk(mtx);
			cv.wait(lk, [] { return counter > 0; });
			counter -= 1;
		}

		std::cout << "dec" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void incrementor() {
	for(auto i = 0U; i < 10; ++i) {
		{
			std::lock_guard<std::mutex> _(mtx);
			counter += 1;
			cv.notify_one();
		}

		std::cout << "inc" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(90));
	}
}

int main() {
	auto thr1 = std::thread(decrementor);
	auto thr2 = std::thread(decrementor);
	auto thr3 = std::thread(incrementor);

	for(auto & t: {&thr3, &thr2, &thr1}) {
		t->join();
	}
}
