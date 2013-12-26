#ifndef MAINSTACK_H
#define MAINSTACK_H

#include <QtWidgets/QStackedWidget>
#include "server.h"
#include <QLineEdit>

class MainStack : public QStackedWidget
{
    Q_OBJECT
public:
    explicit MainStack(QWidget *parent = 0);
signals:
    void data(QString);
public slots:
    void on_createGame();
    void on_connectToGame();
    void on_connected();
    void sendData();
private:
    void init_gui();
    QLineEdit * m_lineEdit;
    Server * m_server;
};

#endif // MAINSTACK_H
