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
#include <QScrollBar>
#include <QLayout>
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QStackedLayout>
#include <QGuiApplication>
#include <QScreen>
#include <QDebug>
#include <QScrollArea>
#include <QGestureEvent>
#include <QMediaPlayer>

// Helper function to return display orientation as a string.
QString Orientation(Qt::ScreenOrientation orientation)
{
    switch (orientation) {
    case Qt::PrimaryOrientation           : return "Primary";
    case Qt::LandscapeOrientation         : return "Landscape";
    case Qt::PortraitOrientation          : return "Portrait";
    case Qt::InvertedLandscapeOrientation : return "Inverted landscape";
    case Qt::InvertedPortraitOrientation  : return "Inverted portrait";
    default                               : return "Unknown";
    }
}

MainStack::MainStack(QWidget *parent) :
    SlidingStackedWidget(parent)
{


    m_hideInputMethodTimer = new QTimer(0);
    m_hideInputMethodTimer->setInterval(300);
    m_hideInputMethodTimer->setSingleShot(true);
    QObject::connect(m_hideInputMethodTimer, SIGNAL(timeout()), qApp->inputMethod(), SLOT(hide()));

    m_returnPressedTimer = new QTimer(0);
    m_returnPressedTimer->setInterval(100);
    m_returnPressedTimer->setSingleShot(true);
    QObject::connect(m_returnPressedTimer, SIGNAL(timeout()), this, SLOT(sendData()));
    //    QObject::connect(m_returnPressedTimer, SIGNAL(timeout()), this, SLOT(on_lineEdit_editingFinished()));

    m_dpiFactor = 1;
    qDebug() << "DPI - Logical:" << this->logicalDpiX() << this->logicalDpiY()
             << "Physical:" << this->physicalDpiX() << this->physicalDpiY()
             << "MM:" << this->widthMM() << this->heightMM();

    //    QSettings s;
    //    s.setValue("text_edit_font_size", 26);// 100 to 360

    QObject::connect(this, SIGNAL(animationFinished()), this, SLOT(on_endOfPageAnimation()));

    QString computerType;
    switch(this->logicalDpiX())
    {
    case 72:
    case 96:
        computerType = "Desktop computer!";
        break;
    case 220:
    case 227:
    case 264:
    case 326:
        computerType = "Apple Retina Display!";
        break;

    default:
        computerType = "Android";
        break;
    }
    if(computerType == "Android")
    {
        int dpi = logicalDpiX();
        if(dpi > 280)
            computerType += " xhdpi";
        else if(dpi > 220)
            computerType += " hdpi";
        else if(dpi > 140)
            computerType += " mdpi";
        else if(dpi > 100)
            computerType += " ldpi";
    }


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

    m_winBox = new WinBox(this);
    m_winBox->hide();
    QObject::connect(m_winBox,SIGNAL(newGame(void)),this,SLOT(on_newGame(void)));
    QObject::connect(m_winBox,SIGNAL(exit(void)),this, SLOT(on_MainMenu(void)));


    m_overlay = new Overlay(m_winBox,this);
    m_overlay->hide();
    QObject::connect(m_overlay, SIGNAL(finished()), this, SLOT(on_endOfVictoryDance()));

    m_additionalStyleSheet = "";
#ifdef Q_OS_IOS
    m_additionalStyleSheet =
            ""
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
            "font-size: 36pt;"
            "}"

            "QDialog QPushButton {"
            ""
            "font-size: 24pt;"
            "}"


            "QMessageBox QLabel {"
            ""// top right bottom left
            "padding: 100px 20px 20px 20px;"
            "font-size: 36pt;"
            "}"

            "QMessageBox QPushButton {"
            ""
            "font-size: 24pt;"
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
    Q_UNUSED(shortcut);
    on_refreshStyleSheet();
    

    //    QTimer * t2 = new QTimer;
    //    t2->setSingleShot(true);
    //    QObject::connect(t2, SIGNAL(timeout()), this, SLOT(myAdjustSize()));
    //    t2->start(400);
    
    //    QProgressDialog * prog = new QProgressDialog();
    //    prog->setValue(20);
    //    this->addWidget(prog);
    //    this->setCurrentWidget(prog);
    
    QObject::connect(m_dict, SIGNAL(ready()), this, SLOT(on_finishedLoading()));
    
    m_currWidget = m_mainMenu;
    
    QStatusBar * bar = this->currentWidget()->findChild<QStatusBar*>();
    //    bar->setFixedHeight(30);
    bar->setMinimumWidth(200);
    bar->setStyleSheet("background:white;");
    bar->showMessage("Loading dictionary");
    foreach(QPushButton * btn, this->currentWidget()->findChildren<QPushButton*>())
    {
        if(btn->text() != "Help" && btn->text() != "Settings")
            btn->setDisabled(true);
    }
    // start a delayed init
    QTimer * t = new QTimer;
    t->setSingleShot(true);
    QObject::connect(t, SIGNAL(timeout()), m_dict, SLOT(init()));
    t->start(500);

    foreach(QTextEdit * txt, this->findChildren<QTextEdit*>())
    {
        txt->setTextInteractionFlags(Qt::NoTextInteraction);
    }

    foreach(QStatusBar * statusBar, this->findChildren<QStatusBar*>())
    {
        statusBar->setStyleSheet("background: rgba(255,255,255,0.5);");
        statusBar->ensurePolished();
        statusBar->adjustSize();
    }

    m_statsTimer = new QTimer;
    m_statsTimer->setInterval(1000);
    QObject::connect(m_statsTimer, SIGNAL(timeout()), this, SLOT(updateStats()));

    m_prevPage = m_mainMenu;
}


void MainStack::showEvent(QShowEvent *)
{
    qDebug() << Q_FUNC_INFO;
    static bool firstRun = true;
    if(!firstRun)
        return;
    myAdjustSize();

    grabGesture(Qt::PinchGesture);

    if(false)
    {
        QWindow * window = this->windowHandle();
        if(window)
        {
            m_window = window;
            // These don't show Window states at all on iOS
            QObject::connect(window, SIGNAL(visibilityChanged(QWindow::Visibility)), this, SLOT(on_visibilityChanged(QWindow::Visibility)));
            QObject::connect(window, SIGNAL(activeChanged()), this, SLOT(on_activeChanged()));
            QObject::connect(window, SIGNAL(windowStateChanged(Qt::WindowState)), this, SLOT(on_windowStateChanged(Qt::WindowState)));
            QObject::connect(window, SIGNAL(destroyed()), this, SLOT(on_destroyed()));
            QObject::connect(window, SIGNAL(screenChanged(QScreen*)), this, SLOT(on_screenChanged(QScreen*)));
        }
    }

    // This works on iOS!!!
    QObject::connect(qApp, SIGNAL(applicationStateChanged(Qt::ApplicationState)), this, SLOT(on_appStateChanged(Qt::ApplicationState)));
}

void MainStack::on_screenChanged(QScreen*)
{
    qDebug() << Q_FUNC_INFO;
}

