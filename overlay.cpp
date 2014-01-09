#include "overlay.h"
#include <QPainter>
#include <QPropertyAnimation>
#include <QDebug>


Overlay::Overlay(QWidget *parent) :QWidget(parent)
{
    setPalette(Qt::transparent);
    setAttribute(Qt::WA_TransparentForMouseEvents);

    this->setFont(QFont("Times",30, QFont::Bold, true));

    m_paraAnimation = new QParallelAnimationGroup;

    QPropertyAnimation * a;
    a = new QPropertyAnimation(this, "star1Pos");
    a->setStartValue(QPoint(0,-100));
    a->setEndValue(QPoint(100, 100));
    a->setDuration(2000);
    a->setEasingCurve(QEasingCurve::InOutBack);

    m_paraAnimation->addAnimation(a);
    a = new QPropertyAnimation(this, "star2Pos");
    a->setStartValue(QPoint(0,-100));
    a->setEndValue(QPoint(225, 100));
    a->setDuration(3000);
    a->setEasingCurve(QEasingCurve::InOutBack);
    m_paraAnimation->addAnimation(a);

    a = new QPropertyAnimation(this, "star3Pos");
    a->setStartValue(QPoint(0,-110));
    a->setEndValue(QPoint(350, 100));
    a->setDuration(4000);
    a->setEasingCurve(QEasingCurve::InOutBack);
    m_paraAnimation->addAnimation(a);

    m_seqAnimation = new QSequentialAnimationGroup();

    m_seqAnimation->addAnimation(m_paraAnimation);


    a = new QPropertyAnimation(this, "textPos");
    a->setStartValue(QPoint(0, 2000));
    a->setEndValue(QPoint(200, 300));
    a->setDuration(3000);
    a->setEasingCurve(QEasingCurve::InOutBack);
    m_seqAnimation->addAnimation(a);

    QGraphicsDropShadowEffect * dse = new QGraphicsDropShadowEffect();
    dse->setBlurRadius(20);
    this->setGraphicsEffect(dse);

//    effect.setLoopCount(QSoundEffect::Infinite);
//    effect.setVolume(0.25f);

    QObject::connect(m_seqAnimation, SIGNAL(finished()), this, SIGNAL(finished()));

#ifdef USE_PLAYER
    m_probe = new QAudioProbe;
    connect(m_probe, SIGNAL(audioBufferProbed(QAudioBuffer)),
            this, SLOT(processBuffer(QAudioBuffer)));
    m_player = new QMediaPlayer;
//    m_player->setVolume(50);

    m_probe->setSource(m_player);
#else
    finishedSoundEffect.setSource(QUrl::fromLocalFile("sounds/finished.wav"));
#endif
}

void Overlay::processBuffer(QAudioBuffer buffer)
{
//    qDebug() << a.sampleCount() << a.data()[0];
    // Assuming 'buffer' is an unsigned 16 bit stereo buffer..
    QAudioBuffer::S16U *frames = buffer.data<QAudioBuffer::S16U>();
    for (int i=0; i < buffer.frameCount(); i++) {
//        qSwap(frames[i].left, frames[i].right);
//        qDebug() << frames[i].average()*100./65535;
        m_audioHeight = frames[i].average()*100./65535;
    }
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
//    painter.drawLine(width()/8, height()/8, 7*width()/8, 7*height()/8);
//    painter.drawLine(width()/8, 7*height()/8, 7*width()/8, height()/8);
    painter.translate(star1Pos());
    painter.drawPolygon(star, 5, Qt::WindingFill);
    painter.translate(-star1Pos() + star2Pos());
    painter.drawPolygon(star, 5, Qt::WindingFill);
    painter.translate(-star2Pos() + star3Pos());
    painter.drawPolygon(star, 5, Qt::WindingFill);
    painter.translate(-star3Pos());

    if(this->geometry().contains(textPos()))
    {
        painter.drawText(textPos(),"WINNER!!!");
    }

#ifdef USE_PLAYER
    painter.drawRect(0, this->height()*4./5, 5,
                     -this->height()*3./5*(100 - m_audioHeight)/100.);
#endif
}

void Overlay::startAnimation()
{
#ifdef USE_PLAYER
    m_player->setMedia(QUrl::fromLocalFile("sounds/finished.wav"));
    m_player->play();
#else
    finishedSoundEffect.play();
#endif
    m_seqAnimation->start();
}
