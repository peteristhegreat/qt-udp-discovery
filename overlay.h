#ifndef OVERLAY_H
#define OVERLAY_H

#include <QWidget>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QPoint>
#include <QGraphicsDropShadowEffect>
#include <QSoundEffect>
#include <QAudioProbe>
#include <QMediaPlayer>

#ifndef Q_OS_MAC
#define USE_PLAYER
#endif


class Overlay : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QPoint star1Pos READ star1Pos WRITE setStar1Pos)
    Q_PROPERTY(QPoint star2Pos READ star2Pos WRITE setStar2Pos)
    Q_PROPERTY(QPoint star3Pos READ star3Pos WRITE setStar3Pos)
    Q_PROPERTY(QPoint textPos READ textPos WRITE setTextPos)
public:
    Overlay(QWidget *parent);
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
    void setTextPos(QPoint p)
    {
        m_textPos = p;
        this->update();
    }

signals:
    void finished();
public slots:
    void processBuffer(QAudioBuffer);
    void paintEvent(QPaintEvent *event);
    void startAnimation();
private:
    int m_audioHeight;
    QParallelAnimationGroup * m_paraAnimation;
    QSequentialAnimationGroup * m_seqAnimation;
    QPoint m_star1Pos;
    QPoint m_star2Pos;
    QPoint m_star3Pos;
    QPoint m_textPos;

#ifdef USE_PLAYER
    QAudioProbe *m_probe;
    QMediaPlayer * m_player;
#else
    QSoundEffect finishedSoundEffect;
#endif
};

#endif // OVERLAY_H
