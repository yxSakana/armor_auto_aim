/**
 * @project_name auto_aim
 * @file test_safe_container.cpp
 * @brief
 * @author yx
 * @date 2023-12-25 22:11:24
 */

#include <chrono>
#include <thread>

#include <gtest/gtest.h>
#include <glog/logging.h>

#include <safe_container/safe_circular_buffer.h>
#include <ostream>

namespace {

struct TimestampInt {
    int v;
    uint64_t timestamp;
    explicit TimestampInt(const int& v)
            : v(v),
              timestamp(std::chrono::system_clock::now().time_since_epoch().count()) {}

    friend std::ostream& operator<<(std::ostream& os, const TimestampInt& an_int) {
        os << "v: " << an_int.v << " timestamp: " << an_int.timestamp;
        return os;
    }
};

int64_t diffFunction(TimestampInt a, TimestampInt b) {
    return std::abs(static_cast<int64_t>(a.timestamp) - static_cast<int64_t>(b.timestamp));
}

int diffFunctionInt(const int& a, const int& b) {
    return std::abs(a - b);
}

TEST(test_safe_container, test_safe_circular_buffer) {
    armor_auto_aim::SafeCircularBuffer<TimestampInt, 6> buffer;
    buffer.push(TimestampInt(1));
    buffer.push(TimestampInt(2));
    buffer.push(TimestampInt(5));
    buffer.push(TimestampInt(5));
    buffer.push(TimestampInt(9));
    buffer.display();
    buffer.displayR();
    std::cout << *buffer.tryTop() << std::endl;
    buffer.display();
    buffer.push(TimestampInt(0));
    buffer.push(TimestampInt(1));
    buffer.push(TimestampInt(3));
    buffer.push(TimestampInt(68));
    buffer.push(TimestampInt(5));
    buffer.push(TimestampInt(324));
    buffer.push(TimestampInt(8));
    buffer.push(TimestampInt(8));
    buffer.push(TimestampInt(5));
    TimestampInt compare_val(123);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    buffer.push(TimestampInt(81));
    buffer.push(TimestampInt(6582));
    buffer.push(TimestampInt(586));
    buffer.push(TimestampInt(123));
    buffer.display();
//    buffer.displayR();
    TimestampInt v(0);

    bool status = buffer.tryPopRecent(v, compare_val, diffFunction);
    buffer.waitPopRecent(v, compare_val, diffFunction);
    if (status) std::cout << "v: " << v << std::endl;
}

TEST(test_safe_container, test_safe_circular_buffer_int) {
    armor_auto_aim::SafeCircularBuffer<int, 4> buffer;
    buffer.push(1);
    buffer.push(2);
    buffer.push(5);
    buffer.push(5);
    buffer.push(9);
    buffer.display();
    buffer.displayR();
    std::cout << *buffer.tryTop() << std::endl;
    buffer.display();
    buffer.push(0);
    buffer.push(1);
    buffer.push(3);
    buffer.push(68);
    buffer.push(5);
    buffer.push(324);
    buffer.push(8);
    buffer.push(8);
    buffer.push(5);
    buffer.push(81);
    buffer.push(6582);
    buffer.push(586);
    buffer.push(123);
    buffer.display();
    buffer.displayR();
    int v;
    int b = 1;

    bool status = buffer.tryPopRecent(v, b, diffFunctionInt);
    std::cout << "v: " << v << std::endl;
}
}
