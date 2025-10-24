#include <thread>
#include <future>
#include <queue>
#include <mutex>
#include <iostream>
#include <functional>
#include <vector>
#include <chrono>
#include <utility>

class ActiveObject {
public:
	using submit_t = void (ActiveObject::*)(std::string);
	using task_t = std::pair<int, std::packaged_task<void(void)>>;

	ActiveObject() 
		: stop{false} {
		work_thread = std::thread(&ActiveObject::work_loop, this);
	}

	~ActiveObject() {
		doStop();
		work_thread.join();
	}

	void process(int prio, submit_t mthd, std::string msg) {
		std::cout << "Submitted" << std::endl;

		std::lock_guard<std::mutex> _(mtx);
		work_queue.emplace(task_t(prio, std::bind(mthd, this, msg)));
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
	struct PrioComparator {
		bool operator()(const task_t & a, const task_t & b) {
			return a.first < b.first;
		}
	};
	void work_loop() {
		while(! stop) {
			std::unique_lock<std::mutex> lk(mtx);
			if (not work_queue.empty()) {
				auto & task = work_queue.top();
				auto tsk = std::move(const_cast<task_t::second_type&>(task.second));
				work_queue.pop();
				lk.unlock();
				auto ftr = tsk.get_future();
				tsk();
				ftr.get();
			}	
		}
	}
	std::mutex mtx;
	std::priority_queue<task_t, std::vector<task_t>, PrioComparator> work_queue;
	std::mutex out_mutex;
	std::thread work_thread;
	bool stop;
};

int main() {
	ActiveObject ac{};
	ac.process(3, &ActiveObject::work1, "first task");
	ac.process(1, &ActiveObject::work2, "second task");
	ac.process(2, &ActiveObject::work3, "thrid task");

	std::this_thread::sleep_for(std::chrono::seconds(5));
}
