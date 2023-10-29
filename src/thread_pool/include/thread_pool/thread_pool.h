/**
 * @projectName IntelligentHanding
 * @file ThreadPool.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-09-28 23:30
 */

#ifndef INTELLIGENTHANDING_THREADPOOL_H
#define INTELLIGENTHANDING_THREADPOOL_H

#include <vector>
#include <queue>
#include <future>
#include <memory>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <stdexcept>

namespace armor_auto_aiming {
class ThreadPool {
public:
    explicit ThreadPool(size_t pool_size=1);
    ~ThreadPool();

    /**
     * @brief 将任务加入到队列, 并唤醒一个线程
     *
     * @tparam Function
     * @tparam Args
     * @param func
     * @param args
     * @return
     */
    template<typename Function, typename... Args>
    auto enqueue(Function&& func, Args&&... args)
    -> std::future<typename std::result_of<Function(Args...)>::type>;

private:
    std::vector<std::thread> works;  // 具体工作线程
    std::queue<std::function<void()>> tasks;  // 任务

    std::mutex tasks_mutex;  // 锁
    std::condition_variable condition;

    bool stop = false;
};


inline ThreadPool::ThreadPool(size_t pool_size) {
    for (size_t i = 0; i < pool_size; i++) {
        works.emplace_back([this](){
            while (true) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(this->tasks_mutex);

                    this->condition.wait(lock, [this]() ->bool {
                        return this->stop || !this->tasks.empty();
                    });

                    if (this->stop && this->tasks.empty())
                        return;

                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }
                task();
            }
        });
    }
}

inline ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(this->tasks_mutex);
        this->stop = true;
    }

    condition.notify_all();
    for (std::thread& work: this->works)
        work.join();
}

template<typename Function, typename... Args>
auto ThreadPool::enqueue(Function &&func, Args &&...args)
-> std::future<typename std::result_of<Function(Args...)>::type>
{
    using return_type = typename std::result_of<Function(Args...)>::type;  // 获取传入 task 的返回类型

    auto task_ptr = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<Function>(func), std::forward<Args>(args)...)
    );  // 通过传入参数(Func, Args)创建任务

    std::future<return_type> result = task_ptr->get_future();  // 获取任务的 future

    // 将任务加入到 this->tasks
    {
        std::unique_lock<std::mutex> lock(this->tasks_mutex);

        if (this->stop)
            throw std::runtime_error("enqueue on stopped ThreadPool(enqueue()时停止线程池)");

        tasks.emplace([task_ptr]() -> void { (*task_ptr)(); });
    }

    // 唤醒一个等待线程
    condition.notify_one();
    return result;  // 返回任务的 future
}
}

#endif //INTELLIGENTHANDING_THREADPOOL_H
