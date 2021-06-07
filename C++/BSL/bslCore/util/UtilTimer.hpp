#ifndef BSLCORE_UTIL_TIMER
#define BSLCORE_UTIL_TIMER

#include <chrono>

namespace UTIL
{
    //!
    //! \class Timer
    //! \brief Simple timer object
    //!
    class Timer {
        public:
            //!
            //! \brief Create a simple timer
            //! \post Timer is set to 0 seconds 
            //!
            Timer();

            //!
            //! \brief Create a timer given an amount of time
            //! \param seconds Number of seconds before timer is expired
            //!
            Timer(double seconds);

            //!
            //! \brief Destructor
            //!
            ~Timer();

            //!
            //! \brief Checks if the timer has expired
            //! \retval true The timer has expired
            //! \retval false The timer has not expired
            //!
            bool isExpired();

            //!
            //! \brief Resets the timer back to the time it was created with
            //!
            void reset();

        private:
            std::chrono::duration<double> seconds;
            std::chrono::time_point<std::chrono::system_clock> start;
        
    };
}

#endif