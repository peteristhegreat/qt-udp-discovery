#include "mainstack.h"
#include <QGridLayout>
#include <QPushButton>
#include <QStatusBar>
#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>
#include "aspectratiosvgwidget.h"
#include <QInputDialog>
#include "flowlayout.h"
#include "letterbutton.h"
#include <QCheckBox>
#include <QComboBox>
#include <QSettings>
#include "highlighter.h"
#include <QMessageBox>
#include "shuffle.h"
#include <QDir>
#include <QApplication>
#include <QShortcut>
#include <QKeySequence>
#include <QGroupBox>
#include <QSlider>
#include <QScroller>
#include "globals.h"
#include <QProgressDialog>


MainStack::MainStack(QWidget *parent) :
    SlidingStackedWidget(parent)
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

    m_overlay = new Overlay(this);
    m_overlay->hide();
    QObject::connect(m_overlay, SIGNAL(finished()), this, SLOT(on_endOfVictoryDance()));

    m_additionalStyleSheet = "";
#ifdef Q_OS_IOS
        m_additionalStyleSheet =
                "QDialog {"
                "background: qlineargradient( x1:0.2 y1:0.4, x2:1 y2:0.5, stop:0 deepskyblue, stop:1 darkslateblue);"

//                "background: white;"
                "}"
                "QDialog QLabel {"
                ""// top right bottom left
                "padding: 100px 20px 20px 20px;"
                "font-size: 36px;"
                "}"

                "QDialog QPushButton {"
                ""
                "font-size: 24px;"
                "}"


                "QMessageBox QLabel {"
                ""// top right bottom left
                "padding: 100px 20px 20px 20px;"
                "font-size: 36px;"
                "}"

                "QMessageBox QPushButton {"
                ""
                "font-size: 24px;"
                "}"
                ;
#else
#ifdef Q_OS_ANDROID
    m_additionalStyleSheet =
            "QDialog {"
            "background: qlineargradient( x1:0.2 y1:0.4, x2:1 y2:0.5, stop:0 deepskyblue, stop:1 darkslateblue);"

//                "background: white;"
            "}"
            "QDialog QLabel {"
            ""// top right bottom left
            "padding: 100px 20px 20px 20px;"
            "font-size: 36px;"
            "}"

            "QDialog QPushButton {"
            ""
            "font-size: 24px;"
            "}"


            "QMessageBox QLabel {"
            ""// top right bottom left
            "padding: 100px 20px 20px 20px;"
            "font-size: 36px;"
            "}"

            "QMessageBox QPushButton {"
            ""
            "font-size: 24px;"
            "}"
            ;

#else
        resize(650, 800);
#endif
#endif

    readSettings();
    //this->currentWidget()->findChild<QStatusBar*>()->showMessage(QDir::currentPath());

    QShortcut * shortcut;
    shortcut = new QShortcut(QKeySequence("F5"),this,SLOT(on_refreshStyleSheet()));
    on_refreshStyleSheet();
    
    // start a delayed init
    QTimer * t = new QTimer;
    t->setSingleShot(true);
    QObject::connect(t, SIGNAL(timeout()), m_dict, SLOT(init()));
    t->start(500);
    
//    QProgressDialog * prog = new QProgressDialog();
//    prog->setValue(20);
//    this->addWidget(prog);
//    this->setCurrentWidget(prog);
    
    QObject::connect(m_dict, SIGNAL(ready()), this, SLOT(on_finishedLoading()));
    
    m_currWidget = m_mainMenu;
    
    QStatusBar * bar = this->currentWidget()->findChild<QStatusBar*>();
    bar->setFixedHeight(30);
    bar->setMinimumWidth(200);
    bar->setStyleSheet("background:white;");
    bar->showMessage("Loading Dictionary");
    foreach(QPushButton * btn, this->currentWidget()->findChildren<QPushButton*>())
    {
        btn->setDisabled(true);
    }
}

