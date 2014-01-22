#include "dictionary.h"
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include <QApplication>
#include <QStringList>

Dictionary::Dictionary(QObject *parent) :
    QObject(parent)
{
    m_allowedWordLengths << 3 << 4 << 5 << 6 << 7 << 8;

    foreach(int i, m_allowedWordLengths)
        m_map[i] = new QHash < QString, int>;

    // start a delayed init
    QTimer * t = new QTimer;
    t->setSingleShot(true);
    QObject::connect(t, SIGNAL(timeout()), this, SLOT(init()));
    t->start(500);
    this->setWordLength(5);
}

bool Dictionary::contains(QString s, bool correct_length)
{
    if(correct_length)
    {
        return m_map[m_wordLength]->contains(s);
    }
    else
    {
        return m_map[s.length()]->contains(s);
    }
}

void Dictionary::setWordLength(int l)
{
    if(m_allowedWordLengths.contains(l))
        m_wordLength = l;
    else
        m_wordLength = 5;
}

void Dictionary::loadFrequencyList()
{
    QTime time;
    time.start();
    QString word, line;
    QFile freq("://frequency.txt");
    freq.open(QFile::ReadOnly);
    int count = 0;
    while(!freq.atEnd())
    {
        line =  freq.readLine();
        word = line.split(' ').at(1);
        if(m_map.contains(word.length()))
        {
            if(m_map[word.length()]->contains(word))
            {
                (*m_map[word.length()])[word] = line.split(' ').at(0).toInt();
//                if(line.split(' ').at(2) == "np0")
//                    qDebug() << word << (*m_map[word.length()])[word];
            }
        }

        count++;
        if(count == 1000)
            qApp->processEvents();
    }
    freq.close();
    qDebug() << "time?" << time.elapsed();
}

QString Dictionary::getNewSecretWord(int difficulty, bool allowDoubleLetters)
{
    // factor in difficulty
    qsrand(QDateTime::currentMSecsSinceEpoch());

    QHash<QString, int>::const_iterator iter;
    do
    {
        int i = qrand() % m_map[m_wordLength]->size();
        iter = m_map[m_wordLength]->constBegin();
        iter += i;
        qDebug() << "SecretWord?" << iter.key() << iter.value() ;
        qApp->processEvents();
    }while(iter.value() < difficulty
           || (!allowDoubleLetters && Dictionary::hasDoubleLetters(iter.key())));

    return iter.key();
}

void Dictionary::init()
{
    // TODO: Cache the word lists for faster loading later
    // if files don't exist
    QTime time;
    time.start();
    QString word;
    QFile dict("://dictionary.txt");
    dict.open(QFile::ReadOnly);
    int count = 0;
    while(!dict.atEnd())
    {
        word = dict.readLine().trimmed();

        if(m_map.contains(word.length()))
        {
            (*m_map[word.length()])[word] = 1;
        }
        count++;
        if(count == 1000)
            qApp->processEvents();
    }
    dict.close();
    qDebug() << "time?" << time.elapsed();

    loadFrequencyList();
}
