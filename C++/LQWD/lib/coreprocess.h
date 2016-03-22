#ifndef COREPROCESS_H
#define COREPROCESS_H


#include <QDir>
#include <QFile>
#include <QObject>
#include <QProcess>
#include <QFileInfo>
#include <QFileInfoList>

#include "corenexus.h"
#include "../widgets/console/defines.h"
#include "../widgets/console/errormachine.h"

class CoreProcess : public QObject
{
    Q_OBJECT
public:
    explicit CoreProcess(QObject *parent = 0);
    QString getCWD();

signals:
    void processComplete(QString, bool);

public slots:
    void processCommand(QString);

private:

    int outputType;
    QProcess * proc;
    CoreNexus nexus;
    ErrorMachine errM;
    QString currDir;

    QString qtInterface(QStringList);
    QString systemInterface(QStringList);
    QString nixInterface(QStringList);
    QString winInterface(QStringList);

    void setWorkingDirectory(QString);
    QString changeDirectory(QStringList);

    QString sendFileNotExist(QString);
};

#endif // COREPROCESS_H
