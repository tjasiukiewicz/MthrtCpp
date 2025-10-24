#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#include <queue>
#include <condition_variable>
#include <future>
#include <stdexcept>

class ThreadPool {
  public:
    ThreadPool(size_t = 0);

    template<class Function, class... Arguments>
    auto enqueue(Function&& f, Arguments&& ... args)
        -> std::future<typename std::result_of<Function(Arguments...)>::type>;

    ~ThreadPool();
  private:
    // need to keep track of threads so we can join them
    std::vector<std::thread> workers;
    // the task queue
    std::queue<std::function<void()>> tasks;

    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads)
    :   stop{false} {
    // Num cores or 2
    if(!threads) {
        auto thrds = std::thread::hardware_concurrency();
        threads = (thrds == 0 ? 2 : thrds);
    }

    while(threads-- > 0) {
        workers.emplace_back(
        [this] {
            for(;;) {
                decltype(tasks)::value_type task;
                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    this->condition.wait(lock,
                    [this] { return this->stop || !this->tasks.empty(); });

                    if(this->stop && this->tasks.empty())
                    { return; }

                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }
                task();
            }
        }
        );
    }
}

// add new work item to the pool
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&& ... args)
    -> std::future<typename std::result_of<F(Args...)>::type> {

    // C++17 invoke_result
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    std::future<return_type> result = task->get_future();
    {
        std::lock_guard<std::mutex> lock(queue_mutex);

        // don't allow enqueueing after stopping the pool
        if(stop) {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }

        tasks.emplace([task]() { (*task)(); });
    }
    condition.notify_one();

    return result;
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for(auto& worker: workers) {
        worker.join();
    }
}

int calculate(int a) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return a + 2;
}

int main() {
    ThreadPool threadPool(8);
    std::vector<std::future<int>> results;
    for(auto i = 0U; i < 100; ++i) {
        results.emplace_back(threadPool.enqueue(&calculate, i));
    }
    for(auto& ftr: results) {
        std::cout << ftr.get() << ' ';
    }
    std::cout << std::endl;
}
