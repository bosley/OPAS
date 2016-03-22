#ifndef COMMANDNEXUS_H
#define COMMANDNEXUS_H

#include <QFile>
#include <QList>
#include <QDebug>
#include <QString>
#include <QStringList>

#include "commandStructures.h"
#include "../widgets/console/defines.h"
#include "../widgets/console/errormachine.h"


class CoreNexus
{
public:
    CoreNexus();
    ~CoreNexus();

    QStringList retrieveCommand(QString cmd);

private :
    ErrorMachine errM;
    QList<_cmd> commandMap;
    QList<_transCmd> translationMap;

    // Explicitly undefined os arguments (indicated by ! in command_map.lqwd)
    // will be ignored, flip bool to true to allow
    bool ignoreExplicitlyUndefinedArguments;

    void loadCommandMap();
    QStringList filterCommand(QString);
};

#endif // COMMANDNEXUS_H
