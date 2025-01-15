#include "cli_logger.h"
#include <time.h>
#include <string.h>

// Helper function to get current timestamp as a string
static void get_timestamp(char *buffer, size_t size)
{
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

// Core logging function
static void log_message(LogLevel level, const char *format, va_list args)
{
    char timestamp[20];
    get_timestamp(timestamp, sizeof(timestamp));

    const char *level_str;
    const char *color;

    switch (level)
    {
    case LOG_LEVEL_INFO:
        level_str = "INFO";
        color = COLOR_GREEN;
        break;
    case LOG_LEVEL_WARNING:
        level_str = "WARNING";
        color = COLOR_YELLOW;
        break;
    case LOG_LEVEL_ERROR:
        level_str = "ERROR";
        color = COLOR_RED;
        break;
    default:
        level_str = "UNKNOWN";
        color = COLOR_RESET;
        break;
    }

    fprintf(stdout, "%s[%s] [%s]: ", color, timestamp, level_str);
    vfprintf(stdout, format, args);
    fprintf(stdout, "%s\n", COLOR_RESET);
}

void log_info(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_message(LOG_LEVEL_INFO, format, args);
    va_end(args);
}

void log_warning(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_message(LOG_LEVEL_WARNING, format, args);
    va_end(args);
}

void log_error(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_message(LOG_LEVEL_ERROR, format, args);
    va_end(args);
}
