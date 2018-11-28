#ifndef NETMULTICASTRECV_H
#define NETMULTICASTRECV_H

#include <QObject>
#include <QHostAddress>
#include <QUdpSocket>

#include "netcommon.h"

namespace NET
{

class MulticastRecv : public QObject
{
    Q_OBJECT
public:
    explicit MulticastRecv(IPV version, QString group, int port, QObject *parent = nullptr);

signals:
    void dataReady(QByteArray datagram);
    void bindError(QString message);

public slots:

private slots:
    void processDatagrams();

private:
    QUdpSocket udpSocket4;
    QUdpSocket udpSocket6;
    QHostAddress groupAddress4;
    QHostAddress groupAddress6;
    IPV ipv;
    int thePort;
    bool errorPresent;
};

} // NET
#endif // NETMULTICASTRECV_H
