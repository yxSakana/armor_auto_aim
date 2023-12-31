/**
 * @project_name auto_aim
 * @file safe_circular_queue.h
 * @brief
 * @author yx
 * @date 2023-12-25 09:19:55
 */

#ifndef ARMOR_AUTO_AIM_SAFE_CONTAINER_SAFE_CIRCULAR_QUEUE_H
#define ARMOR_AUTO_AIM_SAFE_CONTAINER_SAFE_CIRCULAR_QUEUE_H

#include <vector>
#include <mutex>
#include <memory>

namespace armor_auto_aim {
/**
 *
 * @tparam T
 */
template <typename T>
class SafeCircularQueue {
    template <typename Node_T>
    class Node {
        Node_T a;
    };
public:
    SafeCircularQueue() {

    }

    void push(T new_data) {
        std::shared_ptr<T> data(std::make_shared<T>(std::move(new_data)));
        {
            std::lock_guard<std::mutex> lk(m_mutex);
            m_vec.push_back(data);
        }
    }
private:
    mutable std::mutex m_mutex;
    std::vector<std::shared_ptr<T>> m_vec;
};
}

#endif //ARMOR_AUTO_AIM_SAFE_CONTAINER_SAFE_CIRCULAR_QUEUE_H
