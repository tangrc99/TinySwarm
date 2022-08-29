//
// Created by 唐仁初 on 2021/11/1.
//

#ifndef TCPTEST_LOGGERCONTROL_H
#define TCPTEST_LOGGERCONTROL_H


#include <log4cxx/logger.h>
#include <log4cxx/logmanager.h>
#include <log4cxx/propertyconfigurator.h>

using namespace log4cxx;


class ALogger;

class LoggerController {
private:

    ALogger *log_ptr;

    LoggerController();

public:

    void init(const char *log_conf_file, const char *appender_name);

    static LoggerController getInstance() {
        static LoggerController T;
        return T;
    }

    static void exit() {
        log4cxx::LogManager::shutdown();
    }

    void debug(const std::string &str) ;

    void info(const std::string &str) ;

    void warn(const std::string &str) ;

    void error(const std::string &str) ;

    void fatal(const std::string &str) ;


};



#endif //TCPTEST_LOGGERCONTROL_H
