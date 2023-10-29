/**
 * @file BackTrace.cpp
 * @date 2021年05月11日
 * @anchor yao
 */

#include <spdlog_factory/BackTrace.h>
#include <execinfo.h>
#include <csignal>
#include <unistd.h>

std::shared_ptr<spdlog::logger> BackTrace::log = spdlog::stderr_color_mt("BackTrace");

BackTrace::BackTrace() {
    std::signal(SIGHUP, &BackTrace::signal_exit);
    std::signal(SIGINT, &BackTrace::signal_exit);
    std::signal(SIGQUIT, &BackTrace::signal_exit);
    std::signal(SIGABRT, &BackTrace::signal_exit);
    std::signal(SIGKILL, &BackTrace::signal_exit);
    std::signal(SIGSEGV, &BackTrace::signal_exit);
    std::signal(SIGTERM, &BackTrace::signal_exit);
}

void BackTrace::signal_exit(int sig) {
    const char *sig_str = signalString(sig);
    log->error("{} PID={} ThreadID={} ClassName={}", sig_str, getpid(), QThread::currentThreadId(),
               QThread::currentThread()->metaObject()->className());
    if (sig == SIGABRT || sig == SIGSEGV)
        print_stack(sig_str);
    spdlog::apply_all([=](const std::shared_ptr<spdlog::logger> &logger) {
        logger->flush();
    });
    spdlog::shutdown();
    ::exit(sig | 0x80);
}

void BackTrace::print_stack(const char *sig) {
    void *stack[128] = {NULL};
    size_t size = backtrace(stack, 128);
    char **strings = backtrace_symbols(stack, size);
    if (strings == NULL) {
        log->error("backtrace_symbols");
    } else {
        char buff[1024] = {0};
        for (size_t i = 0; i < size; i++) {
            if (get_AddrLine(strings[i], buff))
                log->error("{} at {}", strings[i], buff);
            else
                log->error("{}", strings[i]);
        }
        free(strings);
    }
}

bool BackTrace::get_AddrLine(const char *addr, char *buff) {
    bool status = false;
    char cmd[256] = {0};
    char addrline[32] = {0};
    const char *str1 = strchr(addr, '(');
    const char *str2 = strchr(addr, ')');
    if (str1 == NULL || str2 == NULL)
        return false;
    memcpy(addrline, str1 + 1, str2 - str1 - 1);
    snprintf(cmd, sizeof(cmd), "addr2line -e /proc/%d/exe %s -f --demangle", getpid(), addrline);
    FILE *pip = popen(cmd, "r");
    if (pip == NULL)
        return false;

    memset(buff, 0, 1024);
    size_t len = 0;
    while (NULL != fgets(buff + len, 1024 - len, pip) && len < 1024 - 1)
        len = strlen(buff);

    pclose(pip);
    if (len > 2) {
        if (buff[0] == '?' && buff[1] == '?')
            return false;
        for (size_t i = 0; i < len; i++)
            if (buff[i] == '\n')
                buff[i] = ' ';
    }

    return len != 0;
}

const char *BackTrace::signalString(int sig) noexcept {
    switch (sig) {
        case SIGHUP:
            return "SIGHUP(1)";
        case SIGINT:
            return "SIGINT(2:CTRL_C)";
        case SIGQUIT:
            return "SIGQUIT(3)";
        case SIGABRT:
            return "SIGABRT(6)";
        case SIGKILL:
            return "SIGKILL(9)";
        case SIGTERM:
            return "SIGTERM(15 KILL)";
        case SIGSEGV:
            return "SIGSEGV(11)";
        default:
            return NULL;
    }
}
