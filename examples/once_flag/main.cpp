#include <iostream>
#include <thread>
#include <mutex>

std::once_flag flag;

void worker() {
    std::call_once(flag, []{ std::cout << "Call me once!\n";});
    std::cout << "Work!\n";
}

int main() {
    std::thread thr1(worker);
    std::thread thr2(worker);
    std::thread thr3(worker);
    thr1.join();
    thr2.join();
    thr3.join();
}
