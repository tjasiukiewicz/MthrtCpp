#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <queue>
#include <string>
#include <optional>
#include <memory>

class ThrQueue {
public:
	ThrQueue()
		: doStop{false} {}

	void push(int data) {
		std::lock_guard<std::mutex> _(mtx);
		data_queue.push(data);
		cv.notify_one();
	}

	bool pop(int & box) {
		int data;

		std::unique_lock<std::mutex> lk(mtx);
		cv.wait(lk, [this] { return (not data_queue.empty()) or doStop; });

		if (doStop and data_queue.empty()) {
			return false;
		}
		box = data_queue.front();
		data_queue.pop();

		return true;

	}

	std::optional<int> pop() {
		std::unique_lock<std::mutex> lk(mtx);
		cv.wait(lk, [this] { return (not data_queue.empty()) or doStop; });

		if (doStop and data_queue.empty()) {
			return {};
		}
		std::optional<int> data = std::make_optional<int>(data_queue.front());
		data_queue.pop();

		return data;
	}

	std::shared_ptr<int> pop_shared() {
		std::unique_lock<std::mutex> lk(mtx);
		cv.wait(lk, [this] { return (not data_queue.empty()) or doStop; });

		if (doStop and data_queue.empty()) {
			return {};
		}
		std::shared_ptr<int> data = std::make_shared<int>(data_queue.front());
		data_queue.pop();

		return data;
	}

	std::optional<int> imm_pop() {
		if (mtx.try_lock()) {
			{
				std::lock_guard<std::mutex> _(mtx, std::adopt_lock);
				std::optional<int> data = std::make_optional<int>(data_queue.front());
				data_queue.pop();

				return data;
			}
		}
		return {};
	}

	void shutdown() {
		doStop = true;
		cv.notify_all();
	}
	
	bool stopped() const {
		return doStop;
	}

private:
	std::mutex mtx;
	std::condition_variable cv;
	std::queue<int> data_queue;
	bool doStop;
};

int main() {
	auto q = ThrQueue();

	auto work_lambda = [&]() {
		while(not q.stopped()) {
			auto data = q.pop();
			if (data.has_value()) {
				std::cout << " rcv: " << *data << std::endl;
			} 
		}
	};

	auto thr1 = std::thread(work_lambda);
	auto thr2 = std::thread(work_lambda);

	for (auto i = 0U; i < 20; ++i) {
		q.push(1000 + i);
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	std::cout << "SHUTDOWN!!!\n";
	q.shutdown();

	thr2.join(); thr1.join();
}
