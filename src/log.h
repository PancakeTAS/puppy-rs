/// \file log.h This file contains utilities for logging messages to the console.

#pragma once

typedef enum {
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL
} log_level;

/**
 * Log a trace message.
 *
 * \param module
 *   The module name.
 * \param fmt
 *   The message format.
 * \param ...
 *   The message arguments.
 */
#define log_trace(module, fmt, ...) _log_trace(module, fmt, __FILE__, __LINE__, ##__VA_ARGS__)

/**
 * Log a debug message.
 *
 * \param module
 *   The module name.
 * \param fmt
 *   The message format.
 * \param ...
 *   The message arguments.
 */

#define log_debug(module, fmt, ...) _log_debug(module, fmt, __FILE__, __LINE__, ##__VA_ARGS__)

/**
 * Log an info message.
 *
 * \param module
 *   The module name.
 * \param fmt
 *   The message format.
 * \param ...
 *   The message arguments.
 */
#define log_info(module, fmt, ...) _log_info(module, fmt, __FILE__, __LINE__, ##__VA_ARGS__)

/**
 * Log a warning message.
 *
 * \param module
 *   The module name.
 * \param fmt
 *   The message format.
 * \param ...
 *   The message arguments.
 */
#define log_warn(module, fmt, ...) _log_warn(module, fmt, __FILE__, __LINE__, ##__VA_ARGS__)

/**
 * Log an error message.
 *
 * \param module
 *   The module name.
 * \param fmt
 *   The message format.
 * \param ...
 *   The message arguments.
 */
#define log_error(module, fmt, ...) _log_error(module, fmt, __FILE__, __LINE__, ##__VA_ARGS__)

/**
 * Log a fatal message.
 *
 * \param module
 *   The module name.
 * \param fmt
 *   The message format.
 * \param ...
 *   The message arguments.
 */
#define log_fatal(module, fmt, ...) _log_fatal(module, fmt, __FILE__, __LINE__, ##__VA_ARGS__)

/**
 * Set the maximum log level.
 *
 * \param level
 *   The maximum log level.
 */
void log_set_level(log_level level);

void _log_trace(const char* module, const char* fmt, const char* file, int line, ...);
void _log_debug(const char* module, const char* fmt, const char* file, int line, ...);
void _log_info(const char* module, const char* fmt, const char* file, int line, ...);
void _log_warn(const char* module, const char* fmt, const char* file, int line, ...);
void _log_error(const char* module, const char* fmt, const char* file, int line, ...);
void _log_fatal(const char* module, const char* fmt, const char* file, int line, ...);
