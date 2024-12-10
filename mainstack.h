#ifndef MAINSTACK_H
#define MAINSTACK_H

#include <QtWidgets/QStackedWidget>
#include "mytime.h"
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
#include <QElapsedTimer>
#include <QTime>
#include <QScreen>
#include <QHideEvent>
#include <QGestureEvent>
#include <QPinchGesture>
#include <QWindow>
#include <QExposeEvent>
#include <QScreen>
#include <QApplication>
#include <QInputMethod>
#include <QMediaPlayer>

class LineEdit : public QLineEdit
{
    Q_OBJECT
public:
    LineEdit(QWidget * parent = 0): QLineEdit(parent)
    {
    }
public slots:
    void keyPressEvent(QKeyEvent * ke)
    {
//        qDebug() << "Press:" << QString::number(ke->key(),16);
        QLineEdit::keyPressEvent(ke);
    }
    void keyReleaseEvent(QKeyEvent * ke)
    {
//        qDebug() << "     :" << QString::number(ke->key(),16);
        QLineEdit::keyReleaseEvent(ke);
    }
    void on_returnPressed()
    {
        qDebug() << "Return pressed!";
    }

    void focusInEvent(QFocusEvent* fe)
    {
        QLineEdit::focusInEvent(fe);
    }

    void focusOutEvent(QFocusEvent * fe)
    {
        QLineEdit::focusOutEvent(fe);
#ifdef Q_OS_IOS
        if(fe->reason() == Qt::OtherFocusReason)
        {
            // Done was pressed!
            emit returnPressed();
        }
#endif
    }

private:
    QElapsedTimer * m_enterPressedTimer;
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
    void updateSize(qreal factor, qreal factor_2);
public slots:
    void keyPressEvent(QKeyEvent* ke);
    void keyReleaseEvent(QKeyEvent* ke);
    void on_endOfPageAnimation();
    void dumpCurrentWordLists();
    void on_updateSize(qreal);
    void updateStats();
    void on_appendToTxtEdit(QString);
    void showEvent(QShowEvent *);
    void myAdjustSize();
    void on_lineEdit_editingFinished();
    void on_lineEdit_returnPressed();
    void on_finishedLoading();
    void on_randomGuess();
    void on_sliderChanged();
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
    void on_newGame();
    void on_settingsButton();
    void on_giveUpButton();
    void on_MainMenu();
    void on_confirmedGiveUp();
    void resetBoard();
    void on_shuffle();
    // line edit with enter pressed
    void sendData();
    void on_helpButton();
    void on_backButton();
    void readSettings();
    void writeSettings();
    void closeEvent(QCloseEvent *);
    void hideEvent(QHideEvent *);
    bool event(QEvent *e);
    bool gestureEvent(QGestureEvent *);
    void pinchTriggered(QPinchGesture *gesture);
//    void exposeEvent(QExposeEvent *);
    void on_destroyed();

    void on_visibilityChanged(QWindow::Visibility);
    void on_activeChanged();
    void on_windowStateChanged(Qt::WindowState);
    void on_screenChanged(QScreen*);

    void on_appStateChanged(Qt::ApplicationState);

private:
    QWindow * m_window;
    QWidget * m_prevPage;
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
    QCheckBox * m_showStatsDuringGame;
    QCheckBox * m_preventDuplicateGuesses;
    QComboBox * m_letterButtonScaleFactorCombo;
    QCheckBox * m_soundEffects;

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
    MyTime m_stopWatch;

    int m_dpiFactor;

    qreal m_stat_guessRate;
    int m_stat_numOfGuesses;
    QString m_stat_timeText;
    int m_random_count;
    qreal scaleFactor;
    qreal currentStepScaleFactor;
    qreal rotationAngle;

    QTimer * m_hideInputMethodTimer;
    QTimer * m_returnPressedTimer;

    WinBox *m_winBox;
};

#endif // MAINSTACK_H
