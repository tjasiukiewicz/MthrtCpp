#include <thread>
#include <iostream>
#include <chrono>
#include <mutex>
#include <string>

class Seaman {
public:
	static constexpr int tmout = 10;

	Seaman(int road_width)
		: road_width{road_width}, stop{false}, position(road_width / 2) {
		left_thread = std::thread(&Seaman::walk, this, -1);
		right_thread = std::thread(&Seaman::walk, this, 1);
	}

	void walk(int delta) {
		while(true) {
			int pos;
			std::this_thread::sleep_for(std::chrono::milliseconds(tmout - delta));
			{
				std::lock_guard<std::mutex> _(mtx);
				position += delta;
				pos = position;

				if (((pos <= 0) or (pos >= (road_width + 1))) or (stop)) {
					stop = true;
					break;
				}
				show_road(pos);
			}
		}
	}
	~Seaman() {
		while(! stop) {} 
		left_thread.join();
		right_thread.join();
	}

private:
	void show_road(int pos) const {
		std::cout << '|' << std::string(pos - 1, '-')
			<< '*' << std::string(road_width - pos, '-')
			<< '|' << std::endl;
	}
	int position;
	int road_width;
	std::mutex mtx;
	std::thread left_thread;
	std::thread right_thread;
	bool stop;
};

int main() {
	auto seaman = Seaman(10);
}
