#ifndef OVERLAY_H
#define OVERLAY_H

#include <QWidget>
#include <QAudioBuffer>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QPoint>
#include <QGraphicsDropShadowEffect>
#include <QSoundEffect>
#include <QMediaPlayer>
#include <QPushButton>
#include <QLabel>
#include <winbox.h>

#ifdef Q_OS_WIN
#define USE_PLAYER
#endif

#ifdef Q_OS_IOS
//#define USE_PLAYER
#endif

class Overlay : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QPoint star1Pos READ star1Pos WRITE setStar1Pos)
    Q_PROPERTY(QPoint star2Pos READ star2Pos WRITE setStar2Pos)
    Q_PROPERTY(QPoint star3Pos READ star3Pos WRITE setStar3Pos)
    Q_PROPERTY(QPoint textPos READ textPos WRITE setTextPos)
    Q_PROPERTY(int audioHeight READ audioHeight WRITE setAudioHeight)

public:
    Overlay(WinBox *winBox, QWidget *parent);
    QPoint star1Pos() const { return m_star1Pos; }
    QPoint star2Pos() const { return m_star2Pos; }
    QPoint star3Pos() const { return m_star3Pos; }

    void setStar1Pos(QPoint p)
    {
        m_star1Pos = p;
        this->update();
    }
    void setStar2Pos(QPoint p)
    {
        m_star2Pos = p;
        this->update();
    }
    void setStar3Pos(QPoint p)
    {
        m_star3Pos = p;
        this->update();
    }
    QPoint textPos() const { return m_textPos; }
    int audioHeight() const { return m_audioHeight; }
    void setTextPos(QPoint p)
    {
        m_textPos = p;
        this->update();
    }
    void setAudioHeight(int i) { m_audioHeight = i; update(); }

    WinBox* getWinBox()
    {
        return m_winBox;
    }



    void setTime(QString time);
    void setNumberOfGuesses(int num);

signals:
    void finished();


public slots:
    void processAudioBuffer(const QAudioBuffer &buffer);
    void paintEvent(QPaintEvent *event);
    void startAnimation();


protected:
    void resizeEvent(QResizeEvent *event);

private:
    int m_audioHeight;
    QParallelAnimationGroup * m_paraAnimation;
    QParallelAnimationGroup * m_winBox_text;
    QSequentialAnimationGroup * m_seqAnimation;
    QPoint m_star1Pos;
    QPoint m_star2Pos;
    QPoint m_star3Pos;
    QPoint m_textPos;

    WinBox *m_winBox;

    QTime *m_time;

    QAudioOutput * m_audioOutput;
    QAudioBufferOutput * m_audioBufferOutput;
    QMediaPlayer * m_player;
};

#endif // OVERLAY_H