void MainStack::on_finishedLoading()
{
    qDebug() << this->geometry();
    QStatusBar * bar = this->currentWidget()->findChild<QStatusBar*>();
    bar->showMessage("Ready to go!");
    foreach(QPushButton * btn, this->currentWidget()->findChildren<QPushButton*>())
    {
        btn->setDisabled(false);
    }

#if defined(Q_OS_IOS) || defined(Q_OS_ANDROID)
#ifdef Q_OS_IOS
if(this->width() == 320 || this->height() == 320)
#else
if(this->width() < 500 || this->height() < 500)
#endif
#else
    if(false)
#endif
    {
        // we are an iphone!

        this->setStyleSheet(this->styleSheet() +
                    "QFrame {padding: 3px;}"
                            "QPushButton {"
                            "    padding: 8;"
                            "    margin: 2;"
                            "    /* font-family: \"Times\"; */"
                            "    font-size: 12px;"
                            "    border-radius: 4px;"
                            "    border-style: inset;"
                            "    border-width: 2px;"
                            "    /*  min-width: 10em; */"
                            "    background: white;"
                            "}"


                    );

    }
    else
    {
        this->setStyleSheet(this->styleSheet() +
                            "QFrame {padding: 10px;}"
                    );
    }
    this->ensurePolished();
    //    QProgressDialog * prog = qobject_cast <QProgressDialog*> (this->currentWidget());
//    prog->setValue(90);
//    this->setCurrentWidget(0);
//    m_overlay->startAnimation();
//    qDebug() << Q_FUNC_INFO;
//    m_overlay->startAnimation();
//    m_overlay->show();
//    m_overlay->raise();
}

void MainStack::on_refreshStyleSheet()
{
//    QApplication app( argc, argv );

    // Load an application style

    QFile styleFile( "style.qss" );
    QFile styleFile2( "://style.qss" );


    if(styleFile.exists())
    {
        styleFile.open( QFile::ReadOnly );

        // Apply the loaded stylesheet
        QString style( styleFile.readAll() + m_additionalStyleSheet);
        qApp->setStyleSheet( style );

        this->ensurePolished();
    }
    else
    {
        styleFile2.open( QFile::ReadOnly );

        // Apply the loaded stylesheet
        QString style( styleFile2.readAll() + m_additionalStyleSheet);
        qApp->setStyleSheet( style );

        this->ensurePolished();
    }
}

void MainStack::on_endOfVictoryDance()
{
    QPushButton * btn = this->currentWidget()->findChild<QPushButton *>("Give Up");
    if(btn)
        btn->setEnabled(true);
    QStatusBar * bar = this->currentWidget()->findChild<QStatusBar *>();
    if(bar)
        bar->showMessage("You are super awesome!  Go team!");
    if(btn)
        btn->setFocus();
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
    m_ephHouseRules->setChecked(s.value("eph_house_rules", false).toBool());
    m_allowDoubleLetters->setChecked(s.value("allow_double_letters",true).toBool());
    m_autoMarkZeroLetterGuesses->setChecked(s.value("auto_mark_zero_letter_guesses", true).toBool());
//    m_musicEnabled->setValue(s.value("music_volume", 10).toBool());
//    m_soundsEnabled->setValue(s.value("sounds_volume", 40).toBool());
}

void MainStack::writeSettings()
{
    QSettings s;
    s.setValue("geometry", this->saveGeometry());
    s.setValue("num_letters", m_dict->wordLength());
    s.setValue("eph_house_rules", m_ephHouseRules->isChecked());
    s.setValue("allow_double_letters", m_allowDoubleLetters->isChecked());
    s.setValue("auto_mark_zero_letter_guesses", m_autoMarkZeroLetterGuesses->isChecked());
}

void MainStack::resizeEvent(QResizeEvent *event)
{
    m_overlay->resize(event->size());
    event->accept();
}

