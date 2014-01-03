#include "highlighter.h"
//#include "mainwindow.h"
#include <QSpinBox>
#include <QDebug>

#include "letterbutton.h"

//class MainWindow;

Highlighter::Highlighter(QTextDocument *parent) :
    QSyntaxHighlighter(parent)
{

}

void Highlighter::connectToLetterButton(LetterButton *lb)
{
    QObject::connect(lb, SIGNAL(stateChanged(QChar, int)), this, SLOT(on_stateChanged(QChar, int)));
}

void Highlighter::setTheme(QColor fg, QColor bg, int state)
{
//    while(m_colors.size() < i + 2)
//        m_colors.append(QColor(Qt::black));
//    m_colors[i] = fg;
//    m_colors[i+1] = bg;

    if(!m_stateToFormat.contains(state))
        m_stateToFormat[state] = new QTextCharFormat;
    m_stateToFormat[state]->setBackground(QBrush(bg));
    m_stateToFormat[state]->setForeground(QBrush(fg));
}

void Highlighter::on_stateChanged(QChar c, int state)
{
//    qDebug() << c << state;
    m_formatMap[c.toLower()] = m_stateToFormat[state];
    rehighlight();
}

void Highlighter::highlightBlock(const QString &text)
{
//    qDebug() << text;
    if(text.length() > 0  && text.at(0).isLetter())
        return;
    for(int i = 0; i < text.size(); i++)
    {
        if(text.at(i).isLetter() && m_formatMap.contains(text.at(i).toLower()))
        {
            setFormat(i, 1, *(m_formatMap[text.at(i).toLower()]));
        }
    }
}