void MainStack::on_appStateChanged(Qt::ApplicationState state)
{
    static bool wasInActive = false;
    qDebug() << Q_FUNC_INFO;
    switch(state)
    {
    case Qt::ApplicationSuspended:
        qDebug() << "suspended";// this line doesn't print every time until after the app comes back.
        // app could close at any time
        break;
    case Qt::ApplicationHidden:
        // hidden and running in the background
        // stop graphics
        qDebug() << "hidden";
        break;
    case Qt::ApplicationInactive:
        if(m_server->isConnected())
            wasInActive = true;
        // incoming call or sms message?
        // reduce CPU intensive tasks
        //        writeSettings();

        qDebug() << "inactive"; // usually followed by suspended on iOS

        // store wifi settings
        // notify other wifi player that we are suspended
        //            m_server->writeSettings();

        break;
    case Qt::ApplicationActive:
        // We are up and running!
        qDebug() << "active";
        if(wasInActive)
            m_server->readSettings();
        wasInActive = false;
        break;
    default:
        qDebug() << "Unknown application state";
        break;
    }
}


void MainStack::on_destroyed()
{
    qDebug() << Q_FUNC_INFO;
}

void MainStack::on_visibilityChanged(QWindow::Visibility)
{
    qDebug() << Q_FUNC_INFO;
}

void MainStack::on_activeChanged()
{
    qDebug() << Q_FUNC_INFO;
}

void MainStack::on_windowStateChanged(Qt::WindowState)
{
    qDebug() << Q_FUNC_INFO;
}


