#include "log.h"

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#define LOG_COLOR_PLAIN "\x1b[0m"
#define LOG_COLOR_MODULE "\x1b[90m"
#define LOG_COLOR_TRACE "\x1b[94m"
#define LOG_COLOR_DEBUG "\x1b[36m"
#define LOG_COLOR_INFO "\x1b[32m"
#define LOG_COLOR_WARN "\x1b[33m"
#define LOG_COLOR_ERROR "\x1b[31m"
#define LOG_COLOR_FATAL "\x1b[35m"

#define LOG(level, module, fmt, file, line, args) \
    time_t t = time(NULL); \
    struct tm* timeinfo = localtime(&t); \
    fprintf(stderr, LOG_COLOR_PLAIN "%02d:%02d:%02d " level " " LOG_COLOR_MODULE "%s:%d " LOG_COLOR_PLAIN "[%s] ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, file, line, module); \
    vfprintf(stderr, fmt, args); \
    fprintf(stderr, "\n");

log_level max_level = LOG_TRACE;

void log_set_level(log_level level) {
    max_level = level;
}

void _log_trace(const char* module, const char* fmt, const char* file, int line, ...) {
    if (max_level > LOG_TRACE)
        return;

    va_list args;
    va_start(args, line);
    LOG(LOG_COLOR_TRACE "TRACE", module, fmt, file, line, args);
    va_end(args);
}

void _log_debug(const char* module, const char* fmt, const char* file, int line, ...) {
    if (max_level > LOG_DEBUG)
        return;

    va_list args;
    va_start(args, line);
    LOG(LOG_COLOR_DEBUG "DEBUG", module, fmt, file, line, args)
    va_end(args);
}

void _log_info(const char* module, const char* fmt, const char* file, int line, ...) {
    if (max_level > LOG_INFO)
        return;

    va_list args;
    va_start(args, line);
    LOG(LOG_COLOR_INFO "INFO ", module, fmt, file, line, args)
    va_end(args);
}

void _log_warn(const char* module, const char* fmt, const char* file, int line, ...) {
    if (max_level > LOG_WARN)
        return;

    va_list args;
    va_start(args, line);
    LOG(LOG_COLOR_WARN "WARN ", module, fmt, file, line, args)
    va_end(args);
}

void _log_error(const char* module, const char* fmt, const char* file, int line, ...) {
    if (max_level > LOG_ERROR)
        return;

    va_list args;
    va_start(args, line);
    LOG(LOG_COLOR_ERROR "ERROR", module, fmt, file, line, args)
    va_end(args);
}

void _log_fatal(const char* module, const char* fmt, const char* file, int line, ...) {
    va_list args;
    va_start(args, line);
    LOG(LOG_COLOR_FATAL "FATAL", module, fmt, file, line, args)
    va_end(args);
}
