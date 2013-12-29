#ifndef LETTERBUTTON_H
#define LETTERBUTTON_H

#include <QLabel>
#include <QChar>
#include <QList>
#include <QColor>
#include <QMouseEvent>

class LetterButton : public QLabel
{
    Q_OBJECT
public:
    LetterButton(QChar c, QWidget *parent = 0);
signals:
    void clicked();
public slots:
    void mousePressEvent(QMouseEvent * ){}
    void mouseReleaseEvent(QMouseEvent * ){ emit clicked();}
    void updateColors();
    void setTheme(QColor, QColor, int);
//    void setFont(QFont);
    void on_clicked();
    void on_reset();
private:
    int m_state;
    QList <QColor> m_colors;
};

#endif // LETTERBUTTON_H
