#ifndef CONSOLESETTINGS_H
#define CONSOLESETTINGS_H

#include <QFile>
#include <QHash>
#include <QDebug>
#include <QString>
#include <QTextStream>
#include <QStringList>

#include "defines.h"
#include "errormachine.h"

class ConsoleSettings
{
public:
    ConsoleSettings();


    void loadFile(QString);
    QString getValue(QString key);

private :
    QHash<QString, QString> map;
};

#endif // CONSOLESETTINGS_H
