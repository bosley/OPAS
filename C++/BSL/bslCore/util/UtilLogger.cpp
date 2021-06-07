#include "UtilLogger.hpp"

namespace UTIL
{
    // -----------------------------------------------------------
    //  logger
    // -----------------------------------------------------------
    Logger::Logger(std::string name) : os(&std::cout), 
                                       classLogLevel(Level::OFF),
                                       name(name) {

    }

    // -----------------------------------------------------------
    //  setLevel
    // -----------------------------------------------------------
    void Logger::setLevel(Level level) {

        classLogLevel = level;
    }

    // -----------------------------------------------------------
    //  log
    // -----------------------------------------------------------
    Logger &Logger::log(Level level) {

        requestedLogLevel = level;

        if(classLogLevel == Level::OFF || requestedLogLevel <= classLogLevel) {
            *os << levelToString(requestedLogLevel) << "(" << name << ") ";
        }

        return *this;
    }

    // -----------------------------------------------------------
    //  setStream
    // -----------------------------------------------------------
    void Logger::setStream(std::ostream &o) {
        
        os = &o;
    }

    // -----------------------------------------------------------
    //  levelToString
    // -----------------------------------------------------------
    std::string Logger::levelToString(Level level) {
        switch(level) {
            case Level::TRACE: return std::string("[TRACE] ");
            case Level::WARN:  return std::string("[WARN]  ");
            case Level::INFO:  return std::string("[INFO]  ");
            case Level::DEBUG: return std::string("[DEBUG] ");
            case Level::OFF:   return std::string("[OFF]   ");
        };
        return std::string("[NULL]");
    }
}