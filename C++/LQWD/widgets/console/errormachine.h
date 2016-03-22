#ifndef ERRORMACHINE_H
#define ERRORMACHINE_H

#include <QObject>
#include <QProcess>

#include "defines.h"
#include "messagebox.h"

class ErrorMachine : public QObject
{
    Q_OBJECT
public:
    explicit ErrorMachine(QObject *parent = 0);

signals:

public slots:

    void consoleOut(QString);

    void messageBox(QString);

    bool responseBox(QString);

    void catchError(QString, int arg=0);

    void handleProcessError(QProcess::ProcessError);
};

#endif // ERRORMACHINE_H
