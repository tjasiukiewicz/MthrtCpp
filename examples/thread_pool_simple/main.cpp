#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <cstddef>
#include <vector>
#include <queue>
#include <condition_variable>
#include <functional>

class ThreadPool {
    using glock_t = std::lock_guard<std::mutex>;
    using ulock_t = std::unique_lock<std::mutex>;
public:

    explicit ThreadPool(size_t num_threads = 0): doStop{false} {
        auto cpu_cores = std::thread::hardware_concurrency();
        num_threads = (!num_threads ? cpu_cores: num_threads);
        num_threads = (!num_threads ? 2: num_threads);
        while(num_threads--) {
            workers_threads.emplace_back(&ThreadPool::worker, this);
        }
    }

    void enqueue(std::function<void()> func) {
        if(doStop) {
            // TODO: Powinień być raczej wyjątek...
            std::cerr << "Dude! Don't enqueue on stopped ThreadPool!\n";
            return;
        }
        {
            glock_t lck(mtx);
            jobs.push(func);
        }
        cond.notify_one();
    }

    std::size_t queue_size() const {
        glock_t lck(mtx);
        return jobs.size();
    }

    std::size_t workers_size() const {
        glock_t lck(mtx);
        return workers_threads.size();
    }

    void gentle_stop() {
        glock_t lck(mtx);
        doStop = true;
        cond.notify_all();
    }

    ~ThreadPool() {
        gentle_stop();
        for(auto& thr: workers_threads) {
            thr.join();
        }
    }

private:

    void worker() {
        decltype(jobs)::value_type func;
        for(;;) {
            {
                ulock_t lck(mtx);
                cond.wait(lck, [&] { return doStop || !jobs.empty(); });
                //if(doStop) { // Immediate stop.
                if(doStop && jobs.empty()) { // Gentle stop
                    return;
                }
                func = jobs.front();
                jobs.pop();
            }
            func();
        }
    }

    std::vector<std::thread> workers_threads;
    std::queue<std::function<void()>> jobs;
    mutable std::mutex mtx;
    std::condition_variable cond;
    bool doStop;
};

void foo() {
    std::cout << "Start foo...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    std::cout << "End foo...\n";
}

int main() {
    ThreadPool tpool{2};
    for(auto i = 0U; i < 10; ++i) {
        tpool.enqueue(foo);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}
