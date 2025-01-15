#ifndef CLI_LOGGER_H
#define CLI_LOGGER_H

#include <stdio.h>
#include <stdarg.h>

// Define API_EXPORT based on platform and build settings
#if defined(_WIN32) || defined(__CYGWIN__)
#ifdef BUILDING_DLL
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT __declspec(dllimport)
#endif
#else
#define API_EXPORT __attribute__((visibility("default")))
#endif

// ANSI color codes
#define COLOR_RED "\x1b[31m"    // Red
#define COLOR_YELLOW "\x1b[33m" // Yellow
#define COLOR_GREEN "\x1b[32m"  // Green
#define COLOR_RESET "\x1b[0m"   // Reset

// Log levels
typedef enum
{
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR
} LogLevel;

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Logs an informational message.
     *
     * @param format The format string (printf-style).
     * @param ...    Additional arguments.
     */
    API_EXPORT void log_info(const char *format, ...);

    /**
     * @brief Logs a warning message.
     *
     * @param format The format string (printf-style).
     * @param ...    Additional arguments.
     */
    API_EXPORT void log_warning(const char *format, ...);

    /**
     * @brief Logs an error message.
     *
     * @param format The format string (printf-style).
     * @param ...    Additional arguments.
     */
    API_EXPORT void log_error(const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif // CLI_LOGGER_H
