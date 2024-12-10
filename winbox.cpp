#include "winbox.h"
#include "mainstack.h"

WinBox::WinBox(QWidget *parent) : QWidget(parent)
{
    setObjectName("win_box");

    mapToParent(QPoint(0,0));
    m_exit = new QPushButton("Exit");
    m_newGame = new QPushButton("New Game");
    m_newGame->setObjectName("new game");

    QVBoxLayout *layout = new QVBoxLayout;
    QVBoxLayout *labels = new QVBoxLayout;
    QHBoxLayout *hb;
    QHBoxLayout *buttons = new QHBoxLayout;

    labels->addWidget(new QLabel("You're a Winner!"));

    m_timeLabel = new QLabel("null");
    hb = new QHBoxLayout();
    hb->addWidget(new QLabel("Time to Finish"));
    hb->addStretch();
    hb->addWidget(m_timeLabel);
    labels->addLayout(hb);  //add time to solve in QLabel here

    m_numberOfGuessesLabel = new QLabel("null");
    hb = new QHBoxLayout();
    hb->addWidget(new QLabel("Number of Guesses"));
    hb->addStretch();
    hb->addWidget(m_numberOfGuessesLabel);
    labels->addLayout(hb);

    QObject::connect(m_newGame,SIGNAL(clicked(bool)),this,SIGNAL(newGame(void)));
    QObject::connect(m_newGame,SIGNAL(clicked(bool)),this,SLOT(hide()));

    QObject::connect(m_exit,SIGNAL(clicked(bool)),this,SIGNAL(exit(void)));
    QObject::connect(m_exit,SIGNAL(clicked(bool)),this,SLOT(hide()));


//    QPalette p = this->palette();
//    p.setColor(QPalette::Background, Qt::black);
//    this->setAutoFillBackground(true);
//    this->setPalette(p);
//    qDebug() << "geometry of winbox" << this->geometry();

    buttons->addWidget(m_exit);
    buttons->addWidget(m_newGame);

    layout->addLayout(labels);
    layout->addLayout(buttons);

    this->setAttribute(Qt::WA_TranslucentBackground, false);
    QWidget * w = new QWidget(this);
    QHBoxLayout * box = new QHBoxLayout(this);
    w->setLayout(layout);
    box->addWidget(w);
    setLayout(box);

    setStyleSheet("background-color:white;");




    setFocusPolicy(Qt::StrongFocus);

}

void WinBox::setTime(QString time)
{
    m_time = time;

    m_timeLabel->setText(time);
}

void WinBox::setNumberOfGuesses(int num)
{
    m_numberOfGuessesLabel->setText(QString::number(num));
}


