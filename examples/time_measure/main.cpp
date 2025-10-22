#include <iostream>
#include <chrono>

using namespace std;

// Dumb functions... 
int fast_fun() {
    int sum{};
    for(size_t i = 0; i < 1000; ++i) {
        sum += i;
    }
    return sum;
}

long slow_fun() {
    long sum{};
    for(size_t i = 0; i < 100000000; ++i) {
        sum += i;
    }
    return sum;
}

long other_fun(int z) {
    long sum{};
    for(int i = 0; i < z; ++i) {
        sum += i;
    }
    return sum;
}

template<typename Func, typename... TT>
void check_time(const string& message, Func func, TT&& ...tt) {
	using namespace chrono;
	cout << message << endl;

	auto start_time = high_resolution_clock::now();
	func(forward<TT>(tt)...);
	auto end_time = high_resolution_clock::now();
	
	cout << "Exec time: " 
		<< duration_cast<microseconds>(end_time - start_time).count()
		<< " us.\n";
}

int main() {
    cout << "Clock precision: "
        << (static_cast<double>(chrono::high_resolution_clock::period::num)
         / chrono::high_resolution_clock::period::den) << " sec.\n";
    check_time("Function fast_fun()", fast_fun);
    check_time("Function slow_fun()", slow_fun);
    check_time("Function other_fun()", other_fun, 9000);
}
