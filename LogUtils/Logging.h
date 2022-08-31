//
// Created by 唐仁初 on 2021/11/1.
//

#ifndef TCPTEST_LOGGING_H
#define TCPTEST_LOGGING_H


#include "LoggerControl.h"
#include <cstdarg>

#define LOGCTL_INSTANCE LoggerController::getInstance()

static std::string get_format_str(const char *format_str, ...) {
    va_list args;
    va_start(args, format_str);
    char buf[1024] = "";
    vsnprintf(buf, sizeof(buf), format_str, args);
    va_end(args);
    return buf;
}

#define LOG_INFO(format_str, ...) { LOGCTL_INSTANCE.info(std::move(get_format_str(format_str, __VA_ARGS__)));}

#define LOG_DEBUG(format_str, ...) {LOGCTL_INSTANCE.debug(std::move(get_format_str(format_str, __VA_ARGS__)));}

#define LOG_WARN(format_str, ...) { LOGCTL_INSTANCE.warn(std::move(get_format_str(format_str, __VA_ARGS__))); }

#define LOG_ERROR(format_str, ...) { LOGCTL_INSTANCE.error(std::move(get_format_str(format_str, __VA_ARGS__))); }

#define LOG_FATAL(format_str, ...) { LOGCTL_INSTANCE.fatal(std::move(get_format_str(format_str, __VA_ARGS__)));}

#endif //TCPTEST_LOGGING_H
