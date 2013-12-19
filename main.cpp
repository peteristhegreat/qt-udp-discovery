#include <QCoreApplication>
//#include <QtNetwork/QUdpSocket>
#include <QTextStream>
#include <QString>
#include "server.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QTextStream cout(stdout);
    QTextStream cin(stdin);

    qDebug() << "Listen (L) or Broadcast (B)?";// << QTextStream::;
//    QString line = cin.readLine();  // This is how you read the entire line

    QString word;
    cin >> word;

    Server * s = new Server;
//    while(getchar())
    if(word.startsWith("l", Qt::CaseInsensitive))
    {
        s->listenOnPort();
    }
    else
    {
        s->broadcastUdpDatagram();
    }

    return a.exec();
}
