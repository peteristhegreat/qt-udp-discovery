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
    QHBoxLayout *buttons = new QHBoxLayout;

    labels->addWidget(new QLabel("You're a Winner!"));

    m_timeLabel = new QLabel("null");
    labels->addWidget(m_timeLabel);  //add time to solve in QLabel here

    m_numberOfGuessesLabel = new QLabel("null");
    labels->addWidget(m_numberOfGuessesLabel); //add number of guesses in QLabel here

    QObject::connect(m_newGame,SIGNAL(clicked(bool)),this,SIGNAL(newGame(void)));
    QObject::connect(m_newGame,SIGNAL(clicked(bool)),this,SLOT(hide()));

    QObject::connect(m_exit,SIGNAL(clicked(bool)),this,SIGNAL(exit(void)));
    QObject::connect(m_exit,SIGNAL(clicked(bool)),this,SLOT(hide()));




    buttons->addWidget(m_exit);
    buttons->addWidget(m_newGame);

    layout->addLayout(labels);
    layout->addLayout(buttons);
    setLayout(layout);

    setStyleSheet("background-color:white;");




    setFocusPolicy(Qt::StrongFocus);

}

void WinBox::setTime(QString time)
{
    m_time = time;

    m_timeLabel->setText("Time to finish: " + time);
}

void WinBox::setNumberOfGuesses(int num)
{
    m_numberOfGuessesLabel->setText("Number of Guesses: " + QString::number(num));
}


