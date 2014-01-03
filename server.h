#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QUdpSocket>
#include <QTcpServer>
#include <QTcpSocket>
#include <QString>
//#include <QTimerEvent>

#define MY_PORT 45234

class Server : public QObject
{
    Q_OBJECT
public:
    enum STATE
    {
        IDLE = 0,
        LISTENING_UDP,// Port
        BROADCASTING_AND_SERVER, // Port, Port + 1
        CONNECTED_TCP_SERVER, // Port + 1
        CONNECTED_TCP_SOCKET // Port + 1 (or Port + 2 if host address == local address)
    } m_state;


    explicit Server(QObject *parent = 0);
    void broadcastUdp();
    void listenForUdpBroadcast();
    void startTcpServer();
    void processTheDatagram(QByteArray &ba);
    void connectToTcpServer();
    void dumpMachineAddresses();
    bool isConnected() {return m_connected;}
signals:
    void msg(QString);
    void data(QString);
    void secretWord(QString);
    void connected();
    void disconnected();
public slots:
    void on_tcpSocketError();
    void writeData(QString);
    void on_tcpReadyRead();
    void readByBroadcaster();
    void on_newTcpConnection();
    void readPendingDatagrams();

private:
    bool m_connected;
    void linkTcpSocket();
    QHostAddress m_hostAddress;
    quint16 m_port;
    QTcpServer * m_tcpServer;
    QTcpSocket * m_tcpSocket;
    QUdpSocket * m_udpSocket;
};

#endif // SERVER_H
