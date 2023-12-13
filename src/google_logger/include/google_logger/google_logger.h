/**
 * @projectName test_glog
 * @file google_logger.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-10-08 19:28
 */

#ifndef TEST_GLOG_GOOGLELOGGER_H
#define TEST_GLOG_GOOGLELOGGER_H

#include <filesystem>

#include <fmt/format.h>
#include <fmt/color.h>
#include <glog/logging.h>

namespace armor_auto_aim::google_log {
static bool k_IsInitialized = false;

void initGoogleLogger(int argc, char* argv[]);

void initGoogleLogger(const char* program);

/**
 * @brief 控制台日志
 */
class ConsoleLogSink : public google::LogSink {
public:
    /**
     * @brief send函数重载
     *
     * @param severity 日志级别
     * @param full_filename 绝对路径
     * @param base_filename 文件名
     * @param line 行
     * @param tm_time 时间
     * @param message 消息
     * @param message_len 日志消息文本的长度
     */
    void send(google::LogSeverity severity,
              const char* full_filename, const char* base_filename,
              int line, const struct tm* tm_time,
              const char* message,size_t message_len) override;
};

/**
 * @brief 封装 glog
 */
class GoogleLogger {
public:
    explicit GoogleLogger(const char* program);

    GoogleLogger(int argc, char** argv);

    ~GoogleLogger();

    [[nodiscard]] const std::string& info_log_dir() const { return m_info_log_dir; };

    [[nodiscard]] const std::string& warning_log_dir() const { return m_warning_log_dir; };

    [[nodiscard]] const std::string& error_log_dir() const { return m_error_log_dir; };
private:
    static const int MAX_LOG_LEN = 10;  // 日志最大数

    class Timestamp {
    public:
        /**
         * @brief
         * @param timestamp yyyymmdd-hhmmss
         */
        explicit Timestamp(const std::string& timestamp);

        bool operator<(const Timestamp& t) const;

        bool operator>(const Timestamp& t) const { return !(*this < t); }

        [[nodiscard]] std::string to_string() const {
            return fmt::format("[(Timestamp): {:04}-{:02}-{:02} {:02}:{:02}:{:02}]", year, mon, day, hours, min, sec);
        }
    private:
        int year;
        int mon;
        int day;
        int hours;
        int min;
        int sec;
    };

    std::string m_info_log_dir;
    std::string m_warning_log_dir;
    std::string m_error_log_dir;
    ConsoleLogSink* m_console_log_sink;

    void initGoogleConfig();

    /**
     * @brief 日志前缀
     *
     * @param os
     * @param log_info
     */
    static void CustomPrefix(std::ostream &os, const google::LogMessageInfo &log_info, void*);

    /**
     * @brief 迭代目录结构
     *
     * @param path
     */
    static void filesRecursively(const std::filesystem::path& path);

    static void checkDirectory(const std::string& path);
};

static std::unique_ptr<GoogleLogger> k_GoogleLogger;
} // yx

//template <>
//struct fmt::formatter<armor_auto_aim::google_log::GoogleLogger> {
//    template <typename ParseContext>
//    constexpr auto parse(ParseContext& ctx) {
//        return ctx.begin();
//    }
//
//    template <typename FormatContext>
//    auto format(const armor_auto_aim::google_log::GoogleLogger& google_logger, FormatContext& ctx) {
//        return fmt::format_to(ctx.out(),
//                              "[(yx::GoogleLogger) -> (dir)info: {}; waning: {}; error: {}]",
//                              google_logger.info_log_dir(),
//                              google_logger.warning_log_dir(),
//                              google_logger.error_log_dir());
//    }
//};
#endif //TEST_GLOG_GOOGLELOGGER_H
