#include <thread>
#include <future>
#include <queue>
#include <mutex>
#include <iostream>
#include <functional>

class ActiveObject {
public:
	using submit_t = void (ActiveObject::*)(std::string);

	ActiveObject() { // thread
	}

	~ActiveObject() {
	}

	void process(submit_t mthd, std::string msg) {
		std::cout << "Submited" << std::endl;

		std::lock_guard<std::mutex> _(mtx);
		work_queue.push(std::packaged_task<void(void)>(std::bind(mthd, this, msg)));
	}

	void work1(std::string msg) {}
	void work2(std::string msg) {}
	void work3(std::string msg) {}
private:
	void work_loop() {
		for(;;) {
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
};

int main() {
	ActiveObject ac;
	ac.process(&ActiveObject::work1, "first task");
	ac.process(&ActiveObject::work2, "second task");
	ac.process(&ActiveObject::work1, "thrid task");
}
