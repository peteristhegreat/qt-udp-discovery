#ifndef WINBOX_H
#define WINBOX_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QPoint>
#include <QLabel>
#include <QDebug>
#include <QMouseEvent>
#include <QString>
class MainStack;

class WinBox : public QWidget
{
    Q_OBJECT
public:
    explicit WinBox(QWidget *parent = 0);

    void setTime(QString time);
    void setNumberOfGuesses(int num);



signals:
    void newGame();
    void exit();

public slots:


protected:
    void mousePressEvent(QMouseEvent *event)
    {
        qDebug() << "Mouse Pressed at " << event->position().x() << ", " << event->position().y();
        qDebug() << "Widget size: "<<this->width() << " x " << this->height();
    }

private:
    QVBoxLayout *layout;
    QVBoxLayout *labels;
    QHBoxLayout *buttons;

    QLabel *m_timeLabel;
    QLabel *m_numberOfGuessesLabel;

    QPushButton *m_exit;
    QPushButton *m_newGame;

    QString m_time;

    MainStack *m_parent;



};

#endif // WINBOX_H
