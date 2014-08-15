#include "overlaydialogbox.h"
#include <QGridLayout>
#include <QGraphicsEffect>
#include <QLabel>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QIcon>

OverlayDialogBox::OverlayDialogBox(QWidget *parent, QDialog * dialog) :
    QWidget(parent), m_dialog(dialog)
{
    setPalette(Qt::transparent);
    this->setStyleSheet("QDialog {"
                        "background: qlineargradient( x1:0.2 y1:0.4, x2:1 y2:0.5, "
                        "stop:0 deepskyblue, stop:1 darkslateblue);"
//                        "font-size: 25pt;"
                        "margin: 0px;"
                        "}"

                        "QDialog QLineEdit {padding: 15px;}"

//                        "QDialog {"
//                        "background: qlineargradient( x1:0.2 y1:0.4, x2:1 y2:0.5, stop:0 deepskyblue, stop:1 darkslateblue);"

//        //                "background: white;"
//                        "}"
                        "QDialog QLabel {"
                        ""// top right bottom left
                        //"padding: 100px 20px 20px 20px;"
                        "font-size: 18pt;"
                        "}"

                        "QDialog QPushButton {"
                        ""
                        "font-size: 24pt;"
                        "}"


                        "QMessageBox QLabel {"
                        ""// top right bottom left
                        //"padding: 100px 20px 20px 20px;"
                        "font-size: 18pt;"
                        "}"

                        "QMessageBox QPushButton {"
                        ""
                        "font-size: 24pt;"
                        "}"
                        );
    // if you have buttons on this overlay you probably don't want this one
//    setAttribute(Qt::WA_TransparentForMouseEvents);

    QGraphicsDropShadowEffect * dse = new QGraphicsDropShadowEffect();
    dse->setBlurRadius(20);
    this->setGraphicsEffect(dse);

    QGridLayout * grid = new QGridLayout();
    this->setLayout(grid);
//    m_dialog->setParent(this);
    grid->addWidget(m_dialog);
//    QVBoxLayout * vbox = new QVBoxLayout();
//    this->setLayout(vbox);
//    vbox->addWidget(m_dialog);

    QObject::connect(m_dialog, SIGNAL(accepted()), this, SIGNAL(accepted()));
    QObject::connect(m_dialog, SIGNAL(finished(int)), this, SIGNAL(finished(int)));
    QObject::connect(m_dialog, SIGNAL(rejected()), this, SIGNAL(rejected()));
//    QObject::connect(m_msgBox, SIGNAL(finished(int)), this, SLOT(close()));

    m_dialog->setPalette(Qt::white);
//    m_msgBox->setStyleSheet("background: white;}");

//    m_dialog->layout()->setSizeConstraint(QLayout::SetNoConstraint);

    this->resize(parent->size());

    this->hide();
}

int OverlayDialogBox::exec()
{
    this->show();
    int retVal = m_dialog->exec();
    this->hide();
    return retVal;
}
