#include "overlay.h"
#include <QPainter>
#include <QPropertyAnimation>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QAudioFormat>
#include <QAudioBufferOutput>

Overlay::Overlay(QWidget *parent) : QWidget(parent)
{
    setPalette(Qt::transparent);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setFont(QFont("Times New Roman", 30, QFont::Bold, true));

    // Animation setup
    m_paraAnimation = new QParallelAnimationGroup(this);
    QPropertyAnimation *a = new QPropertyAnimation(this, "star1Pos");
    a->setStartValue(QPoint(0, -100));
    a->setEndValue(QPoint(100, 100));
    a->setDuration(2000);
    a->setEasingCurve(QEasingCurve::InOutBack);
    m_paraAnimation->addAnimation(a);

    a = new QPropertyAnimation(this, "star2Pos");
    a->setStartValue(QPoint(0, -100));
    a->setEndValue(QPoint(225, 100));
    a->setDuration(3000);
    a->setEasingCurve(QEasingCurve::InOutBack);
    m_paraAnimation->addAnimation(a);

    a = new QPropertyAnimation(this, "star3Pos");
    a->setStartValue(QPoint(0, -110));
    a->setEndValue(QPoint(350, 100));
    a->setDuration(4000);
    a->setEasingCurve(QEasingCurve::InOutBack);
    m_paraAnimation->addAnimation(a);

    m_seqAnimation = new QSequentialAnimationGroup(this);
    m_seqAnimation->addAnimation(m_paraAnimation);

    QList<QPoint> positions = {
        QPoint(0, 2000),
        QPoint(200, 300),
        // QPoint(205, 300),
        // QPoint(205, 305),
        // QPoint(200, 305),
        // QPoint(200, 300)
    };

    // Loop through the positions and create animations
    for (int i = 0; i < positions.size() - 1; ++i) {
        QPropertyAnimation *a = new QPropertyAnimation(this, "textPos");
        a->setStartValue(positions[i]);
        a->setEndValue(positions[i + 1]);
        a->setDuration(3000);
        a->setEasingCurve(QEasingCurve::InOutBack);
        // connect(a, &QPropertyAnimation::finished, this, &Overlay::finished);

        m_seqAnimation->addAnimation(a);
    }
    QPropertyAnimation *b = new QPropertyAnimation(this, "audioHeight");
    b->setStartValue(0);
    b->setEndValue(100);
    b->setDuration(1500);
    b->setEasingCurve(QEasingCurve::OutExpo);
    m_seqAnimation->addAnimation(b);

    QGraphicsDropShadowEffect *dse = new QGraphicsDropShadowEffect();
    dse->setBlurRadius(20);
    setGraphicsEffect(dse);

    connect(m_seqAnimation, &QSequentialAnimationGroup::finished, this, &Overlay::finished);


    // Optional: MediaPlayer for playing sound effects
    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_audioBufferOutput = new QAudioBufferOutput(this); // Create audio buffer output

    // Set the audio outputs
    m_player->setAudioOutput(m_audioOutput);
    m_player->setAudioBufferOutput(m_audioBufferOutput);

    connect(m_audioBufferOutput, &QAudioBufferOutput::audioBufferReceived, this, &Overlay::processAudioBuffer);

    m_audioOutput->setVolume(50);
}

void Overlay::processAudioBuffer(const QAudioBuffer &buffer)
{
    if (!buffer.isValid()) {
        qDebug() << "Invalid audio buffer";
        // m_audioHeight = 1;
        return;
    }

    // Get raw audio data from the buffer
    const int16_t *samples = buffer.constData<int16_t>();
    if (!samples) {
        qDebug() << "Buffer data not in Int16 format";
        return;
    }

    // Calculate the maximum amplitude
    int maxAmplitude = 0;
    for (int i = 0; i < buffer.frameCount(); ++i) {
        maxAmplitude = qMax(maxAmplitude, qAbs(samples[i]));
    }

    // Normalize to a percentage
    int normalizedAmplitude = (maxAmplitude * 100) / 32767; // 32767 is the max value for Int16
    // qDebug() << "Current amplitude:" << normalizedAmplitude;
    m_audioHeight = normalizedAmplitude;
}

void Overlay::paintEvent(QPaintEvent *event)
{
    static const QPointF star[5] = {
        QPointF(15.0, 100.0),
        QPointF(100.0, 30.0),
        QPointF(0.0, 30.0),
        QPointF(85.0, 100.0),
        QPointF(50.0, 0.0)
    };

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::red));
    painter.setBrush(QBrush(Qt::red));

    painter.translate(star1Pos());
    painter.drawPolygon(star, 5, Qt::WindingFill);
    painter.translate(-star1Pos() + star2Pos());
    painter.drawPolygon(star, 5, Qt::WindingFill);
    painter.translate(-star2Pos() + star3Pos());
    painter.drawPolygon(star, 5, Qt::WindingFill);
    painter.translate(-star3Pos());

    if (geometry().contains(textPos())) {
        painter.drawText(textPos(), "WINNER!!!");
    }

    // Draw audio visualization
    painter.drawRect(0, height() * 4 / 5, 5,
                     -height() * 3 / 5 * (100 - m_audioHeight) / 100.);
}

void Overlay::startAnimation()
{
    m_player->setSource(QUrl("qrc:/sounds/finished.wav"));
    m_player->play();

    m_seqAnimation->start();
    setTextPos(QPoint(-200, -200));
}