void MainStack::on_data(QString str)
{
    // string arriving from the other player

    if (str.startsWith("*"))
    {
        m_theirSecretWord = str.mid(1);
        m_dict->setWordLength(m_theirSecretWord.length());

        emit appendToTheirs("Received secret word: " + QString(m_theirSecretWord.length(), '*'));
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
                && word.length() < m_theirSecretWord.length())
        {
            bar->showMessage("\"" + word + "\" is too short.", timeout);
        }
        else if(m_correctLength->isChecked()
                && word.length() > m_theirSecretWord.length())
        {
            bar->showMessage("\"" + word + "\" is too long.", timeout);
        }
        else if(!m_allowDoubleLetters->isChecked() && Dictionary::hasDoubleLetters(word))
        {
            bar->showMessage("\"" + word + "\" has double letters.", timeout);
        }
        else if(m_inDictionary->isChecked()
                && !m_dict->contains(word, m_correctLength->isChecked()))
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
        if(m_ephHouseRules->isChecked())
        {
            // this is much less intuitive reporting, but it should be consistent
            tempGuessedWord = "";
            for(int i = 0; i < word.length(); i++)
            {
                if(!tempGuessedWord.contains(word.at(i)))
                {
                    tempGuessedWord += word.at(i);
                }
            }
            // now tempGuessedWord includes only unique letters

            for(int i = 0; i < tempGuessedWord.length(); i++)
            {
//                int index = 0;
                // for every instance of any of the guess letters in the word, add to the tally
                for(int j = 0; j < m_theirSecretWord.length(); j++)
                {
                    if(m_theirSecretWord.at(j) == tempGuessedWord.at(i))
                    {
                        count++;
                    }
                }
            }
        }
        else
        {
            for(int i = 0; i < m_theirSecretWord.length(); i++)
            {
                int letterIndex = tempGuessedWord.indexOf(m_theirSecretWord.at(i));
                if(letterIndex != -1)
                {
                    tempGuessedWord.remove(letterIndex, 1);
                    count++;
                }
            }
        }

        QString infoString = QString::number(count) + " " +
                word;
        emit appendToYours(infoString); // post it on the "yours" text edit

        m_server->writeData(infoString); // send it to the other one

        if(word == m_theirSecretWord)
        {
            // Game Over, you win!
            emit appendToYours("Correct: " + word);
            m_server->writeData("The other player guessed your word!");


            m_overlay->startAnimation();
            m_overlay->show();
            m_overlay->raise();


            QPushButton * btn = this->currentWidget()->findChild<QPushButton *>("Give Up");
            btn->setText("Done");
            btn->setDisabled(true);
            lineEdit->setDisabled(true);
        }
        else if(count == 0 && m_autoMarkZeroLetterGuesses->isChecked())
        {
            QList<LetterButton *> letterButtons = this->currentWidget()->findChildren<LetterButton *>();
            qDebug() << "automark"<< letterButtons.size() << word;
            foreach(LetterButton * lb, letterButtons)
            {
                if(word.contains(lb->getLetter().toLower()))
                {
                    while(lb->getState() != 2)
                    {
                        lb->on_clicked();
                    }
                }
            }
        }
        updateGuessCount();
    }
    lineEdit->clear();
}

void MainStack::updateGuessCount(bool reset)
{

    QLabel * label = this->currentWidget()->findChild<QLabel *> ("Guess Count");
    if(reset)
    {
        // reset the guess count down to 1
        label->setText(QString::number(1));
    }
    else
    {
        // increment the guess count
        label->setText(QString::number(label->text().toInt() + 1));
    }
}

void MainStack::on_connectToGame()
{
    if(m_server->isConnected())
    {
        on_connected();
        return;
    }

    m_server->startTcpServer();
    m_server->broadcastUdp();
}

void MainStack::on_createGame()
{
    if(m_server->isConnected())
    {
        on_connected();
        return;
    }

    m_server->listenForUdpBroadcast();
}

void MainStack::on_connected()
{
    // Jump into two player mode!

    QString input;
    bool ok;
    do
    {
        input = QInputDialog::getText(this,
              "Jotto - Set Secret Word",
              "Please enter a "
              + QString::number(m_dict->wordLength())
              + " letter word.", QLineEdit::Normal, QString(), &ok).toLower();


    } while(ok && m_server->isConnected()
            && (input.length() != m_dict->wordLength()
                || !m_dict->contains(input)
                || (!m_allowDoubleLetters->isChecked() && Dictionary::hasDoubleLetters(input))
                ));


//    QStatusBar * bar = this->currentWidget()->findChild<QStatusBar *>();
//    QString currentMessage = bar->currentMessage();
    if(!ok || !m_server->isConnected())
    {
        this->setCurrentWidget(m_mainMenu);
//        if(!m_server->isConnected())
//        {
//            QStatusBar * bar = this->currentWidget()->findChild<QStatusBar *>();
////            m_bar->
//        }
        return;
    }
    qDebug() << "Secret Word:" << input;
    m_server->writeData("*" + input);

    // TODO: send the secret word over to the other player's program

    this->setCurrentWidget(m_twoPlayerBoard);// two player board

    this->currentWidget()->findChild<QLineEdit*>()->setEnabled(true);
    this->currentWidget()->findChild<QLineEdit*>()->setFocus();
}

