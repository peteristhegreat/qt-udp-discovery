#ifndef OVERLAY_H
#define OVERLAY_H

#include <QWidget>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QPoint>
#include <QGraphicsDropShadowEffect>
#include <QSoundEffect>
#include <QAudioDecoder>
// #include <QAudioBuffer>
#include <QMediaPlayer>
#include <QAudioOutput>

class Overlay : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QPoint star1Pos READ star1Pos WRITE setStar1Pos)
    Q_PROPERTY(QPoint star2Pos READ star2Pos WRITE setStar2Pos)
    Q_PROPERTY(QPoint star3Pos READ star3Pos WRITE setStar3Pos)
    Q_PROPERTY(QPoint textPos READ textPos WRITE setTextPos)
    Q_PROPERTY(int audioHeight READ audioHeight WRITE setAudioHeight)

public:
    Overlay(QWidget *parent);

    QPoint star1Pos() const { return m_star1Pos; }
    QPoint star2Pos() const { return m_star2Pos; }
    QPoint star3Pos() const { return m_star3Pos; }
    QPoint textPos() const { return m_textPos; }
    int audioHeight() const { return m_audioHeight; }

    void setStar1Pos(QPoint p) { m_star1Pos = p; update(); }
    void setStar2Pos(QPoint p) { m_star2Pos = p; update(); }
    void setStar3Pos(QPoint p) { m_star3Pos = p; update(); }
    void setTextPos(QPoint p) { m_textPos = p; update(); }
    void setAudioHeight(int i) { m_audioHeight = i; update(); }

signals:
    void finished();

public slots:
    void paintEvent(QPaintEvent *event);
    void startAnimation();

private slots:
    void processAudioBuffer(const QAudioBuffer &buffer);

private:
    int m_audioHeight = 0;
    QParallelAnimationGroup *m_paraAnimation;
    QSequentialAnimationGroup *m_seqAnimation;
    QPoint m_star1Pos;
    QPoint m_star2Pos;
    QPoint m_star3Pos;
    QPoint m_textPos;

    // QAudioDecoder *m_audioDecoder;
    QAudioOutput * m_audioOutput;
    QAudioBufferOutput * m_audioBufferOutput;
    QMediaPlayer *m_player; // Only for playing sounds

};

#endif // OVERLAY_H
