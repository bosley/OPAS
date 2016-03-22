#ifndef COMMANDSTRUCTURES_H
#define COMMANDSTRUCTURES_H

#include <QString>

// Arguments for Qt driven commands
struct cArg
{
    QString arg, definition;
    cArg(QString a, QString d)
    {
        arg = a;
        definition = d;
    }
};
typedef struct cArg _carg;

// Commands that are Qt driven
struct command
{
    QString sequenceName, cmd, description;
    QList<_carg> arguments;
};
typedef struct command _cmd;

// Commands that will be translated as-per os and
// are not Qt Driven
struct translations
{
    QString input, translation;
    translations(QString i, QString t)
    {
        input = i;
        translation = t;
    }
};
typedef struct translations _transCmd;

#endif // COMMANDSTRUCTURES_H
