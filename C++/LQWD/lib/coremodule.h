#ifndef LQWD_CORE_MODULE
#define LQWD_CORE_MODULE

#include <QDebug>
#include <QObject>

#include "coreprocess.h"
#include "../widgets/console/defines.h"
#include "../widgets/console/errormachine.h"
#include "../widgets/console/consolesettings.h"

class CoreModule : public QObject
{
    Q_OBJECT
public:
    explicit CoreModule(QObject *parent = 0);

    // Module ID, and Settings required by Bkons
    int moduleId;
    ConsoleSettings settings;

    QString getCWD();

signals:
    // Core Process is done, send back response
    void responseReady(QString, bool);

    // Send Data to core process
    void processCommand(QString);

public slots:
    // Get human's data and pass to core process
    void humanInput(QString);

    // Get response back from core process
    void processReturn(QString, bool);

private:
    // Error handler
    ErrorMachine errM;
    CoreProcess cProc;
};

#endif // LQWD_CORE_MODULE