void MainStack::on_onePlayer()
{
    if(false)
    {
        m_overlay->startAnimation();
        m_overlay->show();
        m_overlay->raise();
    }

    // Pick a random word from the dictionary based on difficulty
    m_theirSecretWord = m_dict->getNewSecretWord(16, m_allowDoubleLetters->isChecked());

    this->setCurrentWidget(m_onePlayerBoard);// one player board

    emit appendToYours("Random secret word picked.");

    this->currentWidget()->findChild<QLineEdit*>()->setEnabled(true);
    this->currentWidget()->findChild<QLineEdit*>()->setFocus();
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
    grid->addWidget(btn,0,0,
                    Qt::AlignLeft);
//                    Qt::AlignHCenter);

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

    m_ephHouseRules = new QCheckBox("EPH House Rules for double/triple letter reporting");
    grid->addWidget(m_ephHouseRules);
    m_ephHouseRules->setChecked(false);

    m_allowDoubleLetters = new QCheckBox("Allow double letters");
    grid->addWidget(m_allowDoubleLetters);
    m_allowDoubleLetters->setChecked(true);


    m_autoMarkZeroLetterGuesses = new QCheckBox("Automark Zero Letter Guesses");
    grid->addWidget(m_autoMarkZeroLetterGuesses);
    m_autoMarkZeroLetterGuesses->setChecked(false);

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
    grid->addWidget(btn,0,0,
                    Qt::AlignLeft);
//                    Qt::AlignHCenter);

    QString helpText =
            "Welcome to Jotto!\n\n"
            "You play by trying to guess the secret word. "
            "If you are playing a five letter word game, "
            "the secret word is 5 letters long and "
            "only five letter words can be guessed.";
    QLabel * label = new QLabel(helpText);
    label->setWordWrap(true);
    label->setFixedWidth(300);
    grid->addWidget(label, grid->rowCount(), 0, Qt::AlignHCenter);
//    grid->add

    w->setLayout(grid);
    this->addWidget(w);
    m_helpPage = w;
}

void MainStack::on_helpButton()
{
   qDebug() << Q_FUNC_INFO;
   this->setCurrentWidget(m_helpPage);
}

void MainStack::on_giveUpButton()
{
    QPushButton * btn = this->currentWidget()->findChild<QPushButton *>("Give Up");

    int ret = QMessageBox::Yes;

    if(btn->text() == "Give Up")
    {
        QMessageBox msgBox;
        //    msgBox.setParent(this);
        msgBox.setText("You are so close."
                       "\n\n"
                       "Do you really want to give up?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        ret = msgBox.exec();
    }
    else
    {
        // they answered with a correct word
    }

    if(ret == QMessageBox::Yes)
    {
        if(btn->text() == "Give Up")
        {
            QMessageBox::information(this,"The Secret Word",
                                     "The secret word was:\n\n      "
                                     + m_theirSecretWord
                                     + "\n\nBetter luck next time.");
        }
        foreach(QTextEdit * t,
                this->currentWidget()->findChildren<QTextEdit *>())
        {
            t->clear();
        }

        emit resetLetters();

        updateGuessCount(true);

        m_overlay->hide();
        btn->setText("Give Up");
        QStatusBar * bar = this->currentWidget()->findChild<QStatusBar * >();
        bar->clearMessage();

        this->setCurrentWidget(m_mainMenu);
    }

}

void MainStack::on_shuffle()
{
    QString str;
    // get all the letters highlighted gray or green
    foreach(LetterButton * lb, this->currentWidget()->findChildren<LetterButton*>())
    {
        if(lb->getState() > 4)
            str += lb->getLetter();
    }
    QStatusBar * bar = this->currentWidget()->findChild<QStatusBar *> ();

    if(str.length() < 2)
    {
        bar->showMessage("Need 2+ letters green to shuffle.");
    }
    else
    {
        // scramble the letters
        QString shuffled = shuffle(str).toLower();
        bar->showMessage(shuffled);
    }
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

    QHBoxLayout * hbox;
    hbox = new QHBoxLayout;
    QPushButton * button;
    button = new QPushButton("Give Up");
    button->setObjectName("Give Up");
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(on_giveUpButton()));
    hbox->addWidget(button);
    hbox->addStretch();
    button = new QPushButton("Refresh");
    QObject::connect(button, SIGNAL(clicked()), this, SIGNAL(resetLetters()));
    hbox->addWidget(button);
    hbox->addSpacing(10);
    button = new QPushButton("Shuffle");
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(on_shuffle()));
    hbox->addWidget(button);

    button = new QPushButton("Random");
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(on_randomGuess()));
    hbox->addWidget(button);

    grid->addLayout(hbox, row++, 0,1,2);

    bar = new QStatusBar;
    grid->addWidget(bar,row++,0,1,2);
    grid->setRowStretch(row,0);

