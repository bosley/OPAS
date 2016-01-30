#ifndef ENGINE_H
#define ENGINE_H

#include <QObject>
#include "../sockets/sockethandler.h"

class Engine : public QObject
{
    Q_OBJECT
public:
    explicit Engine(QObject *parent = 0);
    void retrieveNewJob();

signals:

public slots:

private:
    SocketHandler socketH;
};

#endif // ENGINE_H
