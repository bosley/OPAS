#ifndef NETMULTICASTSENDER_H
#define NETMULTICASTSENDER_H

#include <QObject>
#include <QtNetwork>

#include "netcommon.h"

namespace NET
{

class MulticastSender : public QObject
{
    Q_OBJECT
public:
    explicit MulticastSender(IPV version, QString group, int ttl, int port, QObject *parent = nullptr);

    void setTtl(int ttl);

public slots:
    void send(QByteArray datagram);

signals:
    void complete();
    void bindError(QString message);

private:
    QUdpSocket udpSocket4;
    QUdpSocket udpSocket6;
    QHostAddress groupAddress4;
    QHostAddress groupAddress6;
    bool errorPresent;
    IPV ipv;
    int theTtl;
    int thePort;
};

} // NET
#endif // NETMULTICASTSENDER_H
