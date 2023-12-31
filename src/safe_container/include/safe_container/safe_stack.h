/**
 * @project_name auto_aim
 * @file safe_stack.h
 * @brief
 * @author yx
 * @date 2023-12-24 17:19:01
 */

#ifndef ARMOR_AUTO_AIM_SAFE_CONTAINER_SAFE_STACK_H
#define ARMOR_AUTO_AIM_SAFE_CONTAINER_SAFE_STACK_H

#include <vector>
#include <mutex>
#include <condition_variable>

namespace armor_auto_aim {
template<typename T>
class SafeStack {
public:
    SafeStack() =default;

    explicit SafeStack(const int& n)
        : m_is_fixed(true),
          m_n(n) {

    }

    void push(T value) {
        std::shared_ptr<T> data(std::make_shared<T>(std::move(value)));
        {
            std::lock_guard<std::mutex> lk(m_mutex);
            m_vec.push_back(data);
            if (m_is_fixed) {
//                if (m_vec.size() > m_n) m_vec.
            }
        }
        m_condition.notify_one();
    }

    bool tryPop(T& value) {
        std::lock_guard<std::mutex> lk(m_mutex);
        if (m_vec.empty()) return false;

        value = std::move(m_vec.back());
        m_vec.pop_back();
        return true;
    }

    std::shared_ptr<T> tryPop() {

    }

    void tryPopAndClear(T& value) {

    }

    std::shared_ptr<T> tryPopAndClear() {

    }

    void tryPopRecent(T& value) {

    }

    std::shared_ptr<T> tryPopRecent() {

    }

    bool empty() const {

    }

    void clear() {

    }
private:
    mutable std::mutex m_mutex;
    std::condition_variable m_condition;
    std::vector<std::shared_ptr<T>> m_vec;
    bool m_is_fixed = false;
    int m_n = -1;
};
}

#endif //ARMOR_AUTO_AIM_SAFE_CONTAINER_SAFE_STACK_H