void MainStack::myAdjustSize()
{

    
    QScreen *screen = QGuiApplication::screens().first();
    
    
#if defined(Q_OS_IOS) || defined(Q_OS_ANDROID)
#ifdef Q_OS_IOS
    if(screen->size().width() == 320 || screen->size().height() == 320)
#else
    if(screen->size().width() <= 720 || screen->size().height() <= 720)
#endif
#else
    if(false)
#endif
        //    if(true)
        //#else
        //    if(false)
        //#endif
    {
        // we are an iphone!
        qDebug() << Q_FUNC_INFO << "Appending to stylesheet!";

        this->setStyleSheet(this->styleSheet() +
                            "QFrame {padding: 5px;}"
                            "LetterButton {"
                            "padding: 8px 4px;"
                            "margin: 4px 0px;"
                            //                            "border-radius: 3px;"
                            "}"
                            "QLineEdit {padding: 15px;}"
                            "QPushButton {"
                            "    padding: 8;"
                            "    margin: 2;"
                            "    /* font-family: \"Times\"; */"
                            "    font-size: 12pt;"
                            "    border-radius: 4px;"
                            "    border-style: inset;"
                            "    border-width: 2px;"
                            "    /*  min-width: 10em; */"
                            "    background: white;"
                            "}"
                            "QTextEdit {padding: 0px;}"

                            );

    }
    else
    {
        this->setStyleSheet(this->styleSheet() +
                            "QFrame {padding: 10px;}"
                            "QTextEdit {padding: 0px;}"
                            "LetterButton {"
                            "padding: 8px 4px;"
                            "margin: 4px 0px;"
                            //                            "border-radius: 3px;"
                            "}"
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



    // if
    qDebug() << "Information for screen:" << screen->name();
    qDebug() << "  Available geometry:" << screen->availableGeometry().x() << screen->availableGeometry().y() << screen->availableGeometry().width() << "x" << screen->availableGeometry().height();
    qDebug() << "  Available size:" << screen->availableSize().width() << "x" << screen->availableSize().height();
    qDebug() << "  Available virtual geometry:" << screen->availableVirtualGeometry().x() << screen->availableVirtualGeometry().y() << screen->availableVirtualGeometry().width() << "x" << screen->availableVirtualGeometry().height();
    qDebug() << "  Available virtual size:" << screen->availableVirtualSize().width() << "x" << screen->availableVirtualSize().height();
    qDebug() << "  Depth:" << screen->depth() << "bits";
    qDebug() << "  Geometry:" << screen->geometry().x() << screen->geometry().y() << screen->geometry().width() << "x" << screen->geometry().height();
    qDebug() << "  Logical DPI:" << screen->logicalDotsPerInch();
    qDebug() << "  Logical DPI X:" << screen->logicalDotsPerInchX();
    qDebug() << "  Logical DPI Y:" << screen->logicalDotsPerInchY();
    qDebug() << "  Orientation:" << Orientation(screen->orientation());
    qDebug() << "  Physical DPI:" << screen->physicalDotsPerInch();
    qDebug() << "  Physical DPI X:" << screen->physicalDotsPerInchX();
    qDebug() << "  Physical DPI Y:" << screen->physicalDotsPerInchY();
    qDebug() << "  Physical size:" << screen->physicalSize().width() << "x" << screen->physicalSize().height() << "mm";
    qDebug() << "  Primary orientation:" << Orientation(screen->primaryOrientation());
    qDebug() << "  Refresh rate:" << screen->refreshRate() << "Hz";
    qDebug() << "  Size:" << screen->size().width() << "x" << screen->size().height();
    qDebug() << "  Virtual geometry:" << screen->virtualGeometry().x() << screen->virtualGeometry().y() << screen->virtualGeometry().width() << "x" << screen->virtualGeometry().height();
    qDebug() << "  Virtual size:" << screen->virtualSize().width() << "x" << screen->virtualSize().height();


    int diagonal_squared_mm = screen->physicalSize().width()*screen->physicalSize().width() + screen->physicalSize().height()*screen->physicalSize().height();
    int dpi = screen->logicalDotsPerInch();
    // Is the diagonal of the screen less than 6 inches?
    if(diagonal_squared_mm < (6*25.4)*(6*25.4))
    {
        // Treat it like an iphone or an LG or Samsung phone
        qDebug() << "Smaller than 6\" on diagonal";
#ifdef Q_OS_IOS
        m_dpiFactor = 2;
#elif defined(Q_OS_ANDROID)
        m_dpiFactor = 1.5;
#endif
    }
    else
    {
        // Treat it like an iPad or an iPad mini

        // Scale most things up by 2, so it is easier
        // to click with a finger instead of a thumb
        qDebug() << "Larger than 6\" on diagonal";
        m_dpiFactor = 3;
    }
    QSettings s;
    qreal letter_button_scale_factor = s.value("letter_button_scale_factor","1.0").toString().toDouble();
    m_dpiFactor *= (qreal)dpi/96;
    qDebug() << "End dpi factor:" << m_dpiFactor << letter_button_scale_factor;
    emit updateSize(m_dpiFactor, letter_button_scale_factor);
    this->on_updateSize(m_dpiFactor*letter_button_scale_factor);
    this->readSettings();

    this->currentWidget()->adjustSize();
}

void MainStack::on_finishedLoading()
{
    qDebug() << this->geometry();

    m_dict->loadFrequencyList(m_dict->wordLength(), m_allowDoubleLetters->isChecked());
    QStatusBar * bar = this->currentWidget()->findChild<QStatusBar*>();
    bar->showMessage("Ready to go!");
    foreach(QPushButton * btn, this->currentWidget()->findChildren<QPushButton*>())
    {
        btn->setDisabled(false);
    }

    //    m_dict->createShuffledListOfAvailableWords(3,true,0);
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
        qDebug() << "Using filesystem resource file";

        // Apply the loaded stylesheet
        QString style( styleFile.readAll() + m_additionalStyleSheet);
        qApp->setStyleSheet( style );

        this->ensurePolished();
    }
    else
    {
        styleFile2.open( QFile::ReadOnly );
        qDebug() << "Using compiled resource file" << m_additionalStyleSheet.size();

        // Apply the loaded stylesheet
        QString style( styleFile2.readAll() + m_additionalStyleSheet);
        qApp->setStyleSheet( style );

        this->ensurePolished();
    }
}

void MainStack::on_endOfVictoryDance()
{
    QSettings s;
    int victory_count = s.value("victory_count", 0).toInt();

    QStringList victoryMessages;
    victoryMessages
            << "You are super awesome!"
            << "Excellent.  Most excellent."
            << "Good job. That is all I have to say."
            << "Fantastic work!"
            << "You cracked that code so fast."
            << "Jotto master."
            << "You did that faster than GladOs expected."
            << "Amazing job!!!"
            << "I heard you say in your head, \"Eureka!\""
            << "Jotto pro!"
            << "You are super awesome!  Go team!"
            << "If this was pod racing, you are Anakin."
            << "Congrats. Live long and prosper."
            << "oooo O_-_O oooo ... I can see your smile."
            << "Well, that was easy."
            << "Shoot. There's no foolin\' you."
            << "You hacked in faster than Tony Stark!"
            << "Knock, knock. Who's there? A Jotto Winner!"
            << "A jotto master, you are."
            << "Are you going to try more letters this time?"
               ;




    QPushButton * btn = this->currentWidget()->findChild<QPushButton *>("Give Up");
    if(btn)
        btn->setEnabled(true);
    QStatusBar * bar = this->currentWidget()->findChild<QStatusBar *>();
    if(bar)
    {
        bar->showMessage(victoryMessages.at(victory_count % victoryMessages.size()));
        victory_count++;
        s.setValue("victory_count", victory_count);
    }
    if(btn)
        btn->setFocus();
}

void MainStack::closeEvent(QCloseEvent *)
{
    // doesn't work on iOS
    qDebug() << Q_FUNC_INFO;
    writeSettings();
}

void MainStack::hideEvent(QHideEvent *)
{
    // doesn't work on iOS
    qDebug() << Q_FUNC_INFO;
    writeSettings();
}

bool MainStack::event(QEvent *event)
{
    if (event->type() == QEvent::Gesture)
        return gestureEvent(static_cast<QGestureEvent*>(event));
    return QWidget::event(event);
}

bool MainStack::gestureEvent(QGestureEvent *event)
{
    //    qDebug() << "gestureEvent():" << event->gestures().size();
    //    if (QGesture *swipe = event->gesture(Qt::SwipeGesture))
    //        swipeTriggered(static_cast<QSwipeGesture *>(swipe));
    //    else if (QGesture *pan = event->gesture(Qt::PanGesture))
    //        panTriggered(static_cast<QPanGesture *>(pan));
    if (QGesture *pinch = event->gesture(Qt::PinchGesture))
        pinchTriggered(static_cast<QPinchGesture *>(pinch));
    else
        return QWidget::event(event);
    return true;
}

void MainStack::pinchTriggered(QPinchGesture *gesture)
{
    QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();
    if (changeFlags & QPinchGesture::RotationAngleChanged) {
        const qreal value = gesture->property("rotationAngle").toReal();
        const qreal lastValue = gesture->property("lastRotationAngle").toReal();
        const qreal rotationAngleDelta = value - lastValue;
        rotationAngle += rotationAngleDelta;
        //        qDebug() << "pinchTriggered(): rotation by" << rotationAngleDelta << rotationAngle;
    }
    if (changeFlags & QPinchGesture::ScaleFactorChanged) {
        qreal value = gesture->property("scaleFactor").toReal();
        currentStepScaleFactor = value;
        //        qDebug() << "pinchTriggered(): " << currentStepScaleFactor;
        //        on_sliderChanged();
        QSlider * slider = this->currentWidget()->findChild<QSlider*>();
        if(slider)
            slider->setValue(slider->value() * currentStepScaleFactor);
    }
    if (gesture->state() == Qt::GestureFinished) {
        scaleFactor *= currentStepScaleFactor;
        currentStepScaleFactor = 1;
    }
    update();
}

void MainStack::on_sliderChanged()
{
    QSlider * slider = this->currentWidget()->findChild<QSlider*>();
    if(slider)
        on_sliderChanged(slider->value());
}

void MainStack::on_sliderChanged(int size)
{
    //    qDebug() << "Slider" << size;
    QSettings s;
    s.setValue("text_edit_font_size", size);

    size *= m_dpiFactor;
    foreach(QTextEdit* txt, this->findChildren<QTextEdit*>())
    {
        txt->setFontPointSize((qreal)size/10.0);
        txt->update();
    }

    foreach(Highlighter * h, this->findChildren<Highlighter *>())
    {
        h->setFontSize((qreal)size/10.0);
    }

    foreach(QLineEdit * lineEdit, this->findChildren<QLineEdit*>())
    {
        QFont f = lineEdit->font();
        f.setPointSize(size/10);
        lineEdit->setFont(f);
    }

    foreach(QStatusBar * statusBar, this->findChildren<QStatusBar*>())
    {
        QFont f = statusBar->font();
        f.setPointSize(size/10);
        statusBar->setFont(f);
        //        this->currentWidget()->adjustSize();
    }
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
    m_showStatsDuringGame->setChecked(s.value("show_stats_during_game", false).toBool());
    m_preventDuplicateGuesses->setChecked(s.value("prevent_duplicate_guesses", true).toBool());
    m_letterButtonScaleFactorCombo->setCurrentIndex(m_letterButtonScaleFactorCombo->findText(s.value("letter_button_scale_factor","1.0").toString()));
    m_soundEffects->setChecked(s.value("sound_effects", true).toBool());

    int fontSize = 140;//s.value("text_edit_font_size", 140).toInt();
    if(fontSize < 100) fontSize = 100;
    else if (fontSize > 360) fontSize = 100;
    qDebug() << "fontSize" << fontSize;

    foreach(QSlider* slider, this->findChildren<QSlider*>("font_size_slider"))
    {
        slider->setValue(fontSize);
    }
    fontSize *=  m_dpiFactor;
    foreach(QTextEdit * txt, this->findChildren<QTextEdit*>())
    {
        txt->setFontPointSize(fontSize/10);
    }

    foreach(QLineEdit * lineEdit, this->findChildren<QLineEdit*>())
    {
        QFont f = lineEdit->font();
        f.setPointSize(fontSize/10);
        lineEdit->setFont(f);
    }

    foreach(QStatusBar * statusBar, this->findChildren<QStatusBar*>())
    {
        QFont f = statusBar->font();
        f.setPointSize(fontSize/10);
        statusBar->setFont(f);

        statusBar->setSizeGripEnabled(false);
    }

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
    //    s.setValue("text_edit_font_size", this->findChild<QTextEdit*>()->fontPointSize()*10 * 96 / this->physicalDpiX());
    s.setValue("show_stats_during_game",m_showStatsDuringGame->isChecked());
    s.setValue("prevent_duplicate_guesses", m_preventDuplicateGuesses->isChecked());
    s.setValue("letter_button_scale_factor",m_letterButtonScaleFactorCombo->currentText());
    //    qDebug() << this->findChild<QTextEdit*>()->fontPointSize()*10;

    emit updateSize(m_dpiFactor, m_letterButtonScaleFactorCombo->currentText().toDouble());
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
    QString word_def_url_source =  "view-source:www.dictionary.com/browse/"+word+"?s=t";

#ifdef Q_OS_ANDROID
    //    QObject::connect(lineEdit, SIGNAL(returnPressed()), qApp->inputMethod(), SLOT(hide()));
    //    qApp->inputMethod()->hide();
    //    m_returnPressedTimer->start();
#endif

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
        if(m_dict->isWordRecentlyGuessed(word) && m_preventDuplicateGuesses->isChecked())
        {
            bar->showMessage("\"" + word + "\" has already been guessed.", timeout);
        }
        else if(m_correctLength->isChecked()
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
        m_dict->addWordToListOfRecentGuesses(word);
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

        updateGuessCount();

        if(word == m_theirSecretWord)
        {
            m_stopWatch.pause();
            updateStats();
            m_dict->addToOldSecretWords(m_theirSecretWord,
                                        m_stat_numOfGuesses,
                                        m_random_count,
                                        true, m_stat_timeText,
                                        this->currentWidget() == m_twoPlayerBoard);
            // Game Over, you win!
            emit appendToYours("Correct: " + word);
            m_server->writeData("\nThe other player guessed your word!");

            //m_overlay->setTime(m_statsTimer-);
            this->findChild<QPushButton *>("Give Up")->hide();
            m_overlay->setNumberOfGuesses(m_stat_numOfGuesses);
            m_overlay->startAnimation();
            m_overlay->show();
            m_overlay->raise();


            QPushButton * btn = this->currentWidget()->findChild<QPushButton *>("Give Up");
            //btn->setText("Done");
            btn->setDisabled(true);
            lineEdit->setDisabled(true);
            QPushButton * btn2 = this->currentWidget()->findChild<QPushButton *>("Random");
            if(btn2)
                btn2->setDisabled(true);

            this->currentWidget()->findChild<QLabel*>("Guess Rate")->show();
            this->currentWidget()->findChild<QLabel*>("GR Label")->show();
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
    }
    lineEdit->clear();
}

void MainStack::updateGuessCount(bool reset)
{
    //    QPushButton * btn = this->currentWidget()->findChild<QPushButton *>("Give Up");
    //    if(btn->text() == "Done")
    //    {
    ////        label->setText();
    //    }

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
    updateStats();
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

    m_random_count = 0;

    QString input;
    bool ok;
    QString errorText;
    int ret;
    do
    {
        QInputDialog * getTextDialog = new QInputDialog();
        getTextDialog->setInputMode(QInputDialog::TextInput);
        getTextDialog->setWindowTitle("Jotto - Set Secret Word");
        getTextDialog->setLabelText(
                    "Please enter a \n"
                    + QString::number(m_dict->wordLength())
                    + " letter word." + errorText);



        ret = getTextDialog->exec();
        input = getTextDialog->textValue();
        delete getTextDialog;

        //        input = QInputDialog::getText(this,
        //              "Jotto - Set Secret Word",
        //              "Please enter a "
        //              + QString::number(m_dict->wordLength())
        //              + " letter word.", QLineEdit::Normal, QString(), &ok).toLower();
        ok = (ret == QDialog::Accepted);

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
#if defined(Q_OS_IOS) || defined(Q_OS_ANDROID)
    // don't auto select the tap here area
#else
    this->currentWidget()->findChild<QLineEdit*>()->setFocus();
#endif
    on_sliderChanged();
}

void MainStack::on_onePlayer()
{
    qDebug() << "on_onePlayer() running";
    QSettings s;
    bool explain_zoom = s.value("help/explain_zoom", true).toBool();
    s.setValue("help/explain_zoom", false);

    m_random_count = 0;

    if(false)
    {
        QString input, errorText;
        QInputDialog * getTextDialog = new QInputDialog();
        getTextDialog->setInputMode(QInputDialog::TextInput);
        getTextDialog->setWindowTitle("Jotto - Set Secret Word");
        getTextDialog->setLabelText(
                    "Please enter a "
                    + QString::number(m_dict->wordLength())
                    + " letter word." + errorText);


        int ret;

        ret = getTextDialog->exec();
        input = getTextDialog->textValue();
        delete getTextDialog;

        Q_UNUSED(ret);
        //        m_overlay->startAnimation();
        //        m_overlay->show();
        //        m_overlay->raise();
    }

    foreach(QWidget * w, this->findChildren<QWidget *>("Stats Widget"))
    {
        w->setVisible(m_showStatsDuringGame->isChecked());
    }

    // Pick a random word from the dictionary based on difficulty
    //    m_theirSecretWord = m_dict->getNewSecretWord(16, m_allowDoubleLetters->isChecked());
    m_theirSecretWord = m_dict->getNewSecretWord(0,25);
    qDebug() << "The secret word: " << m_theirSecretWord;

    this->setCurrentWidget(m_onePlayerBoard);// one player board

    emit appendToYours("Random secret word picked.");
    if(explain_zoom)
        emit appendToYours("Pinch/pull with 2 fingers to resize text!");

    this->currentWidget()->findChild<QLineEdit*>()->setEnabled(true);
#if defined(Q_OS_IOS) || defined(Q_OS_ANDROID)
    // don't auto select the tap here area
#else
    this->currentWidget()->findChild<QLineEdit*>()->setFocus();
#endif

    m_stopWatch.start();
    m_stopWatch.restart();
    m_statsTimer->start(1000);
    foreach(QLabel * label,this->findChildren<QLabel*>("Guess Rate"))
    {
        label->hide();
    }
    foreach(QLabel * label,this->findChildren<QLabel*>("GR Label"))
    {
        label->hide();
    }
    on_sliderChanged();
}

void MainStack::on_newGame()
{
    this->resetBoard();
    this->on_onePlayer();
}

void MainStack::on_settingsButton()
{
    m_prevPage = this->currentWidget();

    foreach(QWidget * w, m_settingsPage->findChildren<QWidget*>())
    {
        if(w->objectName() != "Back"
                && w->objectName() != "Letter Button Size"
                && w->parent()->objectName() != "Letter Button Size"
                && (qobject_cast<QLabel*>(w)
                    || qobject_cast<QComboBox*>(w)
                    || qobject_cast<QCheckBox*>(w)
                    )
                //                    || qobject_cast<QLabel*>(w)
                //                    || qobject_cast<QLabel*>(w)
                )
        {
            w->setEnabled(m_prevPage == m_mainMenu);
        }
    }

    this->setCurrentWidget(m_settingsPage);
}

void addCheckboxToGrid(QGridLayout * grid, QCheckBox * cb)
{
    grid->addWidget(new QLabel(cb->text()), grid->rowCount(), 0);
    grid->addWidget(cb, grid->rowCount() -1, 1);

}

void MainStack::init_settings()
{
    QWidget * w;
    QFormLayout * form;
    Q_UNUSED(form)
    QGridLayout * grid;
    w = new QWidget;

    //    QScrollArea * scroll = new QScrollArea();
    //    form = new QFormLayout;
    grid = new QGridLayout;

    //    QScroller::grabGesture(w);


    QPushButton * btn;
    btn = new QPushButton("Back");
    btn->setObjectName("Back");

    QObject::connect(btn, SIGNAL(clicked()), this, SLOT(on_backButton()));

    QHBoxLayout * hbox = new QHBoxLayout();
    hbox->addWidget(btn);
    hbox->addStretch();
    hbox->addWidget(new QLabel("Jotto Settings"));
    //    grid->addWidget(btn, 0, 0, Qt::AlignLeft);
    grid->addLayout(hbox,0,0,1,2);
    //                    Qt::AlignHCenter);

    //    QCheckBox * checkbox;

    //    ,grid->rowCount(),0,1,2);

    m_inDictionary = new QCheckBox("Guesses must be in dictionary.");
    m_inDictionary->setChecked(true);

    addCheckboxToGrid(grid, m_inDictionary);

    m_correctLength = new QCheckBox("Guesses must be the same length.");
    m_correctLength->setChecked(true);

    //    form->addRow(new QLabel(m_correctLength->text()), m_correctLength);
    addCheckboxToGrid(grid, m_correctLength);

    //    QComboBox * combo;

    m_numLettersCombo = new QComboBox;
    foreach(int i, m_dict->getWordLengths())
    {
        m_numLettersCombo->addItem(QString::number(i));
    }

    m_numLettersCombo->setCurrentIndex(m_numLettersCombo->findText("5"));
    m_numLettersCombo->setMaximumWidth(120);

    //    m_numLettersCombo->set


    //    QHBoxLayout * hbox = new QHBoxLayout;
    //    hbox->addWidget(new QLabel("# Letters"));
    //            hbox->addWidget(m_numLettersCombo);
    //    form->addLayout(hbox, form->rowCount(), 0);
    //    form->addRow("# Letters", m_numLettersCombo);
    grid->addWidget(new QLabel("# Letters"), grid->rowCount(), 0);
    grid->addWidget(m_numLettersCombo, grid->rowCount() -1, 1);

    m_letterButtonScaleFactorCombo = new QComboBox;
    m_letterButtonScaleFactorCombo->setObjectName("Letter Button Size");
    m_letterButtonScaleFactorCombo->addItems(QStringList()
                                             << "0.5" << "0.6" << "0.7" << "0.8"
                                             << "0.9" << "1.0" << "1.1" << "1.2"
                                             << "1.3" << "1.4" << "1.5");
    QLabel * lb_label = new QLabel("Letter Box Scale");
    lb_label->setObjectName("Letter Button Size");
    grid->addWidget(lb_label, grid->rowCount(), 0);
    grid->addWidget(m_letterButtonScaleFactorCombo, grid->rowCount() -1, 1);

    QObject::connect(m_numLettersCombo, SIGNAL(currentIndexChanged(QString)),
                     m_dict, SLOT(setWordLength(QString)));

    m_ephHouseRules = new QCheckBox("Alternate Dbl Letter Rules");
    m_ephHouseRules->setChecked(false);
    //    form->addRow(new QLabel(m_ephHouseRules->text()), m_ephHouseRules);
    addCheckboxToGrid(grid, m_ephHouseRules);

    m_allowDoubleLetters = new QCheckBox("Allow double letters");
    m_allowDoubleLetters->setChecked(true);
    //    form->addWidget(m_allowDoubleLetters);
    //    form->addRow(new QLabel(m_allowDoubleLetters->text()),m_allowDoubleLetters);
    addCheckboxToGrid(grid, m_allowDoubleLetters);


    m_autoMarkZeroLetterGuesses = new QCheckBox("Automark zero letter guesses");
    m_autoMarkZeroLetterGuesses->setChecked(false);
    //    form->addWidget(m_autoMarkZeroLetterGuesses);
    //    form->addRow(new QLabel(m_autoMarkZeroLetterGuesses->text()),m_autoMarkZeroLetterGuesses);
    //    addCheckboxToGrid(grid, m_autoMarkZeroLetterGuesses);

    m_showStatsDuringGame = new QCheckBox("Show time/stats during game");
    //    form->addWidget(m_showStatsDuringGame);
    //    form->addRow(new QLabel(m_showStatsDuringGame->text()),m_showStatsDuringGame);
    addCheckboxToGrid(grid, m_showStatsDuringGame);

    m_preventDuplicateGuesses = new QCheckBox("Prevent duplicate guesses");
    //    form->addWidget(m_showStatsDuringGame);
    //    form->addRow(new QLabel(m_preventDuplicateGuesses->text()),m_preventDuplicateGuesses);
    addCheckboxToGrid(grid, m_preventDuplicateGuesses);

    m_soundEffects = new QCheckBox("Sound Effects");
    addCheckboxToGrid(grid, m_soundEffects);


    QLabel * label = new QLabel();
    QString labelString;
    QTextStream out(&labelString);
    out << "DPI - Logical: " << this->logicalDpiX() << " x " << this->logicalDpiY() << "\n"
        << "Physical: " << this->physicalDpiX() << " x " << this->physicalDpiY() << "\n"
        << "MM: " << this->widthMM() << " x " << this->heightMM();
    label->setText(labelString);
    //    form->addRow(label);

    //    form->setFieldGrowthPolicy (QFormLayout::ExpandingFieldsGrow);
    //    form->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);

    //    grid->addLayout(form,1,0);//,1,1, Qt::AlignHCenter);
    //    form->addRow(new QLabel("TEST"));

    //    form->setLabelAlignment();
    //    grid->addWidget(label, grid->rowCount(), 1);
    //    grid->setContentsMargins(0,0,0,0);
    //    grid->setSpacing(0);
    grid->setRowStretch(grid->rowCount(),1);
    
    w->setLayout(grid);

    //    scroll->setWidget(w);
    //    m_settingsPage = scroll;
    //    scroll->setFixedWidth(this->width());
    //    scroll->setBackgroundRole(QPalette::Window);
    //    scroll->setStyleSheet("background: transparent;"
    //                          "QComboBox{background: white;}"
    //                          "QPushButton{background:white;}");
    //    w->setMaximumWidth(qApp->screens().first()->geometry().width()*.9);
    //    w->adjustSize();
    //    this->addWidget(scroll);
    this->addWidget(w);
    m_settingsPage = w;
    foreach(QCheckBox* cb, w->findChildren<QCheckBox*>())
    {
        cb->setText("");
        cb->adjustSize();
    }
    foreach(QLabel* lb, w->findChildren<QLabel*>())
    {
        //        cb->setText("");
        //        cb->adjustSize();
        lb->setWordWrap(true);
    }

}

void MainStack::on_backButton()
{
    bool reloadFreqList = false;
    if(this->currentWidget() ==  m_settingsPage)
        reloadFreqList = true;
    qDebug() << Q_FUNC_INFO;
    this->setCurrentWidget(m_prevPage);

    if(reloadFreqList)
    {
        writeSettings();


        m_dict->setWordLength(m_numLettersCombo->currentText().toInt());
        m_dict->loadFrequencyList(m_dict->wordLength(), m_allowDoubleLetters->isChecked());
        foreach(QTextEdit * txt, m_helpPage->findChildren<QTextEdit *>())
        {
            txt->clear();
        }
    }
}

void MainStack::init_helpPage()
{
    QWidget * w;
    QGridLayout * grid;
    w = new QWidget;
    grid = new QGridLayout;

    QPushButton * btn;
    btn = new QPushButton("Back");
    btn->setObjectName("Back");
    QObject::connect(btn, SIGNAL(clicked()), this, SLOT(on_backButton()));
    grid->addWidget(btn,0,0,
                    Qt::AlignLeft);
    //                    Qt::AlignHCenter);

    QTabWidget * tabs = new QTabWidget;
    tabs->setTabPosition(QTabWidget::South);

    QFile f("://spoken/double letter summary.txt");
    f.open(QFile::ReadOnly | QFile::Text);


    QString helpText =
            "Welcome to Jotto!\n\n"
            "You play by trying to guess the secret word. "
            "If you are playing a five letter word game, "
            "the secret word is 5 letters long and "
            "only five letter words can be guessed.\n\n"
            "See <a href=\"http://pmify.com/jotto\">http://pmify.com/jotto</a> for more info.";

    QLabel * label = new QLabel();
    label->setOpenExternalLinks(true);

    QString temp = helpText + "\n\n\n" + f.readAll();
    temp = temp.replace('\n',"<br/>");
    temp = temp.replace('\t', "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");
    label->setText(temp);
    label->setTextFormat(Qt::RichText);
    f.close();
    label->setWordWrap(true);
    //    label->setFixedWidth(300);
    //    grid->addWidget(label, grid->rowCount(), 0, Qt::AlignHCenter);


    tabs->addTab(label,"Instructions");

    QTextEdit * txt;

    txt = new QTextEdit;
    //    txt->setReadOnly(true);
    addKineticScrolling(txt);
    txt->setObjectName("Stats");
    tabs->addTab(txt, txt->objectName());

    txt = new QTextEdit;
    //    txt->setReadOnly(true);
    addKineticScrolling(txt);
    txt->setObjectName("A-Z Words");
    tabs->addTab(txt, txt->objectName());

    txt = new QTextEdit;
    //    txt->setReadOnly(true);
    addKineticScrolling(txt);
    txt->setObjectName("Freq Words");
    tabs->addTab(txt, txt->objectName());

    label = new QLabel();
    label->setObjectName("help sub title");
    grid->addWidget(label, grid->rowCount(), 0);

    grid->addWidget(tabs, grid->rowCount(), 0);//, Qt::AlignCenter);
    w->setLayout(grid);
    this->addWidget(w);
    m_helpPage = w;
}

void MainStack::on_helpButton()
{
    qDebug() << Q_FUNC_INFO;
    m_prevPage = this->currentWidget();
    this->setCurrentWidget(m_helpPage);

    QTextEdit * txt = m_helpPage->findChild<QTextEdit *>("Stats");
    if(txt && txt->document()->lineCount() < 2)
    {
        QTimer * t = new QTimer;
        t->setSingleShot(true);
        QObject::connect(t, SIGNAL(timeout()), this, SLOT(dumpCurrentWordLists()));
        t->start(500);
    }
}

void MainStack::dumpCurrentWordLists()
{
    static int cachedWordLength = m_dict->wordLength();
    bool firstRun = true;
    QLabel * label;
    label = m_helpPage->findChild<QLabel *>("help sub title");
    label->setText(m_numLettersCombo->currentText() + " letter words, " +
                   (m_allowDoubleLetters->isChecked()? "with": "without")
                   + " double letters");

    QTextEdit * txt;
    txt = m_helpPage->findChild<QTextEdit *>("Stats");
    if(txt)
    {
        txt->setPlainText(m_dict->getPreviousGameStats());
        txt->selectAll();
        txt->setAlignment(Qt::AlignCenter);
        QTextCursor tc = txt->textCursor();
        tc.clearSelection();
        tc.setPosition(0);
        txt->setTextCursor(tc);
    }

    if(firstRun || m_dict->wordLength() != cachedWordLength)
    {
        firstRun = false;
        cachedWordLength = m_dict->wordLength();
    }
    else
    {
        return;
    }

    txt = m_helpPage->findChild<QTextEdit *>("A-Z Words");
    if(txt)
    {
        txt->setPlainText(m_dict->getAlphaWordList(m_allowDoubleLetters->isChecked()));
        txt->selectAll();
        txt->setAlignment(Qt::AlignCenter);
        QTextCursor tc = txt->textCursor();
        tc.clearSelection();
        tc.setPosition(0);
        txt->setTextCursor(tc);
    }

    txt = m_helpPage->findChild<QTextEdit *>("Freq Words");
    if(txt)
    {
        txt->setPlainText(m_dict->getFreqWordList(m_allowDoubleLetters->isChecked()));
        txt->selectAll();
        txt->setAlignment(Qt::AlignCenter);
        QTextCursor tc = txt->textCursor();
        tc.clearSelection();
        tc.setPosition(0);
        txt->setTextCursor(tc);
    }
}


void MainStack::on_giveUpButton()
{
    qDebug() << Q_FUNC_INFO;
    /*qDebug() << QObject::sender()->objectName();

    QPushButton * btn = 0;
    //if(QObject::sender()->objectName() == "win_box")
    btn = this->currentWidget()->findChild<QPushButton *>("Give Up");

    if(!btn)
    {
        qDebug() << "No Done button found";
        return;

    }
    int ret = QMessageBox::Yes;
    if(btn->text() == "Give Up")
    {*/
    int ret = QMessageBox::Yes;
    QMessageBox * msgBox = new QMessageBox;
    msgBox->setText("You are so close."
                    "\n\n"
                    "Do you really want to give up?");
    msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox->setDefaultButton(QMessageBox::No);

    ret = msgBox->exec();
    delete msgBox;
    /*
}
else
{
// they answered with a correct word
}

*/
    if(ret == QMessageBox::Yes)
    {
        this->on_confirmedGiveUp();
    }

}

void MainStack::on_confirmedGiveUp()
{
    qDebug() << Q_FUNC_INFO;
    /*updateStats();
    m_dict->addToOldSecretWords(m_theirSecretWord,
                                m_stat_numOfGuesses,
                                m_random_count,
                                false, m_stat_timeText,
                                this->currentWidget() == m_twoPlayerBoard);*/
    int ret = QMessageBox::Yes;
    QMessageBox * msgBox = new QMessageBox();
    msgBox->setText("The secret word was:\n\n      "
                    + m_theirSecretWord
                    + "\n\nBetter luck next time.");
    msgBox->setStandardButtons(QMessageBox::Ok);

    ret = msgBox->exec();
    delete msgBox;

    //}

    this->on_MainMenu();
}

void MainStack::on_MainMenu()
{
    qDebug() << Q_FUNC_INFO;
    this->resetBoard();
    this->setCurrentWidget(m_mainMenu);
}

void MainStack::resetBoard()
{
    qDebug() << Q_FUNC_INFO;
    foreach(QTextEdit * txt, this->currentWidget()->findChildren<QTextEdit *>())
    {
        txt->clear();
    }

    foreach(QTextEdit * t,
            this->findChildren<QTextEdit *>("Game Txt"))
    {
        t->clear();
    }
    QTextEdit * t = m_helpPage->findChild<QTextEdit *>("Stats");
    if(t)
        t->clear();
    QStatusBar * bar = this->currentWidget()->findChild<QStatusBar * >();
    bar->clearMessage();
    this->currentWidget()->findChild<QLineEdit*>()->clear();

    updateStats();
    m_dict->addToOldSecretWords(m_theirSecretWord,
                                m_stat_numOfGuesses,
                                m_random_count,
                                false, m_stat_timeText,
                                this->currentWidget() == m_twoPlayerBoard);
    QPushButton * btn2 = this->currentWidget()->findChild<QPushButton *>("Random");
    if(btn2)
        btn2->setEnabled(true);

    emit resetLetters();

    updateGuessCount(true);

    m_overlay->hide();

    m_dict->resetListOfRecentGuesses();



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
    int screenWidth = qMin(qApp->screens().first()->physicalSize().width(), qApp->screens().first()->physicalSize().height());
    int marginStretch = 1;
    if(screenWidth < 100) // mm
        marginStretch = 0;
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
    button->setObjectName("Random");
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
        grid->setRowStretch(row, 0);
        label = new QLabel("Yours");
        grid->addWidget(label,row,0, Qt::AlignRight);
        label = new QLabel("Theirs");
        grid->addWidget(label,row++,1, Qt::AlignLeft);
    }
    grid->setRowStretch(row, 1);

    //    int col = 0;

    hbox = new QHBoxLayout;

    txt = new QTextEdit;
    txt->setObjectName("Game Txt");
    addKineticScrolling(txt);
#ifdef Q_OS_IOS
    txt->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#endif

    highlighter = new Highlighter(txt->document());
    //    txt->setReadOnly(true);

    QSlider * slider;
    slider = new QSlider(Qt::Vertical);
    slider->setObjectName("font_size_slider");
    slider->setRange(100, 360);
    slider->setValue(260);
    slider->setFixedWidth(30);
    //    grid->addWidget(slider,row,col++,Qt::AlignLeft);
    QVBoxLayout * vboxSlider = new QVBoxLayout;
    vboxSlider->addWidget(new QLabel("A"));
    vboxSlider->addWidget(slider);
    vboxSlider->addWidget(new QLabel("a"));

    button = new QPushButton();
    button->setIcon(QIcon("://settings.png"));
    button->setStyleSheet("padding: 10px;");
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(on_settingsButton()));
    vboxSlider->addWidget(button);

    button = new QPushButton("?");
    //    button->setIcon(QIcon("://question.png"));

    button->setStyleSheet("padding: 10px;");
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(on_helpButton()));
    vboxSlider->addWidget(button);

    hbox->addLayout(vboxSlider);
    hbox->addStretch(marginStretch);

    QObject::connect(slider, SIGNAL(valueChanged(int)), this, SLOT(on_sliderChanged(int)));

    //    txt->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    if(is_two_player)
    {
        //        txt->setMaximumWidth(120);
        //        grid->addWidget(txt,row,col++,Qt::AlignRight);
        hbox->addWidget(txt,4);
    }
    else
    {
        hbox->addWidget(txt,4);
        //        grid->addWidget(txt,row++,col++,1,2, Qt::AlignHCenter);
    }
    grid->setRowStretch(grid->rowCount() - 1, 4);
    //    QObject::connect(this, SIGNAL(data(QString)), this, SLOT(on_data(QString)));
    QObject::connect(this, SIGNAL(appendToYours(QString)), txt, SLOT(append(QString)));
    QObject::connect(this, SIGNAL(appendToYours(QString)), this, SLOT(on_appendToTxtEdit(QString)));

    if(is_two_player)
    {
        txt = new QTextEdit;
        txt->setObjectName("Game Txt");
        addKineticScrolling(txt);
#ifdef Q_OS_IOS
        txt->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#endif
        //        txt->setReadOnly(true);
        //        txt->setMaximumWidth(120);
        //        grid->addWidget(txt,row++,col++, Qt::AlignLeft);
        hbox->addWidget(txt,4);
        QObject::connect(m_server, SIGNAL(data(QString)), this, SLOT(on_data(QString)));
        QObject::connect(this, SIGNAL(appendToTheirs(QString)), txt, SLOT(append(QString)));
        QObject::connect(this, SIGNAL(appendToTheirs(QString)), this, SLOT(on_appendToTxtEdit(QString)));
    }

    hbox->addStretch(marginStretch);
    grid->addLayout(hbox,row++,0,1,grid->columnCount());
    grid->setRowStretch(row -1, 5);

    // Draw the alphabet
    flow = new Utils::FlowLayout(0,0,0);
    //    flow->setContentsMargins(0,0,0,0);
    for(int i = 0; i< 26; i++)
    {
        LetterButton * lb = new LetterButton('A' + i);
        QObject::connect(this, SIGNAL(setTheme(QColor, QColor, int)), lb, SLOT(setTheme(QColor, QColor, int)));
        QObject::connect(this, SIGNAL(resetLetters()), lb, SLOT(on_reset()));
        QObject::connect(this, SIGNAL(updateSize(qreal,qreal)), lb, SLOT(updateSize(qreal,qreal)));
        highlighter->connectToLetterButton(lb);
        flow->addWidget(lb);
    }
    QObject::connect(this, SIGNAL(setTheme(QColor, QColor, int)), highlighter, SLOT(setTheme(QColor, QColor, int)));

    emit setTheme(Qt::black, Qt::white, 0);
    emit setTheme(Qt::white, Qt::black, 2);
    emit setTheme(Qt::black, Qt::lightGray, 4);
    emit setTheme(Qt::black, Qt::green, 6);
    emit resetLetters();

    grid->addLayout(flow,row++,0,1,2, Qt::AlignCenter);

    QWidget * statsWidget = new QWidget;
    statsWidget->setObjectName("Stats Widget");

    hbox = new QHBoxLayout;
    statsWidget->setLayout(hbox);

    //    hbox->addWidget(new QLabel(""));
    label = new QLabel("00:00");
    label->setObjectName("Timer");
    hbox->addWidget(label);

    hbox->addStretch();

    label = new QLabel("0.0");
    label->setObjectName("Guess Rate");
    hbox->addWidget(label);
    label = new QLabel("wpm");
    label->setObjectName("GR Label");
    hbox->addWidget(label);

    hbox->addStretch();

    label = new QLabel("Total:");
    hbox->addWidget(label);
    label = new QLabel("0");
    label->setObjectName("Total Guesses");
    hbox->addWidget(label);

    statsWidget->setStyleSheet("QLabel {padding: 0px; margin: 0px; }");
    foreach(QLabel* l, statsWidget->findChildren<QLabel*>())
    {
        l->setMargin(0);
    }

    grid->addWidget(statsWidget, row++, 0,1,2);
    //    flow = new Utils::FlowLayout;
    //    QHBoxLayout * hbox;
    hbox = new QHBoxLayout;
    hbox->addStretch();

    label = new QLabel("Guess");
    label->setObjectName("Guess Label");
    hbox->addWidget(label);
    label->setMargin(0);

    label = new QLabel("1");
    hbox->addWidget(label);
    label->setObjectName("Guess Count");
    label->setMargin(0);

    QLineEdit * lineEdit = new LineEdit;
    lineEdit->setPlaceholderText("tap here!");
#ifdef Q_OS_ANDROID
    QObject::connect(lineEdit, SIGNAL(returnPressed()), qApp->inputMethod(), SLOT(hide()));
    QObject::connect(lineEdit, SIGNAL(returnPressed()), m_returnPressedTimer, SLOT(start()));
    //    QObject::connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(on_lineEdit_returnPressed()));
#else
    QObject::connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(sendData()));

