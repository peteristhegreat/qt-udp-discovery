#include "overlay.h"
#include <QPainter>
#include <QPropertyAnimation>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QAudioBufferOutput>

Overlay::Overlay(WinBox *winBox, QWidget *parent) : QWidget(parent), m_winBox(winBox)
{

    setPalette(Qt::transparent);
    setAttribute(Qt::WA_TransparentForMouseEvents);

    this->setFont(QFont("Times New Roman",40, QFont::Bold, true));


    m_paraAnimation = new QParallelAnimationGroup;
    m_winBox_text = new QParallelAnimationGroup;

    QPropertyAnimation * a;
    a = new QPropertyAnimation(this, "star1Pos");
    a->setStartValue(QPoint(0,-100));
    //a->setEndValue(QPoint(100, 150));     //Ending coordinate of stars
    a->setEndValue(QPoint(0, 0));
    a->setDuration(2000);
    a->setEasingCurve(QEasingCurve::InOutBack);
    m_paraAnimation->addAnimation(a);

    a = new QPropertyAnimation(this, "star2Pos");
    a->setStartValue(QPoint(0,-100));
    //a->setEndValue(QPoint(225, 150));
    a->setEndValue(QPoint(125,0));
    a->setDuration(3000);
    a->setEasingCurve(QEasingCurve::InOutBack);
    m_paraAnimation->addAnimation(a);

    a = new QPropertyAnimation(this, "star3Pos");
    a->setStartValue(QPoint(0,-100));
    //a->setEndValue(QPoint(350, 150));
    a->setEndValue(QPoint(250,0));
    a->setDuration(4000);
    a->setEasingCurve(QEasingCurve::InOutBack);
    m_paraAnimation->addAnimation(a);

    m_seqAnimation = new QSequentialAnimationGroup();

    m_seqAnimation->addAnimation(m_paraAnimation);

    QPropertyAnimation *b = new QPropertyAnimation(this, "audioHeight");
    b->setStartValue(0);
    b->setEndValue(100);
    b->setDuration(1500);
    b->setEasingCurve(QEasingCurve::OutExpo);
    m_seqAnimation->addAnimation(b);


    a = new QPropertyAnimation(this, "textPos");
    a->setStartValue(QPoint(0, 2000));
    //a->setEndValue(QPoint(200, 350));
    a->setEndValue(QPoint(100, 200));
    a->setDuration(3000);
    a->setEasingCurve(QEasingCurve::InOutBack);
    m_winBox_text->addAnimation(a);

    a = new QPropertyAnimation(winBox, "pos");
    a->setStartValue(QPoint(0, -800));
    a->setEndValue(QPoint(parent->width()/2-m_winBox->width()*3/4, parent->height()*3/4-winBox->height()/2));
    a->setDuration(3000);
    a->setEasingCurve(QEasingCurve::OutQuad);
    m_winBox_text->addAnimation(a);

    m_seqAnimation->addAnimation(m_winBox_text);


    QGraphicsDropShadowEffect * dse = new QGraphicsDropShadowEffect();
    dse->setBlurRadius(20);
    this->setGraphicsEffect(dse);

//    effect.setLoopCount(QSoundEffect::Infinite);
//    effect.setVolume(0.25f);

    QObject::connect(m_seqAnimation, SIGNAL(finished()), this, SIGNAL(finished()));

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

void Overlay::paintEvent(QPaintEvent *)
{
    static const QPointF star[5] = {
        QPointF(15.0, 100.0),
        QPointF(100.0, 30.0),
        QPointF(0.0, 30.0),
        QPointF(85.0, 100.0),
        QPointF(50.0, 0.0)
    };

    QPainter painter(this);
    if(this->width() < 500)
        //painter.scale(((qreal)this->width())/500, ((qreal)this->width())/500);
        painter.scale((qreal)(500/this->width()), (qreal)(500/this->width()));
    else
    {
        //painter.translate((this->width() - 500)/2, 0);
        painter.translate(this->width()/2-175, this->height()/5);
    }

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::red));
    painter.setBrush(QBrush(Qt::red));

    //painter.drawLine(width()/8, height()/8, 7*width()/8, 7*height()/8);
    //painter.drawLine(width()/8, 7*height()/8, 7*width()/8, height()/8);

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

     painter.drawRect(0, this->height()*4./5, 5,
                     -this->height()*3./5*(100 - m_audioHeight)/100.);
     //qDebug() << "audio height: " << m_audioHeight;
}

void Overlay::startAnimation()
{
    this->resize(qobject_cast<QWidget*>(this->parent())->size());
    m_player->play();

//    QFile f("temp.txt");
//    f.open(QFile::WriteOnly);
//    f.write(qPrintable(QDir::current().absolutePath()));
//    f.close();
    m_seqAnimation->start();
    m_winBox->resize(this->size()/2);
    m_winBox->update();
    m_winBox->setVisible(true);
    m_winBox->raise();

    setTextPos(QPoint(-200,-200));
    m_winBox->setProperty("pos",QPoint(-1000,-1000));
    //setWinBoxPos(QPoint(-200,-200));
    //m_winBox->mapToParent(winBoxPos());
}

void Overlay::resizeEvent(QResizeEvent *event)
{
    qDebug() << Q_FUNC_INFO;
    m_winBox->resize(this->size()/2);
    m_winBox->update();
    this->update();
}

void Overlay::setTime(QString time)
{
    m_winBox->setTime(time);

}

void Overlay::setNumberOfGuesses(int num)
{

    m_winBox->setNumberOfGuesses(num);
}


