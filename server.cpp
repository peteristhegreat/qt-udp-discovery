#include "server.h"
#include <QDebug>
#include <QHostInfo>

Server::Server(QObject *parent) :
    QObject(parent)
{
    qDebug() << Q_FUNC_INFO;
    m_state = IDLE;
    m_tcpSocket = 0;
    m_udpSocket = 0;
    m_tcpServer = 0;
    m_connected = false;
    // Starts by broadcasting 3 times and listening?

    // If a connection is heard, it then starts up a TCPSocket to the broadcast address
    // otherwise it keeps an open socket to be connected to.
}

void Server::connectToTcpServer()
{
    qDebug() << Q_FUNC_INFO;
    QList<QHostAddress> add = QHostInfo::fromName(QHostInfo::localHostName()).addresses();

    if(add.contains(m_hostAddress))
    {
        // We are trying to connect to our local machine!
        qDebug() << "We are trying to connect on the same machine!";

        // notify the sender?
    }

    m_tcpSocket = new QTcpSocket;
    QObject::connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(on_tcpReadyRead()));
    m_tcpSocket->connectToHost(m_hostAddress, MY_PORT + 1);

    linkTcpSocket();
}

void Server::linkTcpSocket()
{
//    m_state = CONNECTED_TCP_SOCKET;
//    QObject::connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(on_tcpSocketError()));
//    emit msg("connected!");
//    emit connected();
    m_state = CONNECTED_TCP_SOCKET;
    QObject::connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(on_tcpSocketError()));
    m_connected = true;
    emit msg("connected!");
    emit connected();
}

void Server::on_tcpReadyRead()
{
    QString str = m_tcpSocket->readAll();
    qDebug() << Q_FUNC_INFO;
    qDebug() << "\t" << qPrintable(str);

    emit data(str);
}

void Server::on_newTcpConnection()
{
    qDebug() << Q_FUNC_INFO;
    m_tcpSocket = m_tcpServer->nextPendingConnection();
    connect(m_tcpSocket, SIGNAL(readyRead()),
            this, SLOT(on_tcpReadyRead  ()));
//    connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
//            this, SLOT(displayError(QAbstractSocket::SocketError)));

//    serverStatusLabel->setText(tr("Accepted connection"));
    m_tcpServer->close();
//    qDebug() << "Sending: Testing new TCP Connection!";
//    m_tcpSocket->write("Testing new TCP Connection!");

    linkTcpSocket();
}

void Server::on_tcpSocketError()
{
    emit msg(m_tcpSocket->errorString());
    m_connected = false;
}

void Server::broadcastUdp()
{
    qDebug() << Q_FUNC_INFO;

    static int messageNo = 0;
    m_udpSocket = new QUdpSocket(this);
    QByteArray datagram = "Jotto! " + QByteArray::number(messageNo);
    connect(m_udpSocket, SIGNAL(readyRead()),
            this, SLOT(readByBroadcaster()));
    m_udpSocket->writeDatagram(datagram.data(), datagram.size(),
                             QHostAddress::Broadcast, MY_PORT);
    messageNo++;
}

void Server::readByBroadcaster()
{
    qDebug() << Q_FUNC_INFO;
}

void Server::listenForUdpBroadcast()
{
    emit msg("waiting for 2nd player...");
    qDebug() << Q_FUNC_INFO;

    m_udpSocket = new QUdpSocket(this);
    m_udpSocket->bind(MY_PORT, QUdpSocket::ShareAddress);
//    m_udpSocket->bind(QHostAddress::LocalHost, MY_PORT);

    connect(m_udpSocket, SIGNAL(readyRead()),
            this, SLOT(readPendingDatagrams()));

    m_state = Server::LISTENING_UDP;
}

void Server::readPendingDatagrams()
{
    qDebug() << Q_FUNC_INFO;

    // m_state = LISTENING_UDP;

    // listener for datagrams heard...
    while (m_udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_udpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;


        m_udpSocket->readDatagram(datagram.data(), datagram.size(),
                                &sender, &senderPort);
        qDebug() << "Server Found!";
        qDebug() << sender.toString() << senderPort;

        m_hostAddress = sender;
        m_port = senderPort;

        m_udpSocket->disconnectFromHost();


        QByteArray datagram2 = "Received Message!  L -> B";
        m_udpSocket->writeDatagram(datagram2.data(), datagram2.size(),
                                 sender, m_port);

        processTheDatagram(datagram);

        // Transition from LISTENING_UDP, to connecting to the
        // TCP SERVER
        connectToTcpServer();

//        m_state = CONNECTED_AS_TCP_CLIENT;
    }
}

void Server::startTcpServer()
{
    qDebug() << Q_FUNC_INFO;

    m_tcpServer = new QTcpServer;
    m_tcpServer->listen(QHostAddress::Any, MY_PORT + 1);
    QObject::connect(m_tcpServer, SIGNAL(newConnection()),
                     this, SLOT(on_newTcpConnection()));
}

void Server::processTheDatagram(QByteArray & ba)
{
    qDebug() << Q_FUNC_INFO;
    qDebug() << ba.size() << " --- " << qPrintable(QString(ba));
}

void Server::dumpMachineAddresses()
{
    qDebug() << Q_FUNC_INFO;
    QList<QHostAddress> add = QHostInfo::fromName(QHostInfo::localHostName()).addresses();
    foreach(QHostAddress a, add)
    {
        qDebug() << a.toString();
    }

    qDebug() << "This machines's IP/MAC addresses are:";
}

void Server::writeData(QString s)
{
    if(m_tcpSocket)
        m_tcpSocket->write(qPrintable(s));
}
