#ifndef MAINSTACK_H
#define MAINSTACK_H

#include <QtWidgets/QStackedWidget>
#include "server.h"
#include <QLineEdit>
#include "dictionary.h"
#include "overlay.h"
#include <QStatusBar>
#include <QCheckBox>
#include <QCloseEvent>
#include <QShowEvent>
#include <QComboBox>
#include <QResizeEvent>
#include <QLabel>
#include "slidingstackedwidget.h"
#include <QKeyEvent>
#include <QFocusEvent>
#include <QTimer>
#include <QTime>

class LineEdit : public QLineEdit
{
    Q_OBJECT
public:
    LineEdit(QWidget * parent = 0): QLineEdit(parent){}
public slots:
    void keyPressEvent(QKeyEvent * ke)
    {
//        qDebug() << "Press:" << ke->key();
        QLineEdit::keyPressEvent(ke);
    }
    void keyReleaseEvent(QKeyEvent * ke)
    {
//        qDebug() << "     :" << ke->key();
        QLineEdit::keyReleaseEvent(ke);
    }
    void focusInEvent(QFocusEvent* fe)
    {
//        qDebug() << "in" << fe->reason();
        QLineEdit::focusInEvent(fe);
    }

    void focusOutEvent(QFocusEvent * fe)
    {
//        qDebug() << "out" << fe->reason();
        QLineEdit::focusOutEvent(fe);
#ifdef Q_OS_IOS
        if(fe->reason() == Qt::OtherFocusReason)
        {
            // Done was pressed!
            emit returnPressed();
        }
#endif
    }
//    void focusNextChild()
};

class MainStack : public SlidingStackedWidget
{
    Q_OBJECT
public:
    explicit MainStack(QWidget *parent = 0);
    ~MainStack(){}
    void updateGuessCount(bool reset = false);
    QWidget * currentWidget();
signals:
    void setTheme(QColor, QColor, int);
    void resetLetters();
    void appendToYours(QString);
    void appendToTheirs(QString);
public slots:
    void updateStats();
    void on_appendToTxtEdit(QString);
    void showEvent(QShowEvent *);
    void myAdjustSize();
    void on_lineEdit_editingFinished();
    void on_lineEdit_returnPressed();
    void on_finishedLoading();
    void on_randomGuess();
    void on_sliderChanged(int);
    void setCurrentWidget(QWidget * w);
    void on_refreshStyleSheet();
    void on_endOfVictoryDance();
    void resizeEvent(QResizeEvent*);
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
    QString m_additionalStyleSheet;

    void addKineticScrolling(QWidget * w);
    QWidget * m_currWidget;
    Overlay * m_overlay;
    QWidget * m_helpPage;
    QWidget * m_onePlayerBoard;
    QWidget * m_twoPlayerBoard;
    QWidget * m_mainMenu;
    QWidget * m_settingsPage;
    QWidget * m_loadingScreen;

    QCheckBox * m_inDictionary;
    QCheckBox * m_correctLength;
    QCheckBox * m_ephHouseRules;
    QCheckBox * m_allowDoubleLetters;
    QComboBox * m_numLettersCombo;
    QCheckBox * m_autoMarkZeroLetterGuesses;

    QStatusBar * m_bar;
    Dictionary * m_dict;
    void init_gui();
    void init_helpPage();
    void init_board(bool is_two_player = true);
    void init_settings();
//    QLineEdit * m_lineEdit;
    Server * m_server;

    QString m_theirSecretWord;

    QTimer * m_statsTimer;
    QTime m_stopWatch;
};

#endif // MAINSTACK_H
