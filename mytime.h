#ifndef MYTIME_H
#define MYTIME_H

#include <QTime>
#include <QString>

class MyTime : public QTime
{

public:
//    explicit MyTime(QObject *parent = 0)
//    {
//        m_paused = false;
//    }

    int elapsed() const
    {
        if(m_paused)
            return m_offset;
        else
            return QTime::elapsed() + m_offset;
    }
    int restart()
    {
        int retVal;
        if(m_paused)
            retVal = m_offset;
        else
            retVal = QTime::restart() + m_offset;
        m_paused = false;
        m_offset = 0;
        return retVal;
    }
    void start()
    {
        m_paused = false;
        QTime::start();
    }

    bool isPaused()
    {
        return m_paused;
    }

    void startAt(int ms)
    {
        m_offset = ms;
        this->start();
    }

    void setOffset(int ms)
    {
        m_offset = ms;
    }

    void pause()
    {
        m_offset = this->elapsed();
        m_paused = true;
    }

    void resume()
    {
        startAt(m_offset);
        m_paused = false;
    }

    QString toString(const QString & format) const
    {
        int secs = this->elapsed() / 1000;
        int mins = (secs / 60) % 60;
        int hours = (secs / 3600);
        secs = secs % 60;

        QTime timeString(hours, mins, secs);
        return timeString.toString(format);
    }

private:
    bool m_paused;
    int m_offset;
};

#endif // MYTIME_H
