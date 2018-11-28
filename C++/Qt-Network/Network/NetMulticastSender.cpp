#include "NetMulticastSender.h"

namespace NET
{

MulticastSender::MulticastSender(IPV version, QString group, int ttl, int port, QObject *parent) : QObject(parent)
{
    theTtl = ttl;
    thePort = port;
    errorPresent = false;

    if(version == IPV::IPV4)
    {
        groupAddress4.setAddress(group);
        udpSocket4.bind(QHostAddress(QHostAddress::AnyIPv4), 0);
    }
    else if(version == IPV::IPV6)
    {
        groupAddress6.setAddress(group);
        udpSocket6.bind(QHostAddress(QHostAddress::AnyIPv6), udpSocket4.localPort());

        if(udpSocket6.state() != QAbstractSocket::BoundState)
        {
            errorPresent = true;
            emit bindError(QString("Unable to bind to : [%1]").arg(groupAddress6.toString()));
        }
    }
    ipv = version;
}

void MulticastSender::setTtl(int ttl)
{
    theTtl = ttl;
}

void MulticastSender::send(QByteArray datagram)
{
    if(ipv == IPV::IPV4)
    {
        udpSocket4.writeDatagram(datagram, groupAddress4, thePort);
    }
    else if(ipv == IPV::IPV6)
    {
        if(errorPresent)
            return;

        if (udpSocket6.state() == QAbstractSocket::BoundState)
        {
            udpSocket6.writeDatagram(datagram, groupAddress6, thePort);
        }
        else
        {
            errorPresent = true;
            emit bindError(QString("IPV6 not in a bound state"));
            return;
        }
    }
    emit complete();
}

} // NET
