#ifndef BUS_H
#define BUS_H

#include <map>
#include <mutex>
#include <vector>
#include <string>

#include <iostream>

#define BUS_SUB_ID_0 0x00
#define BUS_SUB_ID_1 0x01
#define BUS_SUB_ID_2 0x02
#define BUS_SUB_ID_3 0x03
#define BUS_SUB_ID_4 0x04
#define BUS_SUB_ID_5 0x05

#define BUS_SIZE_MAX 10000

namespace MOD { namespace BUS {

template<class T>
class Subscriber
{
public:
    virtual ~Subscriber() = default;
    virtual int  getId() const = 0;
    virtual void busNotification(T data) = 0;
    virtual void unsubscribed(std::string publisher) = 0;
};

template<class T>
class Bus
{
public:
    Bus(){}

    void newPublisher(std::string publisher)
    {
        if(!publisherExists(publisher))
        {
            std::lock_guard<std::mutex> lock(chanMutex);
            channels[publisher] = std::vector< Subscriber<T>* >();
        }
    }

    bool newSubscriber(std::string publisher, Subscriber<T> * subscriber)
    {
        if(!publisherExists(publisher))
        {
            return false;
        }

        if(isSubscribed(publisher, subscriber))
        {
            return false;
        }

        std::lock_guard<std::mutex> lock(chanMutex);
        channels[publisher].push_back(subscriber);

        return true;
    }

    bool remPublisher(std::string publisher)
    {
        if(!publisherExists(publisher))
        {
            return false;
        }

        std::lock_guard<std::mutex> lock(chanMutex);
        channels[publisher].clear();
        channels.erase(publisher);
    }

    bool unsubscribe(std::string publisher, Subscriber<T> * subscriber)
    {
        if(!publisherExists(publisher))
        {
            return false;
        }

        if(!isSubscribed(publisher, subscriber))
        {
            return false;
        }

        auto i = channels[publisher].begin();
        for(;  i != channels[publisher].end();
               ++i)
        {
            if((*i)->getId() == subscriber->getId())
            {
                break;
            }
        }

        if(i == channels[publisher].end())
        {
            return false;
        }

        std::lock_guard<std::mutex> lock(chanMutex);
        channels[publisher].erase(i);

        return true;
    }

    bool publish(std::string publisher, T item)
    {
        if(!publisherExists(publisher))
        {
            return false;
        }

        for(auto i =  channels[publisher].begin();
                 i != channels[publisher].end();
               ++i)
        {
            (*i)->busNotification(item);
        }
        return true;
    }


private:

    bool publisherExists(std::string publisher)
    {
        for(auto i =  channels.begin();
                 i != channels.end();
                 ++i)
        {
            if(i->first == publisher)
            {
                return true;
            }
        }
        return false;
    }

    bool isSubscribed(std::string publisher, Subscriber<T>* subscriber)
    {
        if(!publisherExists(publisher))
        {
            return false;
        }

        for(auto i =  channels[publisher].begin();
                 i != channels[publisher].end();
               ++i)
        {
            if((*i)->getId() == subscriber->getId())
            {
                return true;
            }
        }
        return false;
    }

    std::mutex chanMutex;
    std::map<std::string, std::vector<Subscriber<T>*> > channels;
};

}}
#endif // BUS_H
