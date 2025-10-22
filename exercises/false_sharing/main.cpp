#include <iostream>
#include <numeric>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <string>
#include <future>

template<typename Func, typename... TT>
void check_time(const std::string& message, Func func, TT&& ...tt) {
    using namespace std::chrono;
    using namespace std;
    cout << message;
    auto start_time = high_resolution_clock::now();

    func(forward<TT>(tt)...);

    auto end_time = high_resolution_clock::now();
    cout << "Exec time: "
        << duration_cast<microseconds>(end_time - start_time).count()
        << " us.\n";
}

void fill_vec(std::vector<int>& vec, size_t num_elements) {
    // TODO: Fill the vector with random values
}

int serial_sum(const std::vector<int> & vec) {
    // TODO: Sum values (Hint: numeric, accumulate)
    int sum = 0;
    return sum;
}

int thread_sum(const std::vector<int> & vec) {
    // TODO: Map data to threads
    // TODO: Reduce partial results
    
    int sum = 0;
    return sum;
}

int main() {
    srand(time(NULL));
    const auto num_elements = std::thread::hardware_concurrency() * 512; // 1024, 2048...
    std::vector<int> vec;
    vec.reserve(num_elements);
    fill_vec(vec, num_elements);
    std::cout << "Result serial_sum: " << serial_sum(vec) << '\n';
    std::cout << "Result thread_sum: " << thread_sum(vec) << '\n';
    check_time("Serial sum: ", serial_sum, vec);
    check_time("Thread sum: ", thread_sum, vec);
    std::cout << "Vector size (elements): " << vec.size() << '\n';
}
