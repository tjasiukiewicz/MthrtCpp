#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <string>

class Barier {
public:
	Barier(int counter_)
		: max_counter{counter_}, counter{counter_}, mtx{}, cv{}  {
	}

	void wait() {
		std::unique_lock<std::mutex> lk(mtx);
		if (counter == 0) {
			counter = max_counter;
		}
		--counter;
		cv.wait(lk, [this]{ return counter == 0; });
		lk.unlock();
		cv.notify_all();
	}
private:
	const int max_counter;
	int counter;
	std::mutex mtx;
	std::condition_variable cv;
};

int main() {
	auto bar = Barier(3);

	auto work_lam = [](Barier & b, std::string name) {
		b.wait();
		std::cout << name << '\n';
	};

	auto thr1 = std::thread(work_lam, std::ref(bar), "thr1");
	auto thr2 = std::thread(work_lam, std::ref(bar), "thr2");

	std::cout << "in main()\n";

	auto thr3 = std::thread(work_lam, std::ref(bar), "thr3");
	auto thr4 = std::thread(work_lam, std::ref(bar), "thr4");
	
	thr4.join();
	thr3.join();
	thr2.join();
	thr1.join();
}
