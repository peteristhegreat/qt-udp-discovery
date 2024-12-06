#include "overlay.h"
#include <QPainter>
#include <QPropertyAnimation>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QAudioFormat>

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

    a = new QPropertyAnimation(this, "textPos");
    a->setStartValue(QPoint(0, 2000));
    a->setEndValue(QPoint(200, 300));
    a->setDuration(3000);
    a->setEasingCurve(QEasingCurve::InOutBack);
    m_seqAnimation->addAnimation(a);

    QGraphicsDropShadowEffect *dse = new QGraphicsDropShadowEffect();
    dse->setBlurRadius(20);
    setGraphicsEffect(dse);

    connect(m_seqAnimation, &QSequentialAnimationGroup::finished, this, &Overlay::finished);

    // Audio decoding setup
    m_audioDecoder = new QAudioDecoder(this);
    connect(m_audioDecoder, &QAudioDecoder::bufferReady, this, &Overlay::processBuffer);
    connect(m_audioDecoder, &QAudioDecoder::finished, this, &Overlay::handleDecodingFinished);

    // Optional: MediaPlayer for playing sound effects
    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput;
    m_player->setAudioOutput(m_audioOutput);
    m_audioOutput->setVolume(50);
}

void Overlay::processBuffer()
{
    qDebug() << "In processBuffer";
    while (m_audioDecoder->bufferAvailable()) {
        QAudioBuffer buffer = m_audioDecoder->read();
        if (buffer.isValid()) {
            // Get raw data from the buffer
            const int16_t *samples = buffer.constData<int16_t>();
            if (samples) {
                int maxAmplitude = 0; // To calculate the peak amplitude

                // Iterate over audio samples and process each
                for (int i = 0; i < buffer.frameCount(); ++i) {
                    int sampleValue = static_cast<int>(samples[i]); // Cast sample to int
                    maxAmplitude = qMax(maxAmplitude, qAbs(sampleValue)); // Find max amplitude
                }

                // Normalize and set the audio height
                m_audioHeight = (maxAmplitude * 100) / 32767; // Normalize based on Int16 max value
            }
        }
    }
}

void Overlay::handleDecodingFinished()
{
    qDebug() << "Audio decoding finished";
    m_audioDecoder->stop();
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
    m_audioDecoder->setSource(QUrl("qrc:/sounds/finished.wav"));
    m_audioDecoder->start();

    m_player->setSource(QUrl("qrc:/sounds/finished.wav"));
    m_player->play();

    m_seqAnimation->start();
    setTextPos(QPoint(-200, -200));
}
