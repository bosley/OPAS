#include "engine.h"

Engine::Engine(QObject *parent) : QObject(parent)
{

}

void Engine::retrieveNewJob()
{
    socketH.SubmitQuery(REQ_NEW_ITEM);
}
