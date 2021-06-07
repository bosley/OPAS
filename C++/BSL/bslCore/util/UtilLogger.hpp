#ifndef BLSCORE_LOGGER_HPP
#define BLSCORE_LOGGER_HPP

#include <string>
#include <iostream>
#include <sstream>
#include <mutex>

namespace UTIL
{
    // Standard way to get function name
    #define LOG_FUNC_INFO __FUNCTION__

    // Log levels made simple
    #define LL_TRACE UTIL::Logger::Level::TRACE
    #define LL_WARN  UTIL::Logger::Level::WARN
    #define LL_INFO  UTIL::Logger::Level::INFO
    #define LL_DEBUG UTIL::Logger::Level::DEBUG

    // Loggers that are not pointers
    #define LOG_TRACE(logger) logger.log(LL_TRACE)
    #define LOG_WARN( logger) logger.log(LL_WARN)
    #define LOG_INFO( logger) logger.log(LL_INFO)
    #define LOG_DEBUG(logger) logger.log(LL_DEBUG)

    // Loggers that are pointers
    #define PLOG_TRACE(logger) logger->log(LL_TRACE)
    #define PLOG_WARN( logger) logger->log(LL_WARN)
    #define PLOG_INFO( logger) logger->log(LL_INFO)
    #define PLOG_DEBUG(logger) logger->log(LL_DEBUG)

    //!
    //! \class Logger
    //! \brief A simple logger class that spits out data depending on the log level
    //!        The higher the log level, the more information will be displayed / output
    //!        - This is not thread safe. It is recommended that a new logger be used for each thread
    //!
    class Logger {
        public:

            //!
            //! \class Level
            //!
            enum class Level {
                WARN  = 0, //! Output WARN calls
                INFO  = 1, //! Will only output information at the INFO  and WARN
                DEBUG = 2, //! Output DEBUG calls, and WARN/INFO
                TRACE = 3, //! Logs everything DEBUG/WARN/INFO and TRACE calls
                OFF   = 4  //! Disables logging 
            };

            //!
            //! \brief Creates a logger
            //! \param name The logger name
            //!
            Logger(std::string name);

            //!
            //! \brief Get the global logger
            //! \returns Pointer to the logger
            //!
            static Logger* getLogger();

            //!
            //! \brief Log something at a specific level
            //! \param level The level to log
            //!
            Logger & log(Level level);

            //!
            //! \brief Operator overload
            //! \param msg The data to log
            //!
            template<class T>
            Logger &operator<<(const T &msg) {
                if(classLogLevel == Level::OFF || requestedLogLevel <= classLogLevel) {
                    *os << msg;
                }
                return *this;
            }

            //!
            //! \brief Sets the output stream for the logger
            //! \param os The stream to output to
            //! \post The logger will output to whatever stream is handled. It is recommended that this is only called once
            //!       Otherwise threaded applications could have data come out weird
            //!
            void setStream(std::ostream &os);

            //!
            //! \brief Sets the logger's level
            //! \param level The level to set
            //! \post The logger will output anything with a level with a value smaller than the current log level given here
            //        Unless the level is set to OFF, Then nothing will come out.. anywhere
            //!
            void setLevel(Level level);

        private:
            Logger(Logger const&) : os(&std::cout), classLogLevel(Level::OFF) {};
            
            Logger& operator=(Logger const&){ return *this; };

            std::string levelToString(Level level);

            std::ostream *os;

            Level classLogLevel;

            std::string name;

            Level requestedLogLevel;
    };
}





#endif