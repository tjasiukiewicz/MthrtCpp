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
    for(auto i = 0U; i < num_elements; ++i) {
        vec.emplace_back(random());
    }
}

int serial_sum(const std::vector<int> & vec) {
    return std::accumulate(vec.cbegin(), vec.cend(), 0);
}

int thread_sum(const std::vector<int> & vec) {
    auto thread_counter = std::thread::hardware_concurrency();
    auto vec_size = vec.size();
    auto step = vec_size / thread_counter;
    std::future<int> results[thread_counter];
    // map
    for (auto i = 0U; i < thread_counter; ++i) {
    	    results[i] = std::async(std::launch::async,
    	    	[&vec](unsigned start, unsigned stop) {
    			return std::accumulate(vec.cbegin() + start, vec.cbegin() + stop, 0);
    	    	}, i * step, (i + 1) * step);
    }
    
    // reduce
    int sum = 0;
    for(auto & ftr: results) {
    	    sum += ftr.get();
    } 
    
    return sum;
}

int main() {
    srand(time(NULL));
    const auto num_elements = std::thread::hardware_concurrency() * 524288;
    // 4 cores, 32-bit int ~ 8192 (8 * 1024)
    // -,,- opt. O3 ~ 524288 (512 * 1024)
    std::vector<int> vec;
    vec.reserve(num_elements);
    fill_vec(vec, num_elements);
    std::cout << "Result serial_sum: " << serial_sum(vec) << '\n';
    std::cout << "Result thread_sum: " << thread_sum(vec) << '\n';
    check_time("Serial sum: ", serial_sum, vec);
    check_time("Thread sum: ", thread_sum, vec);
    std::cout << "Vector size (elements): " << vec.size() << '\n';
}
