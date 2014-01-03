#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QList>
#include <QColor>
#include <QMap>

class LetterButton;

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    Highlighter(QTextDocument *parent = 0);
    void connectToLetterButton(LetterButton * lb);
public slots:
    void setTheme(QColor fg, QColor bg, int state);
    void on_stateChanged(QChar c, int state);
protected:
    void highlightBlock(const QString &text);
private:
    QList <QColor> m_colors;
    QChar m_char;
    QMap <QChar, QTextCharFormat *> m_formatMap;
    QMap <int, QTextCharFormat *> m_stateToFormat;
};

#endif // HIGHLIGHTER_H
