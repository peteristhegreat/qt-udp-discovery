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
#include <QCheckBox>
#include <QComboBox>
#include <QSettings>
#include "highlighter.h"

MainStack::MainStack(QWidget *parent) :
    QStackedWidget(parent)
{
    m_server = new Server;
    QObject::connect(m_server, SIGNAL(connected()), this, SLOT(on_connected()));

    m_helpPage = 0;
    m_onePlayerBoard = 0;
    m_twoPlayerBoard = 0;
    m_settingsPage = 0;
    m_loadingScreen = 0;
    m_mainMenu = 0;

    m_dict = new Dictionary(this);

    init_gui();
    readSettings();

}

void MainStack::closeEvent(QCloseEvent *)
{
    writeSettings();
}

void MainStack::readSettings()
{
    QSettings s;
    this->restoreGeometry(s.value("geometry").toByteArray());
    m_dict->setWordLength(s.value("num_letters", 5).toInt());
    m_numLettersCombo->setCurrentIndex(m_numLettersCombo->findText(QString::number(m_dict->wordLength())));
}

void MainStack::writeSettings()
{
    QSettings s;
    s.setValue("geometry", this->saveGeometry());
    s.setValue("num_letters", m_dict->wordLength());
}

void MainStack::on_data(QString str)
{
    // string arriving from the other player

    if (str.startsWith("*"))
    {
        m_theirSecretWord = str.mid(1);
    }
    else
    {
        emit appendToTheirs(str);
    }
}

