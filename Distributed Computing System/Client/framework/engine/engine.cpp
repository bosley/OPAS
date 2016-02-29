#include "engine.h"

Engine::Engine(QObject *parent) : QObject(parent)
{

}

void Engine::retrieveNewJob()
{
    qDebug() << "Engine requesting job from socket handler";

    socketH.SubmitQuery(REQ_NEW_ITEM);
}
