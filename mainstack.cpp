#include "mainstack.h"
#include <QGridLayout>
#include <QPushButton>
#include <QStatusBar>
#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>
#include <QSvgWidget>
#include <QInputDialog>
#include "flowlayout.h"
#include "letterbutton.h"

MainStack::MainStack(QWidget *parent) :
    QStackedWidget(parent)
{
    m_server = new Server;
    QObject::connect(m_server, SIGNAL(connected()), this, SLOT(on_connected()));

    init_gui();

    m_dict = new Dictionary(this);
}

void MainStack::sendData()
{
//    qDebug() << Q_FUNC_INFO;
    QLineEdit * lineEdit = this->currentWidget()->findChild<QLineEdit *>();
    QStatusBar * bar = this->currentWidget()->findChild<QStatusBar *>();
//    bool is_two_player = true;
//    if(this->currentWidget()->objectName() == "One Player")
//        is_two_player = false;

    QString word = lineEdit->text().toLower().trimmed();
    int timeout = 3000;

    bool send = true;
    if(!word.startsWith(':'))
    {
        if(word.length() < m_dict->wordLength())
        {
            bar->showMessage("\"" + word + "\" is too short.", timeout);
            send = false;
        }
        else if(word.length() > m_dict->wordLength())
        {
            bar->showMessage("\"" + word + "\" is too long.", timeout);
            send = false;
        }
        else if(!m_dict->contains(word))
        {
            bar->showMessage("\"" + word + "\" was not found in the dictionary.", timeout);
            send = false;
        }
    }
    if(send)
    {
        m_server->writeData(word); // send it to the other one

        QString tempGuessedWord = word;
        int count = 0;
        for(int i = 0; i < m_theirSecretWord.length(); i++)
        {
            int letterIndex = tempGuessedWord.indexOf(m_theirSecretWord.at(i));
            if(letterIndex != -1)
            {
                tempGuessedWord.remove(letterIndex, 1);
                count++;
            }
        }
        emit data(QString::number(count) + " " +
                  word); // post it on the "yours" text edit

        if(word == m_theirSecretWord)
        {
            // Game Over, you win!
            emit data("Correct: 5 " + word);
        }
    }
    lineEdit->clear();
}

void MainStack::on_connectToGame()
{
    m_server->startTcpServer();
    m_server->broadcastUdp();
}

void MainStack::on_createGame()
{
    m_server->listenForUdpBroadcast();
}

void MainStack::on_connected()
{
    QString input;
    do
    {
    input = QInputDialog::getText(0,"Jotto - Set Secret Word",
                          "Please enter a 5 letter word.");
    }while(input.length() != 5 || !m_dict->contains(input));

    qDebug() << "Secret Word:" << input;

    // TODO: send the secret word over to the other player's program

    this->setCurrentIndex(2);// two player board
}

void MainStack::on_onePlayer()
{
    // Pick a random word from the dictionary based on difficulty
    m_theirSecretWord = m_dict->getNewSecretWord(16);

    this->setCurrentIndex(1);// one player board

    emit data("Random secret word picked.");
}

void MainStack::init_gui()
{

    QSvgWidget * svg;
    QWidget * w;
    QGridLayout * grid;
    QPushButton * btn;
//    QTextEdit * txt;
//    QLineEdit * lineEdit;
//    QLabel * label;
    QStatusBar * bar;
//    Utils::FlowLayout * flow;

    w = new QWidget;
    grid = new QGridLayout;

    svg = new QSvgWidget("://jotto-logo.svg");
    grid->addWidget(svg);

    btn = new QPushButton("Create Game");
    QObject::connect(btn, SIGNAL(clicked()), this, SLOT(on_createGame()));
    grid->addWidget(btn);

    btn = new QPushButton("Join Game");
    QObject::connect(btn, SIGNAL(clicked()), this, SLOT(on_connectToGame()));
    grid->addWidget(btn);

    btn = new QPushButton("Quick Game");
    QObject::connect(btn, SIGNAL(clicked()), this, SLOT(on_onePlayer()));
    grid->addWidget(btn);

    bar = new QStatusBar;
    grid->addWidget(bar);
    QObject::connect(m_server, SIGNAL(msg(QString)),bar, SLOT(showMessage(QString)));

    w->setLayout(grid);
    this->addWidget(w);

    // add widget for one player
    init_board(false);
    // add widget for two player
    init_board(true);

    // add widget for
}

void MainStack::init_board(bool is_two_player)
{
//    QSvgWidget * svg;
    QWidget * w;
    QGridLayout * grid;
//    QPushButton * btn;
    QTextEdit * txt;
//    QLineEdit * lineEdit;
    QLabel * label;
    QStatusBar * bar;
    Utils::FlowLayout * flow;

    int row = 0;
    w = new QWidget;
    grid = new QGridLayout;

    if(is_two_player)
        w->setObjectName("Two Player");
    else
        w->setObjectName("One Player");

    if(is_two_player)
    {
        label = new QLabel("Yours");
        grid->addWidget(label,row,0, Qt::AlignRight);
        label = new QLabel("Theirs");
        grid->addWidget(label,row++,1, Qt::AlignLeft);
    }

    txt = new QTextEdit;
    txt->setReadOnly(true);

    if(is_two_player)
    {
        txt->setMaximumWidth(120);
        grid->addWidget(txt,row,0,Qt::AlignRight);
    }
    else
    {
        grid->addWidget(txt,row++,0,1,2, Qt::AlignCenter);
    }
    QObject::connect(this, SIGNAL(data(QString)), txt, SLOT(append(QString)));

    if(is_two_player)
    {
        txt = new QTextEdit;
        txt->setReadOnly(true);
        txt->setMaximumWidth(120);
        grid->addWidget(txt,row++,1, Qt::AlignLeft);
        QObject::connect(m_server, SIGNAL(data(QString)), txt, SLOT(append(QString)));
    }

    // Draw the alphabet
    flow = new Utils::FlowLayout;
    for(int i = 0; i< 26; i++)
    {
        LetterButton * lb = new LetterButton('A' + i);
        QObject::connect(this, SIGNAL(setTheme(QColor, QColor, int)), lb, SLOT(setTheme(QColor, QColor, int)));
        QObject::connect(this, SIGNAL(resetLetters()), lb, SLOT(on_reset()));
        flow->addWidget(lb);
    }

    grid->addLayout(flow,row++,0,1,2, Qt::AlignCenter);

    flow = new Utils::FlowLayout;

    label = new QLabel("Your Guess");
    flow->addWidget(label);

    QLineEdit * lineEdit = new QLineEdit;
    QObject::connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(sendData()));
    flow->addWidget(lineEdit);

    grid->addLayout(flow,row++,0,1,2, Qt::AlignCenter);

    bar = new QStatusBar;
    grid->addWidget(bar,row++,0,1,2);
    QObject::connect(m_server, SIGNAL(msg(QString)),bar, SLOT(showMessage(QString)));

    w->setLayout(grid);
    this->addWidget(w);
}
