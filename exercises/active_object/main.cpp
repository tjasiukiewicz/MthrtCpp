#include <thread>
#include <future>
#include <queue>
#include <mutex>
#include <iostream>
#include <functional>
#include <chrono>

class ActiveObject {
public:
	using submit_t = void (ActiveObject::*)(std::string);

	ActiveObject() 
		: stop{false} {
		work_thread = std::thread(&ActiveObject::work_loop, this);
	}

	~ActiveObject() {
		doStop();
		work_thread.join();
	}

	void process(submit_t mthd, std::string msg) {
		std::cout << "Submited" << std::endl;

		std::lock_guard<std::mutex> _(mtx);
		work_queue.push(std::packaged_task<void(void)>(std::bind(mthd, this, msg)));
	}

	void doStop() {
		stop = true;
	}

	void work1(std::string msg) {
		{
			std::lock_guard<std::mutex> _(out_mutex);
			std::cout << "Work1 start: " << msg << '\n';
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
		{
			std::lock_guard<std::mutex> _(out_mutex);
			std::cout << "Work1 stop:  " << msg << '\n';
		}
	}
	void work2(std::string msg) {
		{
			std::lock_guard<std::mutex> _(out_mutex);
			std::cout << "Work2 start: " << msg << '\n';
		}
		std::this_thread::sleep_for(std::chrono::seconds(2));
		{
			std::lock_guard<std::mutex> _(out_mutex);
			std::cout << "Work2 stop:  " << msg << '\n';
		}
	}
	void work3(std::string msg) {
		{
			std::lock_guard<std::mutex> _(out_mutex);
			std::cout << "Work3 start: " << msg << '\n';
		}
		std::this_thread::sleep_for(std::chrono::seconds(3));
		{
			std::lock_guard<std::mutex> _(out_mutex);
			std::cout << "Work3 stop:  " << msg << '\n';
		}
	}
private:
	void work_loop() {
		while(! stop) {
			std::unique_lock<std::mutex> lk(mtx);
			if (not work_queue.empty()) {
				auto task = std::move(work_queue.front());
				work_queue.pop();
				lk.unlock();
				auto ftr = task.get_future();
				task();
				ftr.get();
			}	
		}
	}
	std::mutex mtx;
	std::queue<std::packaged_task<void(void)>> work_queue;
	std::mutex out_mutex;
	std::thread work_thread;
	bool stop;
};

int main() {
	ActiveObject ac{};
	ac.process(&ActiveObject::work1, "first task");
	ac.process(&ActiveObject::work2, "second task");
	ac.process(&ActiveObject::work3, "thrid task");

	std::this_thread::sleep_for(std::chrono::seconds(5));
}
