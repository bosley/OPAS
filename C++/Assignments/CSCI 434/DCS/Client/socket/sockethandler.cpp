#include "sockethandler.h"

SocketHandler::SocketHandler(QObject *parent)
{
    // Need QObject, but SocketHandler doesn't care about its parents
    Q_UNUSED(parent);

    // Set server information
    host = "HOST";
    port = 5004;
}

void SocketHandler::SubmitQuery(int request_type, QString data)
{
    // Make a new tcp socket, and connect to the server
    socket = new QTcpSocket(this);
    socket->connectToHost(host, port);

    // Wait a few seconds for a connection
    if (socket->waitForConnected(5000))
    {
        // When connected, build a socket query based on parameters
        QString temp;
        qDebug() << "[SOCKET HANDLER] : Connected.";
        // Byte array for converting qstring to const char *
        QByteArray bytes;
        const char * query;

        switch(request_type)
        {
            case REQ_NEW_ITEM:
                qDebug() << "[SOCKET HANDLER] : Requesting new job.";
                temp = "n^" + data;
                break;
            case UPD_CUR_ITEM:
                qDebug() << "[SOCKET HANDLER] : Requesting update on job.";
                temp = "u^" + data;
                break;
            default:
                break;
        }

        // Convert data for sending
        bytes = temp.toLocal8Bit();
        query = bytes.data();

        // Send query
        socket->write(query);
        socket->waitForBytesWritten(10000);

        // Recieve a response from the server
        QByteArray arr;
        while(!arr.contains(SERVER_RECV_DELIMITER))
        {
            socket->waitForReadyRead();
            arr += socket->readAll();
        }

        int b = arr.indexOf(SERVER_RECV_DELIMITER);
        QByteArray message = arr.left(b);
        arr = arr.mid(b);
        qDebug() << "[SOCKET HANDLER] : DONE." ;
        emit dataReady(message);
        socket->close();
    }
    else
    {
        qDebug() << "[SOCKET HANDLER] : COULD NOT CONNECT.";
    }
}
