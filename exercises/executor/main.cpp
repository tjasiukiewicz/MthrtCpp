#include <iostream>
#include <chrono>
#include <thread>
#include <future>

struct Executor {
	// No instance, only static.
	Executor() = delete;
	~Executor() = delete;

	template<typename Duration, typename Function, typename... Args>
	static void periodic(Duration duration, Function && func, Args... args) {
		// TODO: Periodical execute func
	}

	template<typename Duration, typename Function, typename... Args>
	static void oneShoot(Duration duration, Function && func, Args... args) {
		// TODO: One exec func
	}

	static void doStop() {
		Executor::stop = true;
	}

	static void doStart() {
		Executor::stop = false;
	}
private:
	static bool stop;
};

bool Executor::stop = false;


void ping() {
	std::cout << "Ping!\n";
}

int main() {
	auto ftr1 = std::async(std::launch::async, [] {
			Executor::periodic(std::chrono::seconds(1), ping);
	});

	auto ftr2 = std::async(std::launch::async, [] {
			Executor::oneShoot(std::chrono::seconds(5), []{ Executor::doStop(); });
	});

	for (auto i = 0U; i < 10; ++i) {
		std::cout << "Work in main()\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
	//Executor::doStop();
	ftr1.get();
	ftr2.get();
}
