#ifndef MAINSTACK_H
#define MAINSTACK_H

#include <QtWidgets/QStackedWidget>
#include "server.h"
#include <QLineEdit>
#include "dictionary.h"
#include <QStatusBar>
#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QLabel>

class MainStack : public QStackedWidget
{
    Q_OBJECT
public:
    explicit MainStack(QWidget *parent = 0);
    void updateGuessCount(bool reset = false);
signals:
    void setTheme(QColor, QColor, int);
    void resetLetters();
    void appendToYours(QString);
    void appendToTheirs(QString);
public slots:
    // data is received from the server
    void on_data(QString);
    void on_createGame();
    void on_connectToGame();
    void on_connected();
    void on_onePlayer();
    void on_settingsButton();
    void on_giveUpButton();
    void on_shuffle();
    // line edit with enter pressed
    void sendData();
    void on_helpButton();
    void on_backButton();
    void readSettings();
    void writeSettings();
    void closeEvent(QCloseEvent *);
private:
    QWidget * m_helpPage;
    QWidget * m_onePlayerBoard;
    QWidget * m_twoPlayerBoard;
    QWidget * m_mainMenu;
    QWidget * m_settingsPage;
    QWidget * m_loadingScreen;

    QCheckBox * m_inDictionary;
    QCheckBox * m_correctLength;

    QComboBox * m_numLettersCombo;

    QStatusBar * m_bar;
    Dictionary * m_dict;
    void init_gui();
    void init_helpPage();
    void init_board(bool is_two_player = true);
    void init_settings();
//    QLineEdit * m_lineEdit;
    Server * m_server;

    QString m_theirSecretWord;
};

#endif // MAINSTACK_H
