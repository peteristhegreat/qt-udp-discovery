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
    // start a delayed init
    QTimer * t = new QTimer;
    t->setSingleShot(true);
    QObject::connect(t, SIGNAL(timeout()), this, SLOT(init()));
    t->start(500);
    this->setWordLength(5);
}

bool Dictionary::contains(QString s)
{
    if(m_wordLength == 5)
    {
        return five_letter_words.contains(s);
    }
    else
    {
        return six_letter_words.contains(s);
    }
}

void Dictionary::setWordLength(int l)
{
    m_wordLength = l;
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
        if(word.length() == m_wordLength)
        {
            if(five_letter_words.contains(word))
                five_letter_words[word] = line.split(' ').at(0).toInt();
        }
//        if(word.length() == 5)
//        {
//            five_letter_words[word] = 1;
////            if(word.startsWith('z'))
////                qDebug() << qPrintable(word);
//        }
//        else if(word.length() == 6)
//        {
//            six_letter_words[word] = 1;
//        }
        count++;
        if(count == 1000)
            qApp->processEvents();
    }
    freq.close();
    qDebug() << "time?" << time.elapsed();
}

QString Dictionary::getNewSecretWord(int difficulty)
{
    // TODO, factor in difficulty
    qsrand(QDateTime::currentMSecsSinceEpoch());

//    loadFrequencyList();

//    QTime t;
//    t.start();
    if(m_wordLength == 5)
    {
        QHash<QString, int>::const_iterator iter;// = five_letter_words.constBegin();
        do
        {
            int i = qrand() % five_letter_words.size();
            //        five_letter_words.iterator
            iter = five_letter_words.constBegin();
            iter += i;
            qDebug() << "SecretWord?" << iter.key() << iter.value() ;//<< t.elapsed() << five_letter_words.size();
            qApp->processEvents();
        }while(iter.value() < difficulty);

        return iter.key();
    }
    else
    {
//        int i = qrand() % six_letter_words.size();
        return QString();//six_letter_words.contains(s);
    }

}

void Dictionary::init()
{
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
        if(word.length() == 5)
        {
            five_letter_words[word] = 1;
//            if(word.startsWith('z'))
//                qDebug() << qPrintable(word);
        }
        else if(word.length() == 6)
        {
            six_letter_words[word] = 1;
        }
        count++;
        if(count == 1000)
            qApp->processEvents();
    }
    dict.close();
    qDebug() << "time?" << time.elapsed();

    // load dictionary.txt, and generate x-letter-words lists

    // load 6-letter-words.txt
    // load 5-letter-words.txt
    // load 4-letter-words.txt
    loadFrequencyList();
}
