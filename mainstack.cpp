#include "mainstack.h"
#include <QGridLayout>
#include <QPushButton>
#include <QStatusBar>
#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>
#include <QSvgWidget>

MainStack::MainStack(QWidget *parent) :
    QStackedWidget(parent)
{
    m_server = new Server;
    QObject::connect(m_server, SIGNAL(connected()), this, SLOT(on_connected()));

    init_gui();
}

void MainStack::sendData()
{
    QString word = m_lineEdit->text();
    m_server->writeData(word); // send it to the other one
    emit data(word); // post it on the "yours" text edit
    m_lineEdit->clear();
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
    this->setCurrentIndex(1);
}

void MainStack::init_gui()
{

    QSvgWidget * svg;
    QWidget * w;
    QGridLayout * grid;
    QPushButton * btn;
    QTextEdit * txt;
//    QLineEdit * lineEdit;
    QLabel * label;
    QStatusBar * bar;

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

    bar = new QStatusBar;
    grid->addWidget(bar);
    QObject::connect(m_server, SIGNAL(msg(QString)),bar, SLOT(showMessage(QString)));

    w->setLayout(grid);
    this->addWidget(w);

    w = new QWidget;
    grid = new QGridLayout;

    label = new QLabel("Yours");
    grid->addWidget(label,0,0);

    txt = new QTextEdit;
    txt->setReadOnly(true);
    grid->addWidget(txt,1,0);
    QObject::connect(this, SIGNAL(data(QString)), txt, SLOT(append(QString)));

    label = new QLabel("Theirs");
    grid->addWidget(label,0,1);

    txt = new QTextEdit;
    txt->setReadOnly(true);
    grid->addWidget(txt,1,1);
    QObject::connect(m_server, SIGNAL(data(QString)), txt, SLOT(append(QString)));

    label = new QLabel("Your Guess");
    grid->addWidget(label);

    m_lineEdit = new QLineEdit;
    QObject::connect(m_lineEdit, SIGNAL(returnPressed()), this, SLOT(sendData()));
    grid->addWidget(m_lineEdit);

    bar = new QStatusBar;
    grid->addWidget(bar,3,0,1,2);
    QObject::connect(m_server, SIGNAL(msg(QString)),bar, SLOT(showMessage(QString)));

    w->setLayout(grid);
    this->addWidget(w);
}