void MainStack::sendData()
{
    // enter has been pressed on the lineedit...

    QLineEdit * lineEdit = this->currentWidget()->findChild<QLineEdit *>();
    QStatusBar * bar = this->currentWidget()->findChild<QStatusBar *>();

    QString word = lineEdit->text().toLower().trimmed();
    int timeout = 3000;

    bool send = false;
    if(m_theirSecretWord.length() == 0)
    {
        bar->showMessage("Waiting for opponent's secret word.");
    }
    else if(word.startsWith(':'))
    {
        // word starts with a ':', its a comment, send it over
        // without validation
        m_server->writeData(word);
        emit appendToYours(word);
    }
    else
    {
        // validation of input
        if(m_correctLength->isChecked()
                && word.length() < m_dict->wordLength())
        {
            bar->showMessage("\"" + word + "\" is too short.", timeout);
        }
        else if(m_correctLength->isChecked()
                && word.length() > m_dict->wordLength())
        {
            bar->showMessage("\"" + word + "\" is too long.", timeout);
        }
        else if(m_inDictionary->isChecked()
                && !m_dict->contains(word))
        {
            bar->showMessage("\"" + word + "\" was not found in the dictionary.", timeout);
        }
        else
        {
            send = true;
        }
    }

    if(send)
    {
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

        QString infoString = QString::number(count) + " " +
                word;
        emit appendToYours(infoString); // post it on the "yours" text edit

        m_server->writeData(infoString); // send it to the other one

        if(word == m_theirSecretWord)
        {
            // Game Over, you win!
            emit appendToYours("Correct: 5 " + word);
            m_server->writeData("The other player guessed your word!");
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
    // Jump into two player mode!

    QString input;
    do
    {
        input = QInputDialog::getText(this,
                                      "Jotto - Set Secret Word",
                                      "Please enter a 5 letter word.");
    } while(input.length() != 5 || !m_dict->contains(input));

    qDebug() << "Secret Word:" << input;
    m_server->writeData("*" + input);

    // TODO: send the secret word over to the other player's program

    this->setCurrentWidget(m_twoPlayerBoard);// two player board
}

void MainStack::on_onePlayer()
{
    // Pick a random word from the dictionary based on difficulty
    m_theirSecretWord = m_dict->getNewSecretWord(16);

    this->setCurrentWidget(m_onePlayerBoard);// one player board

    emit appendToYours("Random secret word picked.");
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

    btn = new QPushButton("Help");
    QObject::connect(btn, SIGNAL(clicked()), this, SLOT(on_helpButton()));
    grid->addWidget(btn);

    btn = new QPushButton("Settings");
    QObject::connect(btn, SIGNAL(clicked()), this, SLOT(on_settingsButton()));
    grid->addWidget(btn);

    bar = new QStatusBar;
    grid->addWidget(bar);
    QObject::connect(m_server, SIGNAL(msg(QString)),bar, SLOT(showMessage(QString)));

    w->setLayout(grid);
    this->addWidget(w);// 0
    m_mainMenu = w;

    // add widget for one player
    init_board(false);// 1
    // add widget for two player
    init_board(true);// 2

    // add widget for help
    init_helpPage(); // 3

    init_settings();
}

void MainStack::on_settingsButton()
{
    this->setCurrentWidget(m_settingsPage);
}

void MainStack::init_settings()
{
    QWidget * w;
    QGridLayout * grid;
    w = new QWidget;
    grid = new QGridLayout;


    QPushButton * btn;
    btn = new QPushButton("Back");
    QObject::connect(btn, SIGNAL(clicked()), this, SLOT(on_backButton()));
    grid->addWidget(btn);

//    QCheckBox * checkbox;
    grid->addWidget(new QLabel("Jotto Settings"));

    m_inDictionary = new QCheckBox("Guesses must be in dictionary.");
    m_inDictionary->setChecked(true);

    grid->addWidget(m_inDictionary);

    m_correctLength = new QCheckBox("Guesses must be the same length.");
    m_correctLength->setChecked(true);

    grid->addWidget(m_correctLength);

//    QComboBox * combo;

    m_numLettersCombo = new QComboBox;
    foreach(int i, m_dict->getWordLengths())
    {
        m_numLettersCombo->addItem(QString::number(i));
    }

    m_numLettersCombo->setCurrentIndex(m_numLettersCombo->findText("5"));


    QHBoxLayout * hbox = new QHBoxLayout;
    hbox->addWidget(new QLabel("# Letters"));
            hbox->addWidget(m_numLettersCombo);
    grid->addLayout(hbox, grid->rowCount(), 0);

    QObject::connect(m_numLettersCombo, SIGNAL(currentIndexChanged(QString)),
                     m_dict, SLOT(setWordLength(QString)));
    if(false)
    {
        QVBoxLayout * vbox = new QVBoxLayout;
        vbox->addStretch();
        grid->addLayout(vbox,grid->rowCount(), 0);
    }
    else
    {
        grid->addWidget(new QWidget());
    }
    w->setLayout(grid);
    this->addWidget(w);

    m_settingsPage = w;
}

void MainStack::on_backButton()
{
    qDebug() << Q_FUNC_INFO;
    this->setCurrentWidget(m_mainMenu);
}

void MainStack::init_helpPage()
{
    QWidget * w;
    QGridLayout * grid;
    w = new QWidget;
    grid = new QGridLayout;

    QPushButton * btn;
    btn = new QPushButton("Back");
    QObject::connect(btn, SIGNAL(clicked()), this, SLOT(on_backButton()));
    grid->addWidget(btn);

    QString helpText =
            "Welcome to Jotto!\n\n"
            "You play by trying to guess the secret word. "
            "If you are playing a five letter word game, "
            "the secret word is 5 letters long and only "
            "only five letter words can be guessed.";
    QLabel * label = new QLabel(helpText);
    label->setWordWrap(true);
    label->setFixedWidth(250);
    grid->addWidget(label);

    w->setLayout(grid);
    this->addWidget(w);
    m_helpPage = w;
}

void MainStack::on_helpButton()
{
   qDebug() << Q_FUNC_INFO;
   this->setCurrentWidget(m_helpPage);
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

    Highlighter * highlighter;

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
    highlighter = new Highlighter(txt->document());
//    txt->setReadOnly(true);

//    txt->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    if(is_two_player)
    {
        txt->setMaximumWidth(120);
        grid->addWidget(txt,row,0,Qt::AlignRight);
    }
    else
    {
        grid->addWidget(txt,row++,0,1,2, Qt::AlignHCenter);
    }
    grid->setRowStretch(grid->rowCount() - 1, 4);
//    QObject::connect(this, SIGNAL(data(QString)), this, SLOT(on_data(QString)));
    QObject::connect(this, SIGNAL(appendToYours(QString)), txt, SLOT(append(QString)));

    if(is_two_player)
    {
        txt = new QTextEdit;
//        txt->setReadOnly(true);
        txt->setMaximumWidth(120);
        grid->addWidget(txt,row++,1, Qt::AlignLeft);
        QObject::connect(m_server, SIGNAL(data(QString)), this, SLOT(on_data(QString)));
        QObject::connect(this, SIGNAL(appendToTheirs(QString)), txt, SLOT(append(QString)));
    }

    // Draw the alphabet
    flow = new Utils::FlowLayout;
    for(int i = 0; i< 26; i++)
    {
        LetterButton * lb = new LetterButton('A' + i);
        QObject::connect(this, SIGNAL(setTheme(QColor, QColor, int)), lb, SLOT(setTheme(QColor, QColor, int)));
        QObject::connect(this, SIGNAL(resetLetters()), lb, SLOT(on_reset()));
        highlighter->connectToLetterButton(lb);
        flow->addWidget(lb);
    }
    QObject::connect(this, SIGNAL(setTheme(QColor, QColor, int)), highlighter, SLOT(setTheme(QColor, QColor, int)));

    emit setTheme(Qt::black, Qt::white, 0);
    emit setTheme(Qt::white, Qt::black, 2);
    emit setTheme(Qt::black, Qt::lightGray, 4);
    emit setTheme(Qt::white, Qt::green, 6);
    emit resetLetters();

    grid->addLayout(flow,row++,0,1,2, Qt::AlignCenter);

    bar = new QStatusBar;
    grid->addWidget(bar,row++,0,1,2);
    QObject::connect(m_server, SIGNAL(msg(QString)),bar, SLOT(showMessage(QString)));

    flow = new Utils::FlowLayout;

    label = new QLabel("Your Guess");
    flow->addWidget(label);

    QLineEdit * lineEdit = new QLineEdit;
    QObject::connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(sendData()));
    flow->addWidget(lineEdit);

    grid->addLayout(flow,row++,0,1,2, Qt::AlignCenter);


    w->setLayout(grid);
    this->addWidget(w);
    if(is_two_player)
        m_twoPlayerBoard = w;
    else
        m_onePlayerBoard = w;

}
