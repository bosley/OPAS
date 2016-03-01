#ifndef ENGINE_H
#define ENGINE_H

#include <QObject>
#include "work/bsorts.h"
#include "../socket/sockethandler.h"

class Engine : public QObject
{
    Q_OBJECT
public:
    explicit Engine(QObject *parent = 0);
    // Start retrieving jobs from the server. Will not stop until stop signal sent to engine ('s')
    void retrieveNewJob();

public slots:
    // Slots for getting information from socket class, and for getting signals from the user
    void processServerData(QString);
    void listenForSignal(char);

signals:
    // Signal to tell the user a message
    void userSignal(QString message);
    // Signal when done
    void jobCompleted();

private:
    // Socket obj, the running flag, and function(s) for job completion
    Sorts NumberSort;
    SocketHandler socketH;
    bool continueWorking;
    void completeSortingJob(QStringList job);

};

#endif // ENGINE_H
