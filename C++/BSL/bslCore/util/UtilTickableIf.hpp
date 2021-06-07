#ifndef BSLCORE_TICKABLE_IF
#define BSLCORE_TICKABLE_IF

namespace UTIL
{
    //!
    //! \class An interface to objects that need to be periodically ticked
    //!
    class Tickable {
        public:
            virtual ~Tickable() = default;

            //!
            //! \brief Inform the tickable object to do its processing
            //!
            virtual void tick() = 0;
    };
}

#endif