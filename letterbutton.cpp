#include "letterbutton.h"
#include <QDebug>
#include <QBrush>
#include <QPalette>

LetterButton::LetterButton(QChar c, QWidget *parent) :
    QLabel(parent)
{
    this->setText(c);
    this->setAlignment(Qt::AlignCenter);
    this->setFixedSize(30, 30);
    QObject::connect(this, SIGNAL(clicked()), this, SLOT(on_clicked()));
    this->setTheme(Qt::black, Qt::white, 0);
    this->setTheme(Qt::white, Qt::black, 2);
    this->on_reset();
    this->setFrameStyle(QFrame::Box);
    this->setAutoFillBackground(true);
//    this->setFlat(true);
//    this->setBackgroundRole(QPalette::Window);
//    this->setForegroundRole(QPalette::WindowText);
}

void LetterButton::setTheme(QColor fg, QColor bg, int i)
{
    while(m_colors.size() < i + 2)
        m_colors.append(QColor(Qt::black));
    m_colors[i] = fg;
    m_colors[i+1] = bg;
    if(m_state == i)
        updateColors();
}

void LetterButton::on_clicked()
{
    m_state += 2;
    if(m_state >= m_colors.size())
        m_state = 0;
    updateColors();
}

void LetterButton::updateColors()
{
//    qDebug() << m_colors.size() << m_state;
    QBrush fg(m_colors.at(m_state % m_colors.size()));
    QBrush bg(m_colors.at((m_state + 1) % m_colors.size()));
    QPalette p;//(  fg,//windowText,
//                bg,//button,
//                bg,//light,
//                bg,//dark,
//                bg,//mid,
//                fg,//text,
//                fg,//bright_text,
//                bg,//base,
//                bg//window
//                );
    p.setColor(this->foregroundRole(), m_colors.at(m_state % m_colors.size()));
    p.setColor(this->backgroundRole(), m_colors.at((m_state+1) % m_colors.size()));
//    p.setColor(QPalette::Light, );


//    p.setColor(QPalette::ButtonText, m_colors.at((m_state+1) % m_colors.size()));
//    p.setColor(QPalette::WindowText, m_colors.at((m_state+1) % m_colors.size()));
    this->setPalette(p);
    this->update();
}

void LetterButton::on_reset()
{
    if(m_state == 0)
        return;
    m_state = 0;
    updateColors();
}

