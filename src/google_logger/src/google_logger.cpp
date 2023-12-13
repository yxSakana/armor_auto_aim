/**
 * @projectName test_glog
 * @file google_logger.cpp
 * @brief 
 * 
 * @author yx 
 * @date 2023-10-09 20:30
 */

#include <iostream>
#include <iomanip>
#include <regex>
#include <chrono>
#include <thread>

#include <google_logger/google_logger.h>

namespace armor_auto_aim::google_log {
void initGoogleLogger(int argc, char* argv[]) {
    if (!k_IsInitialized) {
        k_GoogleLogger = std::make_unique<GoogleLogger>(argc, argv);
        k_IsInitialized = true;
    }
}

void initGoogleLogger(const char* program) {
    if (!k_IsInitialized) {
        k_GoogleLogger = std::make_unique<GoogleLogger>(program);
        k_IsInitialized = true;
    }
}

void ConsoleLogSink::send(google::LogSeverity severity, const char *full_filename, const char *base_filename, int line,
                          const struct tm *tm_time, const char *message, size_t message_len) {
    int y = 1900 + tm_time->tm_year;
    int m = tm_time->tm_mon;
    int d = tm_time->tm_mday;
    int h = tm_time->tm_hour;
    int min = tm_time->tm_min;
    int s = tm_time->tm_sec;

    const std::string reset_color("\033[0m");
    const std::string thread_id_color("\033[4m");
    const std::string time_color("\033[90m");
    const std::array<std::string, 4> color {"\033[92m", "\033[93m", "\033[91m", "\033[101m"};
    const std::array<std::string, 4> level_info {"INFO", "WARNING", "ERROR", "FATAL"};
    fmt::print("({}:{})"
               "{}[{}]{}"
               "{}[{:04d}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}]{}"
               "{}{}{}",
               base_filename, line,
               color[toascii(severity)], level_info[toascii(severity)], reset_color,
               time_color, y, m, d, h, min, s, reset_color,
               color[toascii(severity)], message, reset_color
    );
//    std::cout << "(" << base_filename << ":" << line << ")"
//              << color[toascii(severity)] << "[" << level_info[toascii(severity)] << "]" << reset_color
//              << thread_id_color << "(thread: " << std::this_thread::get_id() << ")" << reset_color
//              << time_color << "[" << y << "-"
//              << std::setw(2) << std::setfill('0') << m << "-"
//              << std::setw(2) << std::setfill('0') << d << " "
//              << std::setw(2) << std::setfill('0') << h << ":"
//              << std::setw(2) << std::setfill('0') << min << ":"
//              << std::setw(2) << std::setfill('0') << s << "] " << reset_color
//              << color[toascii(severity)] << message << reset_color;
}

GoogleLogger::GoogleLogger(const char *program)
        : m_console_log_sink(new ConsoleLogSink)
{
    google::InitGoogleLogging(program, CustomPrefix);
    initGoogleConfig();
}

GoogleLogger::GoogleLogger(int argc, char** argv)
    : m_console_log_sink(new ConsoleLogSink)
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0], CustomPrefix);
    initGoogleConfig();
}

GoogleLogger::~GoogleLogger() {
//    filesRecursively(m_info_log_dir);
//    filesRecursively(m_warning_log_dir);
//    filesRecursively(m_error_log_dir);
//    LOG(WARNING) << "删除部分日志!";  // TODO: 日志删除机制更改
    google::ShutdownGoogleLogging();
}

GoogleLogger::Timestamp::Timestamp(const std::string &timestamp) {
    year = std::stoi(timestamp.substr(0, 4));
    mon = std::stoi(timestamp.substr(4, 2));
    day = std::stoi(timestamp.substr(6, 2));
    hours = std::stoi(timestamp.substr(9, 2));
    min = std::stoi(timestamp.substr(11, 2));
    sec = std::stoi(timestamp.substr(13, 2));
}

bool GoogleLogger::Timestamp::operator<(const armor_auto_aim::google_log::GoogleLogger::Timestamp &t) const {
    if (year < t.year) return true; else if (year > t.year) return false;
    if (mon < t.mon) return true; else if (mon > t.mon) return false;
    if (day < t.day) return true;  else if (day > t.day) return false;
    if (hours < t.hours) return true; else if (hours > t.hours) return false;
    if (min < t.min) return true; else if (min > t.min) return false;
    if (sec < t.sec) return true; else if (sec > t.sec) return false;
    return false;
}

void GoogleLogger::CustomPrefix(std::ostream &os, const google::LogMessageInfo &log_info, void *) {
    int y = 1900 + log_info.time.year();
    int m = log_info.time.month();
    int d = log_info.time.day();
    int h = log_info.time.hour();
    int min = log_info.time.min();
    int s = log_info.time.sec();

    os << fmt::format("({}:{})[{}][{:04d}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}]",
                      log_info.filename, log_info.line_number, log_info.severity,
                      y, m, d, h, min, s);
}

void GoogleLogger::initGoogleConfig() {
    google::InstallFailureSignalHandler();
    FLAGS_stderrthreshold = 5;  // 设置默认控制台记录器级别阈值, 消除默认控制台记录器
    google::EnableLogCleaner(3);
    std::string pwd = std::filesystem::current_path().string();
    m_info_log_dir = pwd + "/../log/info/";
    m_warning_log_dir = pwd + "/../log/warning/";
    m_error_log_dir = pwd + "/../log/error/";
    checkDirectory(m_info_log_dir);
    checkDirectory(m_warning_log_dir);
    checkDirectory(m_error_log_dir);
    google::SetLogDestination(google::GLOG_INFO, m_info_log_dir.c_str());
    google::SetLogDestination(google::GLOG_WARNING, m_warning_log_dir.c_str());
    google::SetLogDestination(google::GLOG_ERROR, m_error_log_dir.c_str());
    // 控制台日志
    google::AddLogSink(m_console_log_sink);
}

void GoogleLogger::filesRecursively(const std::filesystem::path &path) {
    struct FileTimestamp {
        std::filesystem::directory_entry directory_entry;
        Timestamp timestamp;

        FileTimestamp(std::filesystem::directory_entry entry, Timestamp timestamp)
                : directory_entry(std::move(entry)),
                  timestamp(timestamp)
        {}
    };

    std::regex pattern("\\d{8}-\\d{6}");
    std::smatch result;
    std::vector<FileTimestamp> all_file_timestamp;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
        if (std::string filename = entry.path().string();
                entry.is_regular_file() && std::regex_search(filename, result, pattern))
            all_file_timestamp.emplace_back(entry, Timestamp(result.str()));
    }
    std::sort(all_file_timestamp.begin(), all_file_timestamp.end(),
              [](const FileTimestamp& a, const FileTimestamp& b)->bool { return a.timestamp > b.timestamp; });
    while (all_file_timestamp.size() > MAX_LOG_LEN) {
        remove(all_file_timestamp.back().directory_entry.path());
        all_file_timestamp.pop_back();
    }
}

void GoogleLogger::checkDirectory(const std::string& path) {
    if (!std::filesystem::exists(path))
        std::filesystem::create_directories(path);
}
} // armor_auto_aim
