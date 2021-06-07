
#include "UtilMemBank.hpp"

#include <iostream>
#include <string>

namespace {

    class SomeClass {
        public:
            SomeClass(std::string name) : name(name){
            }   

        std::string name;
    };

    SomeClass myClasses[10] {
        SomeClass("This is the first class"),   // 0
        SomeClass("This is the second class"),  // 1
        SomeClass("This is the third class"),   // 2
        SomeClass("This is the fourth class"),  // 3
        SomeClass("This is the fifth class"),   // 4
        SomeClass("This is the sixth class"),   // 5
        SomeClass("This is the seventh class"), // 6
        SomeClass("This is the eighth class"),  // 7
        SomeClass("This is the best class"),    // 8
        SomeClass("This is the last class")     // 9
    };
}

int main() {

    UTIL::MemBank<SomeClass> myMemBank(200);

    for(int i = 0; i < 10; i++) {

        myMemBank.pushData(myClasses[i]);
    }

    // Will grab the third class 
    SomeClass* thirdClass = myMemBank.getEntryIdx(2);

    std::cout << "Got [" << thirdClass->name << "]" << std::endl;

    // Well grab fifth class up-to and including best class 
    std::vector<SomeClass> frame = myMemBank.getFrame(4, 8);

    for(auto &i : frame) {
        std::cout << "> " << i.name << std::endl;
    }

    return 0;
}