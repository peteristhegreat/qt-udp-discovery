#include "mainstack.h"
#include <QGridLayout>
#include <QHBoxLayout>
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
#include <QSizePolicy>

//void MainStack::init_gui()
//{
//    AspectRatioSvgWidget * svg;
//    QWidget * w;
//    QGridLayout * grid;
//    QVBoxLayout * vbox;
////    QVBoxLayout * group_vbox;
//    QPushButton * btn;
//    QGroupBox * group;
////    QTextEdit * txt;
////    QLineEdit * lineEdit;
////    QLabel * label;
//    QStatusBar * bar;
////    Utils::FlowLayout * flow;
//    QHBoxLayout * hbox;

//    w = new QWidget;
//    vbox = new QVBoxLayout;

//    grid = new QGridLayout;

//    svg = new AspectRatioSvgWidget("://jotto-logo.svg");
//    svg->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
//    grid->addWidget(svg,0,0,1,3);
//    grid->setRowStretch(0, 1);
////    grid->setRowStretch(grid->rowCount(),6);

//    QLabel * label = new QLabel("by Peter Hyatt");
//    label->setFont(QFont("monospace",10));
//    label->setStyleSheet("color: black;");
////    vbox->addWidget(label);
////    vbox->addStretch();

//    grid->addWidget(label, 1, 0, 1, 3,Qt::AlignHCenter);



//    btn = new QPushButton("Start");
//    btn->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
//    btn->setMaximumSize(2000,2000);
//    QObject::connect(btn, SIGNAL(clicked()), this, SLOT(on_onePlayer()));
//    grid->addWidget(btn, 2, 0, 1, 2, Qt::AlignHCenter);
////    vbox->addWidget(btn);


//    btn = new QPushButton("Options");
//    QObject::connect(btn, SIGNAL(clicked()), this, SLOT(on_settingsButton()));
//    grid->addWidget(btn, 3, 1, 1, 2, Qt::AlignHCenter);


//    group = new QGroupBox("Multiplayer");

//    QHBoxLayout * group_hbox = new QHBoxLayout;

//    btn = new QPushButton("Create Game");
//    QObject::connect(btn, SIGNAL(clicked()), this, SLOT(on_createGame()));
//    group_hbox->addWidget(btn, Qt::AlignLeft);

//    btn = new QPushButton("Join Game");
//    QObject::connect(btn, SIGNAL(clicked()), this, SLOT(on_connectToGame()));
//    group_hbox->addWidget(btn, Qt::AlignRight);

//    group->setLayout(group_hbox);
//    group->setStyleSheet("background: lightgray;");

////    vbox->addWidget(group);
//    grid->addWidget(group, 4, 0, 1, 2, Qt::AlignHCenter);



//    btn = new QPushButton("Help");
//    QObject::connect(btn, SIGNAL(clicked()), this, SLOT(on_helpButton()));
//    grid->addWidget(btn, 5, 1);
////    vbox->addWidget(btn);


////    vbox->addWidget(btn);

////    vbox->addStretch();


////    hbox = new QHBoxLayout;
////    hbox->addStretch();
////    hbox->addLayout(vbox);
////    hbox->addStretch();

////    grid->addLayout(hbox,grid->rowCount(), 0);

//    bar = new QStatusBar;
//    grid->addWidget(bar, grid->rowCount(), 0, 1, grid->columnCount());
//    bar->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
//    QObject::connect(m_server, SIGNAL(msg(QString)),bar, SLOT(showMessage(QString)));


//    w->setLayout(grid);
//    this->addWidget(w);// 0
//    m_mainMenu = w;

//    // add widget for one player
//    init_board(false);// 1
//    // add widget for two player
//    init_board(true);// 2

//    // add widget for help
//    init_helpPage(); // 3

//    init_settings();
//}

void MainStack::init_gui()
{
    AspectRatioSvgWidget * svg;
    QWidget * w;
    QGridLayout * grid;
    QVBoxLayout * vbox;
    QVBoxLayout * group_vbox;
    QPushButton * btn;
    QGroupBox * group;
//    QTextEdit * txt;
//    QLineEdit * lineEdit;
//    QLabel * label;
    QStatusBar * bar;
//    Utils::FlowLayout * flow;
    QHBoxLayout * hbox;

    w = new QWidget;
    vbox = new QVBoxLayout;

    grid = new QGridLayout;

    svg = new AspectRatioSvgWidget("://jotto-logo.svg");
    svg->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    grid->addWidget(svg,0,0);
    grid->setRowStretch(0, 1);
//    grid->setRowStretch(grid->rowCount(),6);

    QLabel * label = new QLabel("by Peter Hyatt");
    label->setFont(QFont("monospace",10));
    label->setStyleSheet("color: black;");
    vbox->addWidget(label);
    vbox->addStretch();


    group = new QGroupBox("Multiplayer");

    group_vbox = new QVBoxLayout;

    btn = new QPushButton("Create Game");
    QObject::connect(btn, SIGNAL(clicked()), this, SLOT(on_createGame()));
    group_vbox->addWidget(btn, Qt::AlignLeft);

    btn = new QPushButton("Join Game");
    QObject::connect(btn, SIGNAL(clicked()), this, SLOT(on_connectToGame()));
    group_vbox->addWidget(btn, Qt::AlignRight);

    group->setLayout(group_vbox);

    vbox->addWidget(group);

    btn = new QPushButton("Quick Game");
    QObject::connect(btn, SIGNAL(clicked()), this, SLOT(on_onePlayer()));
    vbox->addWidget(btn);

    btn = new QPushButton("Help");
    QObject::connect(btn, SIGNAL(clicked()), this, SLOT(on_helpButton()));
    vbox->addWidget(btn);

    btn = new QPushButton("Settings");
    QObject::connect(btn, SIGNAL(clicked()), this, SLOT(on_settingsButton()));
    vbox->addWidget(btn);

    vbox->addStretch();

    hbox = new QHBoxLayout;
    hbox->addStretch();
    hbox->addLayout(vbox);
    hbox->addStretch();

    grid->addLayout(hbox,grid->rowCount(), 0);

    bar = new QStatusBar;
    grid->addWidget(bar, grid->rowCount(), 0, 1, grid->columnCount());
    bar->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
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
