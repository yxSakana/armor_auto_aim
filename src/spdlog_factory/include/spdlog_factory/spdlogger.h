/**
 * @file spdlogger.h
 * @author yao
 * @date 2021年1月13日
 * @brief 基于spdlog的二次封装，内置错误流和标准输出流
 */

#ifndef KDROBOTCPPLIBS_SPDLOGGER_H
#define KDROBOTCPPLIBS_SPDLOGGER_H

#include <utility>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <spdlog_factory/QtCompatibilityLayer.h>

class spdlogger {
    static bool isLogToFile;
    static std::shared_ptr<spdlog::sinks::basic_file_sink_mt> FileSink;
    std::shared_ptr<spdlog::logger> log, err_log;
public:
    /**
     * 从两个记录器重建·创建logger
     * @param _logger 标准输入输出流记录器
     * @param _err_logger 错误流记录器
     */
    spdlogger(std::shared_ptr<spdlog::logger> _logger, std::shared_ptr<spdlog::logger> _err_logger);

    /**
     * 拷贝构造函数
     * @param copy 拷贝对象
     */
    spdlogger(const spdlogger &copy) :
            log(copy.log), err_log(copy.err_log) {}

    /**
     * 转移构造函数
     * @param mov 转移对象
     */
    spdlogger(const spdlogger &&mov) :
            log(std::move(mov.log)), err_log(std::move(mov.err_log)) {}

    /**
     * 调用工厂方法构建对象
     * @param name logger名字
     */
    spdlogger(const std::string &name);

    /**
     * 重载运算符= 赋值拷贝
     * @param logger 拷贝对象
     * @return 自身拷贝
     */
    spdlogger operator=(const spdlogger &logger) {
        this->log = logger.log;
        this->err_log = logger.err_log;
        return *this;
    }

    /**
     * 启用回溯功能
     * @param n_messages 回溯条数
     */
    void enable_backtrace(size_t n_messages);

    /**
     * 显示回溯日志
     */
    void dump_backtrace();

    /**
     * 关闭回溯功能
     */
    void disable_backtrace();

    /**
     * 跟踪日志
     * @tparam T 
     * @param msg 日志
     */
    template<typename T>
    void trace(const T &msg) {
        log->trace(msg);
    }

    /**
     * 调试日志
     * @tparam T 
     * @param msg 日志
     */
    template<typename T>
    void debug(const T &msg) {
        log->debug(msg);
    }

    /**
     * 信息日志
     * @tparam T 
     * @param msg 日志
     */
    template<typename T>
    void info(const T &msg) {
        log->info(msg);
    }

    /**
     * 警告日志
     * @tparam T 
     * @param msg 日志
     */
    template<typename T>
    void warn(const T &msg) {
        log->warn(msg);
    }

    /**
     * 错误日志
     * @tparam T 
     * @param msg 日志
     */
    template<typename T>
    void error(const T &msg) {
        err_log->error(msg);
    }

    /**
     * 重要日志
     * @tparam T 
     * @param msg 日志
     */
    template<typename T>
    void critical(const T &msg) {
        log->critical(msg);
    }

    /**
     * 跟踪日志
     * @tparam FormatString 格式化字符串泛型
     * @tparam Args 参数列表
     * @param fmt 格式化字符串
     * @param args 参数列表
     */
    template<typename FormatString, typename... Args>
    void trace(const FormatString &fmt, const Args &... args) {
        log->trace(fmt, args...);
    }

    /**
     * 调试日志
     * @tparam FormatString 格式化字符串泛型
     * @tparam Args 参数列表
     * @param fmt 格式化字符串
     * @param args 参数列表
     */
    template<typename FormatString, typename... Args>
    void debug(const FormatString &fmt, const Args &... args) {
        log->debug(fmt, args...);
    }

    /**
     * 信息日志
     * @tparam FormatString 格式化字符串泛型
     * @tparam Args 参数列表
     * @param fmt 格式化字符串
     * @param args 参数列表
     */
    template<typename FormatString, typename... Args>
    void info(const FormatString &fmt, const Args &... args) {
        log->info(fmt, args...);
    }

    /**
     * 警告日志
     * @tparam FormatString 格式化字符串泛型
     * @tparam Args 参数列表
     * @param fmt 格式化字符串
     * @param args 参数列表
     */
    template<typename FormatString, typename... Args>
    void warn(const FormatString &fmt, const Args &... args) {
        log->warn(fmt, args...);
    }

    /**
     * 错误日志
     * @tparam FormatString 格式化字符串泛型
     * @tparam Args 参数列表
     * @param fmt 格式化字符串
     * @param args 参数列表
     */
    template<typename FormatString, typename... Args>
    void error(const FormatString &fmt, const Args &... args) {
        err_log->error(fmt, args...);
    }

    /**
     * 重要日志
     * @tparam FormatString 格式化字符串泛型
     * @tparam Args 参数列表
     * @param fmt 格式化字符串
     * @param args 参数列表
     */
    template<typename FormatString, typename... Args>
    void critical(const FormatString &fmt, const Args &... args) {
        log->critical(fmt, args...);
    }

    /**
     * 创建或者获取logger的工厂方法
     * @param name 记录器名字
     * @return 记录器
     */
    static spdlogger getLogger(const std::string &name);

    /**
     * 让所有记录器记录到文件
     * @param filename 日志文件名
     * @param truncate 是否截断
     */
    static void allLogger_logToFile(const std::string &filename, bool truncate = false);

    /**
     * 给当前logger添加记录槽
     * @param sink 记录槽
     */
    void AddSink(const spdlog::sink_ptr &sink);

    void LogToFile() {
        if (FileSink != nullptr)
            AddSink(FileSink);
    }

    inline void flush() {
        log->flush();
        err_log->flush();
    }
};


#endif
