/**
 * @project_name auto_aim
 * @file safe_circular_buffer.h
 * @brief 固定大小
 *        缓冲区满时, push() 移除旧数据, 添加新数据 \n
 *        top() 获取最新数据并清空缓冲区 \n
 *        popRecent(OtherType other, DifferenceFunction) \n
 *          \t 获取缓冲区中与 other, DifferenceFunction(buffer_data, other)最小的数据 \n
 *        m_bottom->next 指向缓冲区最末尾的 \n
 *        缓冲区最末尾->next = nullptr \n
 *        nullptr <-> |last|... <-> |t-2| <-> |t-1| <-> |Top|> nullptr \n
 *                    ↑ \n
 *        nullptr <-Bottom \n
 * @author yx
 * @date 2023-12-25 19:52:28
 */

#ifndef ARMOR_AUTO_AIM_SAFE_CONTAINER_SAFE_CIRCULAR_BUFFER_H
#define ARMOR_AUTO_AIM_SAFE_CONTAINER_SAFE_CIRCULAR_BUFFER_H

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <memory>

namespace armor_auto_aim {
template <typename T, std::size_t Capacity>
class SafeCircularBuffer {
    using SizeType = unsigned int;
    template <typename Item>
    struct Node {
        Item item;
        std::shared_ptr<Node> next;
        std::shared_ptr<Node> prev;
        explicit Node(Item value): item(value), next(nullptr), prev(nullptr) {}
    };
public:
    SafeCircularBuffer() =default;

    void push(T value) {
        auto new_node = std::make_shared<Node<T>>(value);
        {
            std::lock_guard<std::mutex> lk(m_mutex);
            if (m_n >= m_max) {
                m_bottom = m_bottom->prev;
                if (m_bottom == nullptr) {
                    m_bottom = m_top = nullptr;
                } else {
                    m_bottom->next = nullptr;
                    --m_n;
                }
            }

            if (m_top == nullptr && m_bottom == nullptr) {
                m_top = m_bottom = new_node;
            } else {
                new_node->next = m_top;
                m_top->prev = new_node;
                m_top = new_node;
            }
            ++m_n;
        }
        m_condition.notify_one();
    }

    bool tryTop(T& value) {
        std::lock_guard<std::mutex> lk(m_mutex);
        if (m_n == 0) return false;

        value = std::move(m_top->item);
        m_top = m_bottom = nullptr;
        m_n = 0;
        return true;
    };

    std::shared_ptr<T> tryTop() {
        std::lock_guard<std::mutex> lk(m_mutex);
        if (m_n == 0) return nullptr;

        auto value = std::make_shared<T>(std::move(m_top->item));
        m_top = m_bottom = nullptr;
        m_n = 0;
        return value;
    }

    void waitTop(T& value) {
        std::unique_lock<std::mutex> lk(m_mutex);
        m_condition.wait(lk, [this]()->bool { return m_n != 0; });
        value = std::move(m_top->item);
        m_top = m_bottom = nullptr;
        m_n = 0;
    }

    template <typename OtherType, typename DifferenceFunction>
    bool tryPopRecent(T& result, const OtherType& compare_val, DifferenceFunction diff_function) {
        std::lock_guard<std::mutex> lk(m_mutex);
        if (m_n == 0) return false;

        T* current_val = &m_top->item;
        auto min_diff = diff_function(*current_val, compare_val);
        for (std::shared_ptr<Node<T>> node = m_top->next; node != nullptr; node = node->next) {
            auto diff = diff_function(node->item, compare_val);
            if (diff > min_diff) {
                result = std::move(node->prev->item);
                m_top = m_bottom = nullptr;
                m_n = 0;
                return true;
            } else {
                min_diff = diff;
            }
        }

        result = std::move(m_bottom->item);
        m_top = m_bottom = nullptr;
        m_n = 0;
        return true;
    }

    template <typename OtherType, typename DifferenceFunction>
    int64_t waitPopRecent(T& result, const OtherType& compare_val, DifferenceFunction diff_function) {
        auto start = std::chrono::system_clock::now();
        std::unique_lock<std::mutex> lk(m_mutex);
        m_condition.wait(lk, [this]()->bool { return m_n != 0; });
        auto end = std::chrono::system_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>((end - start)).count();

        T* current_val = &m_top->item;
        auto min_diff = diff_function(*current_val, compare_val);
        for (std::shared_ptr<Node<T>> node = m_top->next; node != nullptr; node = node->next) {
            auto diff = diff_function(node->item, compare_val);
            if (diff > min_diff) {
                result = std::move(node->prev->item);
                m_top = m_bottom = nullptr;
                m_n = 0;
                return time;
            } else {
                min_diff = diff;
            }
        }
        result = std::move(m_bottom->item);
        m_top = m_bottom = nullptr;
        m_n = 0;
        return time;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lk(m_mutex);
        return m_n == 0 && m_bottom == nullptr && m_top == nullptr;
    }

    bool full() const {
        std::lock_guard<std::mutex> lk(m_mutex);
        return m_n == m_max && m_bottom != nullptr && m_top != nullptr;
    }

    void clear() {
        std::lock_guard<std::mutex> lk(m_mutex);
        m_top = m_bottom = nullptr;
        m_n = 0;
    }

    void display() const {
        std::lock_guard<std::mutex> lk(m_mutex);
        std::shared_ptr<Node<T>> current = m_top;
        while (current) {
            std::cout << current->item << ", ";
            current = current->next;
        }
        std::cout << std::endl;
    }

    void displayR() const {
        std::lock_guard<std::mutex> lk(m_mutex);
        std::shared_ptr<Node<T>> current = m_bottom;
        while (current) {
            std::cout << current->item << ", ";
            current = current->prev;
        }
        std::cout << std::endl;
    }

private:
    std::shared_ptr<Node<T>> m_top = nullptr;
    std::shared_ptr<Node<T>> m_bottom = nullptr;
    mutable std::mutex m_mutex;
    std::condition_variable m_condition;
    SizeType m_max = Capacity;
    SizeType m_n = 0;
};
}

#endif //ARMOR_AUTO_AIM_SAFE_CONTAINER_SAFE_CIRCULAR_BUFFER_H