//    bar->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
    QObject::connect(m_server, SIGNAL(msg(QString)),bar, SLOT(showMessage(QString)));

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

//    int col = 0;

    hbox = new QHBoxLayout;

    txt = new QTextEdit;
    addKineticScrolling(txt);

    highlighter = new Highlighter(txt->document());
    txt->setReadOnly(true);

    QSlider * slider;
    slider = new QSlider(Qt::Vertical);
    slider->setRange(100, 360);
    slider->setValue(140);
    slider->setFixedWidth(30);
//    grid->addWidget(slider,row,col++,Qt::AlignLeft);
    hbox->addWidget(slider);
    hbox->addStretch();

    QObject::connect(slider, SIGNAL(valueChanged(int)), this, SLOT(on_sliderChanged(int)));


//    txt->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    if(is_two_player)
    {
        txt->setMaximumWidth(120);
//        grid->addWidget(txt,row,col++,Qt::AlignRight);
        hbox->addWidget(txt);
    }
    else
    {
        hbox->addWidget(txt);
//        grid->addWidget(txt,row++,col++,1,2, Qt::AlignHCenter);
    }
    grid->setRowStretch(grid->rowCount() - 1, 4);
//    QObject::connect(this, SIGNAL(data(QString)), this, SLOT(on_data(QString)));
    QObject::connect(this, SIGNAL(appendToYours(QString)), txt, SLOT(append(QString)));

    if(is_two_player)
    {
        txt = new QTextEdit;
        addKineticScrolling(txt);
        txt->setReadOnly(true);
        txt->setMaximumWidth(120);
//        grid->addWidget(txt,row++,col++, Qt::AlignLeft);
        hbox->addWidget(txt);
        QObject::connect(m_server, SIGNAL(data(QString)), this, SLOT(on_data(QString)));
        QObject::connect(this, SIGNAL(appendToTheirs(QString)), txt, SLOT(append(QString)));
    }

    hbox->addStretch();
    grid->addLayout(hbox,row++,0,1,grid->columnCount());
    grid->setRowStretch(row -1, 5);

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


//    flow = new Utils::FlowLayout;
//    QHBoxLayout * hbox;
    hbox = new QHBoxLayout;
    hbox->addStretch();

    label = new QLabel("Guess");
    hbox->addWidget(label);
    label->setMargin(0);

    label = new QLabel("1");
    hbox->addWidget(label);
    label->setObjectName("Guess Count");
    label->setMargin(0);

    QLineEdit * lineEdit = new QLineEdit;
    QObject::connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(sendData()));
    hbox->addWidget(lineEdit);

    button = new QPushButton("Go");
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(sendData()));
    hbox->addWidget(button);
    hbox->addStretch();

    
    
    grid->addLayout(hbox,row++,0,1,2, Qt::AlignCenter);

    w->setLayout(grid);
    this->addWidget(w);
    if(is_two_player)
        m_twoPlayerBoard = w;
    else
        m_onePlayerBoard = w;

}


void MainStack::addKineticScrolling(QWidget * w)
{
#ifdef Q_OS_IOS
        //qDebug() << "is iOS!!!";
        QScroller::grabGesture(w, QScroller::TouchGesture);
#else
        QScroller::grabGesture(w, QScroller::LeftMouseButtonGesture);
#endif
}

void MainStack::on_randomGuess()
{
    QLineEdit * lineEdit = this->currentWidget()->findChild<QLineEdit*>();
    QString guess = m_dict->getNewSecretWord(5, m_allowDoubleLetters->isChecked());
    lineEdit->setText(guess);
    sendData();
}

void MainStack::on_sliderChanged(int size)
{
    Highlighter * h = this->currentWidget()->findChild<Highlighter *>();
    QList <QTextEdit*> txts = this->currentWidget()->findChildren<QTextEdit*>();
//    qDebug() << Q_FUNC_INFO << txts.size() << size;
    for(int i = 0; i< txts.size(); i++)
    {
        txts[i]->setFontPointSize((qreal)size/10.0);
    }
    if(h)
    {
        h->setFontSize((qreal)size/10.0);
    }
}

void MainStack::setCurrentWidget(QWidget * w)
{
    this->slideInIdx(this->indexOf(w));
    m_currWidget = w;
}

QWidget * MainStack::currentWidget()
{
    return m_currWidget;
}
