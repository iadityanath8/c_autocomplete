#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ======================
   Log Level Definitions
   ====================== */
typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL
} LogLevel;

/* ======================
   Color Definitions (ANSI)
   ====================== */
#ifdef LOGGER_NO_COLOR
    #define COLOR_RESET   ""
    #define COLOR_BLACK   ""
    #define COLOR_RED     ""
    #define COLOR_GREEN   ""
    #define COLOR_YELLOW  ""
    #define COLOR_BLUE    ""
    #define COLOR_MAGENTA ""
    #define COLOR_CYAN    ""
    #define COLOR_WHITE   ""
#else
    #define COLOR_RESET   "\033[0m"
    #define COLOR_BLACK   "\033[30m"
    #define COLOR_RED     "\033[31m"
    #define COLOR_GREEN   "\033[32m"
    #define COLOR_YELLOW  "\033[33m"
    #define COLOR_BLUE    "\033[34m"
    #define COLOR_MAGENTA "\033[35m"
    #define COLOR_CYAN    "\033[36m"
    #define COLOR_WHITE   "\033[37m"
#endif

/* ======================
   Log Level Colors
   ====================== */
#define DEBUG_COLOR  COLOR_CYAN
#define INFO_COLOR   COLOR_GREEN
#define WARN_COLOR   COLOR_YELLOW
#define ERROR_COLOR  COLOR_RED
#define FATAL_COLOR  COLOR_MAGENTA

/* ======================
   Configuration
   ====================== */
#ifndef LOG_LEVEL
    #define LOG_LEVEL LOG_DEBUG  // Default log level
#endif

#ifndef LOG_USE_TIMESTAMP
    #define LOG_USE_TIMESTAMP 1  // Include timestamp by default
#endif

#ifndef LOG_SHOW_LEVEL
    #define LOG_SHOW_LEVEL 1     // Show log level by default
#endif

#ifndef LOG_SHOW_FILELINE
    #define LOG_SHOW_FILELINE 1  // Show file and line by default
#endif

/* ======================
   Internal Helper Functions
   ====================== */
static const char* _log_level_to_string(LogLevel level) {
    switch(level) {
        case LOG_DEBUG: return "DEBUG";
        case LOG_INFO:  return "INFO";
        case LOG_WARN:  return "WARN";
        case LOG_ERROR: return "ERROR";
        case LOG_FATAL: return "FATAL";
        default:        return "UNKNOWN";
    }
}

static const char* _log_level_to_color(LogLevel level) {
    switch(level) {
        case LOG_DEBUG: return DEBUG_COLOR;
        case LOG_INFO:  return INFO_COLOR;
        case LOG_WARN:  return WARN_COLOR;
        case LOG_ERROR: return ERROR_COLOR;
        case LOG_FATAL: return FATAL_COLOR;
        default:        return COLOR_RESET;
    }
}

static void _get_timestamp(char* buffer, size_t size) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

/* ======================
   Main Logging Function
   ====================== */
#define _LOG(level, fmt, ...) do { \
    if (level >= LOG_LEVEL) { \
        char timestamp[32] = ""; \
        if (LOG_USE_TIMESTAMP) { \
            _get_timestamp(timestamp, sizeof(timestamp)); \
        } \
        \
        const char* level_str = _log_level_to_string(level); \
        const char* color = _log_level_to_color(level); \
        \
        if (LOG_USE_TIMESTAMP && LOG_SHOW_LEVEL && LOG_SHOW_FILELINE) { \
            fprintf(stderr, "%s[%s]%s %s%-7s%s %s:%d: " fmt "\n", \
                    COLOR_WHITE, timestamp, COLOR_RESET, \
                    color, level_str, COLOR_RESET, \
                    __FILE__, __LINE__, ##__VA_ARGS__); \
        } else if (LOG_USE_TIMESTAMP && LOG_SHOW_LEVEL) { \
            fprintf(stderr, "%s[%s]%s %s%-7s%s: " fmt "\n", \
                    COLOR_WHITE, timestamp, COLOR_RESET, \
                    color, level_str, COLOR_RESET, \
                    ##__VA_ARGS__); \
        } else if (LOG_SHOW_LEVEL) { \
            fprintf(stderr, "%s%-7s%s: " fmt "\n", \
                    color, level_str, COLOR_RESET, \
                    ##__VA_ARGS__); \
        } else { \
            fprintf(stderr, fmt "\n", ##__VA_ARGS__); \
        } \
    } \
} while(0)

/* ======================
   Simplified Logging Macros
   ====================== */
#define log_debug(fmt, ...) _LOG(LOG_DEBUG, fmt, ##__VA_ARGS__)
#define log_info(fmt, ...)  _LOG(LOG_INFO,  fmt, ##__VA_ARGS__)
#define log_warn(fmt, ...)  _LOG(LOG_WARN,  fmt, ##__VA_ARGS__)
#define log_error(fmt, ...) _LOG(LOG_ERROR, fmt, ##__VA_ARGS__)
#define log_fatal(fmt, ...) do { \
    _LOG(LOG_FATAL, fmt, ##__VA_ARGS__); \
    exit(EXIT_FAILURE); \
} while(0)

/* ======================
   Special Logging Macros
   ====================== */
#define log_perror(msg) _LOG(LOG_ERROR, "%s: %s", msg, strerror(errno))

#define log_var(var, fmt) _LOG(LOG_DEBUG, #var " = " fmt, var)

#define log_enter() _LOG(LOG_DEBUG, "Entering %s()", __func__)
#define log_exit()  _LOG(LOG_DEBUG, "Exiting %s()", __func__)
#define log_exit_val(val, fmt) _LOG(LOG_DEBUG, "Exiting %s() with " fmt, __func__, val)

/* ======================
   Log Level Control
   ====================== */
#define log_set_level(level) do { \
    extern LogLevel log_current_level; \
    log_current_level = level; \
} while(0)

#define log_get_level() (LOG_LEVEL)

#endif /* LOGGER_H */
