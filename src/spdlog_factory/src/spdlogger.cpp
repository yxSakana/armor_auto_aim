/**
 * @file spdlogger.cpp
 * @author yao
 * @date 2021年1月13日
 */

#include <spdlog_factory/spdlogger.h>

bool spdlogger::isLogToFile = false;
std::shared_ptr<spdlog::sinks::basic_file_sink_mt> spdlogger::FileSink = nullptr;

spdlogger spdlogger::getLogger(const std::string &name) {
    if (spdlog::get(name) == nullptr) {
        auto &&logger = spdlog::stdout_color_mt(name);
        auto &&err_logger = spdlog::stderr_color_mt(std::string(name) + "_error");
        return {logger, err_logger};
    } else {
        auto &&logger = spdlog::get(name);
        auto &&err_logger = spdlog::get(std::string(name) + "_error");
        return {logger, err_logger};
    }
}

spdlogger::spdlogger(const std::string &name) {
    spdlogger &&t = spdlogger::getLogger(name);
    log = t.log;
    err_log = t.err_log;
}

void spdlogger::enable_backtrace(size_t n_messages) {
    log->enable_backtrace(n_messages);
    err_log->enable_backtrace(n_messages);
}

void spdlogger::dump_backtrace() {
    log->dump_backtrace();
    err_log->dump_backtrace();
}

void spdlogger::disable_backtrace() {
    log->disable_backtrace();
    err_log->disable_backtrace();
}

void spdlogger::allLogger_logToFile(const std::string &filename, bool truncate) {
    if (!isLogToFile) {
        isLogToFile = true;
        FileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename, truncate);
        spdlog::apply_all([=](const std::shared_ptr<spdlog::logger> &logger) {
            auto &sinks = logger->sinks();
            for (const auto &item : sinks) {
                if (item == FileSink)
                    return;
            }
            logger->sinks().push_back(FileSink);
        });
        spdlog::flush_every(std::chrono::seconds(1));
    }
}

void spdlogger::AddSink(const spdlog::sink_ptr &sink) {
    for (const auto &i : log->sinks())
        if (i != sink) continue;
        else return;
    for (const auto &i : err_log->sinks())
        if (i != sink) continue;
        else return;
    log->sinks().push_back(sink);
    err_log->sinks().push_back(sink);
}

spdlogger::spdlogger(std::shared_ptr<spdlog::logger> _logger, std::shared_ptr<spdlog::logger> _err_logger) :
        log(_logger), err_log(_err_logger) {
    if (isLogToFile) AddSink(FileSink);
}
