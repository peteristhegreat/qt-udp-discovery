#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QUdpSocket>
#include <QTcpServer>

#define MY_PORT 45234

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);
    void initSocket();
    void broadcastUdpDatagram();
    void listenOnPort();
    void init_TCPIP();
    void processTheDatagram(QByteArray &ba);
signals:

public slots:
    void readPendingDatagrams();

private:
    QUdpSocket * udpSocket;
};

#endif // SERVER_H