#endif

    hbox->addWidget(lineEdit);
    //    QObject::connect(lineEdit, SIGNAL(editingFinished()), this, SLOT(on_lineEdit_editingFinished()));
    //    QObject::connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(on_lineEdit_returnPressed()));

    button = new QPushButton("Go");
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(sendData()));
    button->setStyleSheet("padding:15px 5px;");
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

void MainStack::updateStats()
{
    //    qDebug() << "Implement updateStats()";

    // get time from m_stopwatch
    QLabel * label = this->currentWidget()->findChild<QLabel*>("Timer");
    m_stat_timeText = m_stopWatch.toString("mm:ss");
    m_overlay->setTime(m_stat_timeText);

    if(label)
        label->setText(m_stat_timeText);

    //    qDebug() << m_stopWatch.elapsed();
    label = this->currentWidget()->findChild<QLabel*>("Guess Count");
    if(label)
        m_stat_numOfGuesses = label->text().toInt() - 1;
    m_overlay->setNumberOfGuesses(m_stat_numOfGuesses);

    label = this->currentWidget()->findChild<QLabel*>("Total Guesses");
    if(label)
        label->setText(QString::number(m_stat_numOfGuesses));// works

    //    m_stat_guessRate = (qreal)m_stopWatch.elapsed()/1000./qMax(1, m_stat_numOfGuesses);
    m_stat_guessRate = m_stat_numOfGuesses/((qreal)(m_stopWatch.elapsed())/1000/60);
    label = this->currentWidget()->findChild<QLabel*>("Guess Rate");
    if(label)
        label->setText(QString::number(m_stat_guessRate,'f',1));
}

