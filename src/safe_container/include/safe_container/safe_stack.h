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
    SafeStack();
private:
    mutable std::mutex m_mutex;
    std::condition_variable m_condition;
    std::vector<T> m_vec;
};
}

#endif //ARMOR_AUTO_AIM_SAFE_CONTAINER_SAFE_STACK_H
