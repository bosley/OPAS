#include "coremodule.h"

CoreModule::CoreModule(QObject *parent) :
    QObject(parent)
{
    errM.consoleOut("Core Module Inititated");

    // Load the UI settings
    settings.loadFile(":/config/coreModule/core_config.lqwd");

    // Connect to core process
    connect(this, SIGNAL(processCommand(QString)), &cProc, SLOT(processCommand(QString)));
    connect(&cProc, SIGNAL(processComplete(QString, bool)), this, SLOT(processReturn(QString, bool)));
}

/*
    Return the working directory
 */
QString CoreModule::getCWD()
{
    return cProc.getCWD();
}

/*
    Slot that recieves the command from the user
*/
void CoreModule::humanInput(QString cmd)
{
    emit processCommand(cmd);
}

/*
    Slot that recieves the response from the process
*/
void CoreModule::processReturn(QString res, bool w)
{
    emit responseReady(res, w);
}



