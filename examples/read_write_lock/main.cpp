#include <shared_mutex>
#include <thread>
#include <iostream>


static std::shared_mutex mtx;
static int counter = 100;

void incrementCounter() {
    for(auto i = 0U; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
        std::lock_guard<std::shared_mutex> l(mtx);
        ++counter;
    }
}

// Łamię DRY :-)
void decrementCounter() {
    for(auto i = 0U; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(130));
        std::lock_guard<std::shared_mutex> l(mtx);
        --counter;
    }
}

void showCounter() {
    for(auto i = 0U; i < 20; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::shared_lock<std::shared_mutex> l(mtx);
        std::cout << counter << std::endl;
    }
}

int main() {
    std::thread t1{incrementCounter};
    std::thread t2{decrementCounter};
    std::thread t3{showCounter};

    for(auto * t: {&t1, &t2, &t3}) {
        t->join();
    }
}


