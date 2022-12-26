//
// Created by 唐仁初 on 2021/11/2.
//

#include "LoggerControl.h"

namespace logger {

    class ALogger {
    private:
        log4cxx::LoggerPtr FILE_LOGGER;

    public:

        void debug(const std::string &str) {
            LOG4CXX_DEBUG(FILE_LOGGER, str.c_str());
        }


        void info(const std::string &str) {
            LOG4CXX_INFO(FILE_LOGGER, str.c_str());
        }

        void warn(const std::string &str) {
            LOG4CXX_WARN(FILE_LOGGER, str.c_str());

        }

        void error(const std::string &str) {
            LOG4CXX_ERROR(FILE_LOGGER, str.c_str());

        }

        void fatal(const std::string &str) {
            LOG4CXX_FATAL(FILE_LOGGER, str.c_str());
        }

        void init(const char *log_conf_file, const char *appender_name) {
            PropertyConfigurator::configure(log_conf_file);
            FILE_LOGGER = Logger::getLogger(appender_name);
        }
    };

//void ALogger::debug(std::string str) {
//    LOG4CXX_DEBUG(FILE_LOGGER, str.c_str())
//}

    void LoggerController::debug(const std::string &str) {
        log_ptr->debug(str);
    }

    void LoggerController::info(const std::string &str) {
        log_ptr->info(str);
    }

    void LoggerController::warn(const std::string &str) {
        log_ptr->warn(str);
    }

    void LoggerController::error(const std::string &str) {
        log_ptr->error(str);
    }

    void LoggerController::fatal(const std::string &str) {
        log_ptr->fatal(str);
    }

    LoggerController::LoggerController() {
        log_ptr = new ALogger;
    }

    void LoggerController::init(const char *log_conf_file, const char *appender_name) {
        log_ptr->init(log_conf_file, appender_name);
    }


}
