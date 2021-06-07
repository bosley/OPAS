
#include "UtilTimer.hpp"
#include "UtilTimedCaller.hpp"

#include <iostream>
#include <string>

namespace {

    class MyCallable : public UTIL::TimedCallableIf {
        public:
            MyCallable(std::string  id) : id(id) {

            }

            void call() override {

                std::cout << "Callable [" << id << "] was called!" << std::endl;
            }

        private:
            std::string id;
    };
}


int main() {

    UTIL::Timer threeSec(3);
    UTIL::Timer fiveSec(5);
    UTIL::Timer tenSec(10);

    MyCallable threeSecCallableOne("First three sec timer - not repeated");
    MyCallable threeSecCallableTwo("Second three sec timer - repeated");
    MyCallable fiveSecCallable("Five sec - repeated");
    MyCallable tenSecCallable("Ten sec - not repeated");

    UTIL::TimedCaller myTimedCaller;

    myTimedCaller.registerCallable(threeSec, threeSecCallableOne, false);
    myTimedCaller.registerCallable(threeSec, threeSecCallableTwo);
    myTimedCaller.registerCallable(fiveSec,  fiveSecCallable);
    myTimedCaller.registerCallable(tenSec,   tenSecCallable, false);

    UTIL::Timer programTimer(60);

    while(!programTimer.isExpired()) {

        myTimedCaller.tick();
        std::cout << std::flush;
    }

    std::cout << "Example complete" << std::endl;
    
    return 0;
}