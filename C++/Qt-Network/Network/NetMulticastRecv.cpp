#include "NetMulticastRecv.h"

namespace NET
{

MulticastRecv::MulticastRecv(IPV version, QString group, int port, QObject *parent) : QObject(parent)
{
    thePort = port;
    ipv = version;

    if(version == IPV::IPV4)
    {
        groupAddress4.setAddress(group);
        udpSocket4.bind(QHostAddress::AnyIPv4, port, QUdpSocket::ShareAddress);
        udpSocket4.joinMulticastGroup(groupAddress4);

        connect(&udpSocket4, SIGNAL(readyRead()),
                this, SLOT(processDatagrams()));
    }
    else if(version == IPV::IPV6)
    {
        groupAddress6.setAddress(group);
        if (!udpSocket6.bind(QHostAddress::AnyIPv6, port, QUdpSocket::ShareAddress) ||
                !udpSocket6.joinMulticastGroup(groupAddress6))
        {
            errorPresent = true;
            emit bindError("Unable to bind IPv6");
        }
        else
        {
            connect(&udpSocket6, &QUdpSocket::readyRead, this, &MulticastRecv::processDatagrams);
        }
    }
}

void MulticastRecv::processDatagrams()
{
    QByteArray datagram;
    if(ipv == IPV::IPV4)
    {
        while (udpSocket4.hasPendingDatagrams())
        {
            datagram.resize(int(udpSocket4.pendingDatagramSize()));
            udpSocket4.readDatagram(datagram.data(), datagram.size());
        }
        emit dataReady(datagram);
    }
    else if(ipv == IPV::IPV6)
    {
        if(errorPresent)
            return;

        while (udpSocket6.hasPendingDatagrams())
        {
            datagram.resize(int(udpSocket6.pendingDatagramSize()));
            udpSocket6.readDatagram(datagram.data(), datagram.size());
        }
        emit dataReady(datagram);
    }
}

} // NET
