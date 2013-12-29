#ifndef MAINSTACK_H
#define MAINSTACK_H

#include <QtWidgets/QStackedWidget>
#include "server.h"
#include <QLineEdit>
#include "dictionary.h"
#include <QStatusBar>

class MainStack : public QStackedWidget
{
    Q_OBJECT
public:
    explicit MainStack(QWidget *parent = 0);
signals:
    void data(QString);
    void setTheme(QColor, QColor, int);
    void resetLetters();
public slots:
    void on_createGame();
    void on_connectToGame();
    void on_connected();
    void on_onePlayer();
    void sendData();
private:
    QStatusBar * m_bar;
    Dictionary * m_dict;
    void init_gui();
    void init_board(bool is_two_player = true);
//    QLineEdit * m_lineEdit;
    Server * m_server;

    QString m_theirSecretWord;
};

#endif // MAINSTACK_H
