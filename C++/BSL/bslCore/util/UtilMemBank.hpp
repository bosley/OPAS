//
//  Bank - A simple memory bank utility to safely store and access objects
//         This will need to grow into an IPC-able and Network-available bank
//
#ifndef BSLCORE_MEM_BANK_HPP
#define BSLCORE_MEM_BANK_HPP

#include <map>
#include <mutex>
#include <deque>
#include <vector>
#include <iostream>

namespace UTIL 
{
    //!
    //! \class MemBank
    //!
    template<class T>
    class MemBank {
    public:
        //! 
        //! \brief Creates a memory bank 
        //! \param depth The size of the memory bank
        //!
        MemBank(uint64_t depth) : depth(depth) {

        }

        //!
        //! \brief Destructor
        //!
        ~MemBank() {

        }

        //!
        //! \brief Get an entry by an index
        //! \param idx The index of the object
        //!
        T* getEntryIdx(uint64_t idx) {

            std::lock_guard<std::mutex> cm(coreMutex);

            if(idx > memoryBank.size()) {
                return nullptr;
            }

            return &memoryBank[idx];
        }

        //!
        //! \brief Get the last piece of information to be placed in the bank
        //!
        T* getNewest() {

            std::lock_guard<std::mutex> cm(coreMutex);

            if(memoryBank.size() == 0) {
                return nullptr;
            }
            return &memoryBank[memoryBank.size()-1];
        }

        //!
        //! \brief Push data into the bank
        //! \param data The data to push into the bank
        //!
        bool pushData(T data) {

            std::lock_guard<std::mutex> cm(coreMutex);


            memoryBank.push_back(data);

            if(memoryBank.size() > depth) {
                memoryBank.pop_front();
            }
            return true;
        }

        //!
        //! \brief Get a frame of memory
        //! \param start The beginning point
        //! \param end   The last frame (inclusive)
        //! \returns A vector of type <T> 
        //!
        std::vector<T> getFrame(uint64_t start, uint64_t end) {

            std::vector<T> returnVector;

            if(end < start) {
                return returnVector;
            }

            std::lock_guard<std::mutex> cm(coreMutex);

            if(memoryBank.size() <= end) {
                end = memoryBank.size()-1;
            }

            if(memoryBank.size() <= start) {
                start = memoryBank.size()-1;
            }

            for(uint64_t i = start; i <= end; i++) {
                returnVector.push_back(
                            memoryBank[i]
                            );
            }

            return returnVector;
        }

        //!
        //! \brief Clear out all data in the memory bank
        //!
        void clear() {
            std::lock_guard<std::mutex> cm(coreMutex);

            memoryBank.clear();
        }

    private:
        uint64_t depth;
        std::mutex coreMutex;
        std::deque<T> memoryBank;

    };
}

#endif // MEM_BANK_HPP