void MainStack::on_appendToTxtEdit(QString)
{
    foreach(QTextEdit * txt, this->currentWidget()->findChildren<QTextEdit*>())
    {
        txt->verticalScrollBar()->setSliderPosition(
                    txt->verticalScrollBar()->maximum());
        if(!txt->textCursor().atEnd())
            txt->textCursor().clearSelection();
    }
}


void MainStack::on_lineEdit_returnPressed()
{
    qDebug() << Q_FUNC_INFO;
}


void MainStack::on_lineEdit_editingFinished()
{
    qDebug() << "timer timeout?";
}


void MainStack::addKineticScrolling(QWidget * w)
{
    //    QTextEdit * te = qobject_cast<QTextEdit *>(w);
    //    if(te)
    //        te->setTextInteractionFlags(Qt::NoTextInteraction);
    //    static bool firstRun = true;
#if defined(Q_OS_IOS) || defined(Q_OS_ANDROID)
    //qDebug() << "is iOS!!!";
    //    if(firstRun)
    QScroller::grabGesture(w, QScroller::TouchGesture);
    QScroller::grabGesture(w, QScroller::LeftMouseButtonGesture);
    //    firstRun = false;
    //        QScroller * scroller = QScroller::scroller(w);
    //        Qt::GestureType t = scroller->grabGesture(w);
    //        qDebug() << "Gesture Grabbed" << t;
#else
    QScroller::grabGesture(w, QScroller::LeftMouseButtonGesture);
#endif
}

