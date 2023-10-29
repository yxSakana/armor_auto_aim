/**
 * @file BackTrace.h
 * @date 2021年05月11日
 * @anchor yao
 */

#ifndef BACKTRACE_BACKTRACE_H
#define BACKTRACE_BACKTRACE_H

#include <spdlog_factory/spdlogger.h>

/**
 * @class BackTrace
 * @brief 堆栈回溯
 * @details 在全局定义一个对象即可
 * @warning 只在Linux中提供
 * @warning 如果使用了{@link MainThread}请勿自行定义对象
 * @bug 目前无法准确捕捉由SIGABRT信号发出的QThread内的异常堆栈，SIGSEGV信号可以捕捉
 * @test 尚不保证没有其他问题
 */
class BackTrace {

    static std::shared_ptr<spdlog::logger> log;

public:

    /**
     * @brief BackTrace构造函数，重定向信号 SIGHUP SIGINT SIGQUIT SIGABRT SIGKILL SIGSEGV SIGTERM
     */
    BackTrace();

private:
    static void signal_exit(int sig);

    static void print_stack(const char *sig);

    static bool get_AddrLine(const char *addr, char *buff);

    static const char *signalString(int sig) noexcept;
};


#endif //BACKTRACE_BACKTRACE_H
