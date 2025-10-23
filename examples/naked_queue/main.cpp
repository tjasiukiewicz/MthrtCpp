#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <queue>
#include <string>

std::mutex mtx;
std::condition_variable cv;
std::queue<int> data_queue;
bool doStop = false;

void sender(int data) {
	std::lock_guard<std::mutex> _(mtx);
	data_queue.push(data);
	cv.notify_one();
}

void receiver(const std::string & name) {
	for(;;) {
		std::unique_lock<std::mutex> lk(mtx);
		cv.wait(lk, [] { return (! data_queue.empty()) or doStop; });

		/*
		if (doStop and data_queue.empty()) {
			break;
		}
		*/
		if (doStop) {
			break;
		}

		auto data = data_queue.front();
		data_queue.pop();

		lk.unlock();
		std::cout << name << " Rec: " << data << std::endl;
	}
}

void shutdown() {
	doStop = true;
	cv.notify_all();
}

int main() {
	auto thr1 = std::thread(receiver, "First  ");
	auto thr2 = std::thread(receiver, "Second ");

	for (auto i = 0U; i < 20; ++i) {
		sender(1000 + i);
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	shutdown();

	thr2.join(); thr1.join();
}