void MainStack::on_randomGuess()
{
    QLineEdit * lineEdit = this->currentWidget()->findChild<QLineEdit*>();
    QString guess;

    do
    {
        guess = m_dict->getNewSecretWord(0,75).trimmed();

    } while(m_dict->isWordRecentlyGuessed(guess) || guess == m_theirSecretWord);

    m_random_count++;
    lineEdit->setText(guess);
    sendData();
}

void MainStack::setCurrentWidget(QWidget * w)
{
    this->slideInIdx(this->indexOf(w));
    m_currWidget = w;
}

void MainStack::on_endOfPageAnimation()
{
    this->currentWidget()->adjustSize();
}
QWidget * MainStack::currentWidget()

{
    return m_currWidget;
}

void MainStack::on_updateSize(qreal factor)
{
    static bool firstRun = true;
    Q_UNUSED(factor);
    //    QScreen *screen = QGuiApplication::screens().first();
    if(firstRun)
    {
        foreach(QTextEdit * textEdit, this->findChildren<QTextEdit *>())
        {
            Q_UNUSED(textEdit)
            //        qDebug() << "factor" << factor << this->width();
            //        textEdit->setMinimumWidth(screen->availableSize().width()*2/3);
            //            textEdit->setMaximumWidth(textEdit->maximumWidth()*1.5*m_dpiFactor);

        }
        firstRun = false;
    }
}

void MainStack::keyPressEvent(QKeyEvent* ke)
{
    qDebug() << "ms" << ke->key() << "down";
    SlidingStackedWidget::keyPressEvent(ke);
}

void MainStack::keyReleaseEvent(QKeyEvent* ke)
{
    if(ke->key() == Qt::Key_Back)// necessary for Q_OS_ANDROID
    {
        QPushButton * pb = this->currentWidget()->findChild<QPushButton *>("Back");
        if(pb)
        {
            ke->accept();
            this->on_backButton();
            return;
        }


        pb = this->currentWidget()->findChild<QPushButton *>("Give Up");
        if(pb)
        {
            ke->accept();
            this->on_giveUpButton();
            return;
        }
        ke->accept();
    }
    else
    {
        qDebug() << "ms" << ke->key() << "up";

        SlidingStackedWidget::keyPressEvent(ke);

    }
}
