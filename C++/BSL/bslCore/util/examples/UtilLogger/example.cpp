#include "UtilLogger.hpp"

#include <iostream>

int main() {

    /*
        Higher log levels display all levels below them with the exception of OFF
        If the logger is OFF, nothing will display

        INFO  = 0,
        WARN  = 1,
        DEBUG = 2,
        TRACE = 3,
        OFF   = 4
    */

    // Logger starts 'off'
    UTIL::Logger * pLogger = new UTIL::Logger("WarningLogger");
    pLogger->setLevel(UTIL::Logger::Level::WARN);

    PLOG_WARN(pLogger) << "A warning logger made as a pointer\n";


    UTIL::Logger logger("DebugLogger");
    logger.setLevel(LL_DEBUG);

    LOG_WARN(logger) << "Warn called while in debug\n";

    LOG_TRACE(logger) << "This won't be called while in debug\n";

    LOG_DEBUG(logger) << "A debug logger\n";

    LOG_DEBUG(logger) << "The function that called me was : " << LOG_FUNC_INFO << "\n";

    logger.setLevel(LL_TRACE);

    LOG_TRACE(logger) << "A trace call\n";

    logger.setLevel(LL_INFO);
    
    LOG_INFO(logger) << "An info call\n";

    LOG_WARN(logger) << "A warn call while in info! \n";



}