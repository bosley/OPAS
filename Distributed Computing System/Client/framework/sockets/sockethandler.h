#ifndef SOCKETHANDLER_H
#define SOCKETHANDLER_H

#include <QDebug>
#include <QObject>
#include <QTcpSocket>

#define REQ_NEW_ITEM 1
#define UPD_CUR_ITEM 2

class SocketHandler : public QObject
{
    Q_OBJECT
public:
    explicit SocketHandler(QObject *parent = 0);

    void SubmitQuery(int request_type, QString data = "");

signals:
    void dataReady(QString);

private:
    QTcpSocket *socket;
    QString host;
    int port;
};

#endif // SOCKETHANDLER_H
