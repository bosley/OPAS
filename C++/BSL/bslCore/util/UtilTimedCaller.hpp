#ifndef BSLCORE_TIMED_CALLER
#define BSLCORE_TIMED_CALLER

#include "UtilTimer.hpp"
#include "UtilTickableIf.hpp"

#include <vector>

namespace UTIL
{
    //!
    //! \class TimedCallableIf
    //! \brief An object that can be called periodically by a timed caller
    //!
    class TimedCallableIf{
        public:
            virtual ~TimedCallableIf() = default;
            virtual void call()        = 0;
    };

    //!
    //! \class TimedCaller
    //! \brief The timed caller object that will trigger callable interfaces periodically
    //!
    class TimedCaller : public Tickable {
        public:
            //!
            //! \brief Constructor
            //!
            TimedCaller();

            //!
            //! \brief Constructor with a reservation for call vector
            //! \param reserve Amount of memory to reserve for callable objects
            //! \post  The storage unit for callables will have space reserved in memory for registration
            //!
            TimedCaller(uint64_t reserve);

            //!
            //! \brief Destructor
            //!
            ~TimedCaller();

            //!
            //! \brief Register a callable object with the timed caller
            //! \param callTimer The timer that will be used to determine when the object will be called
            //! \param callable The object to be called when the timer is expired
            //! \param repeatCalls If true, the timer will be reset and the object called again. Else, the callable will be removed  
            //! \post The data will be scheduled to be called. Does not check if already called for a given object
            //!
            void registerCallable(UTIL::Timer callTimer, TimedCallableIf& callable, bool repeatCalls=true);

            // From TickableIf
            void tick() override;

        private:
            struct CallableData {
                UTIL::Timer       callTimer;
                TimedCallableIf * callable;
                bool              repeat;
            };

            std::vector<CallableData> callList;
    };
}

#endif