#include<QDebug>
#include "errormachine.h"

ErrorMachine::ErrorMachine(QObject *parent) :
    QObject(parent)
{
}

void ErrorMachine::consoleOut(QString e)
{
    qDebug() << e ;
}

void ErrorMachine::messageBox(QString m)
{
    MessageBox * mb = new MessageBox(m);
    mb->show();
}

bool ErrorMachine::responseBox(QString)
{
    // Show yes/no response box and reply
    return true;
}

void ErrorMachine::catchError(QString err, int arg)
{
    // Error that the user needs to know about. arg == -1 = terminate
    qDebug() << "CAUGHT: " << err ;
    if( arg != 0 )
        qDebug() << "ARG : " << arg;
}

void ErrorMachine::handleProcessError(QProcess::ProcessError e)
{
    qDebug() << e;
}
