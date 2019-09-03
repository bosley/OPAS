#ifndef BBUTTONS
#define BBUTTONS

#include <Arduino.h>
#include <Jdll.h>

namespace BTNS
{

typedef void (* buttonCallback) ();

namespace 
{
    struct buttonMap
    {
        unsigned pin;
        buttonCallback callback;
    };
}   

class BButtons
{
public:

    BButtons(){}
    ~BButtons(){}

    void addButton(unsigned inputPin, buttonCallback callback)
    {
        buttonMap bmap;
        bmap.pin = inputPin;
        bmap.callback = callback;
        list.pushBack(bmap);
    }

    void tick()
    {
        for(int i =0; i < list.size; i++)
        {
            buttonMap bmap = list.retrieveDataByIndex(i);
            if(digitalRead(bmap.pin) == HIGH)
                bmap.callback();
        }
    }

private:

    bosley::JDLL<buttonMap> list;
};

}

#endif