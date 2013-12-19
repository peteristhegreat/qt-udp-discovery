#include "server.h"
#include <QDebug>

Server::Server(QObject *parent) :
    QObject(parent)
{

}

void Server::broadcastUdpDatagram()
{
    static int messageNo = 0;
    udpSocket = new QUdpSocket(this);
    QByteArray datagram = "Jotto! " + QByteArray::number(messageNo);
    udpSocket->writeDatagram(datagram.data(), datagram.size(),
                             QHostAddress::Broadcast, MY_PORT);
    messageNo++;
}

void Server::listenOnPort()
{
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(MY_PORT, QUdpSocket::ShareAddress);

    connect(udpSocket, SIGNAL(readyRead()),
            this, SLOT(readPendingDatagrams()));
}

void Server::initSocket()
{
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::LocalHost, MY_PORT);

    connect(udpSocket, SIGNAL(readyRead()),
            this, SLOT(readPendingDatagrams()));
}

void Server::readPendingDatagrams()
{
    // listener for datagrams heard...
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;


        udpSocket->readDatagram(datagram.data(), datagram.size(),
                                &sender, &senderPort);
        qDebug() << "Server Found!";
        qDebug() << sender.toString() << senderPort;

        processTheDatagram(datagram);

//        qDebug() << "Switching to TCP/IP...";

    }
}

void Server::init_TCPIP()
{

}

void Server::processTheDatagram(QByteArray & ba)
{
    qDebug() << ba.size() << " --- " << qPrintable(QString(ba));
}
