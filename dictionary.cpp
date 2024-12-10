#include "dictionary.h"
#include <QFile>
#include <QTextStream>
#include <QElapsedTimer>
#include <QTime>
#include <QDebug>
#include <QApplication>
#include <QStringList>
#include "shuffle.h"
#include <QRandomGenerator>

Dictionary::Dictionary(QObject *parent) :
    QObject(parent)
{
    documentsPath = "";
#ifdef Q_OS_IOS
    documentsPath = "../Documents/";
//#elif Q_OS_MAC
//    documentsPath = "Contents/Resources/";
#endif

    rand = new QRandomGenerator(QDateTime::currentMSecsSinceEpoch());

    m_allowedWordLengths << 3 << 4 << 5 << 6 << 7 << 8;

    foreach(int i, m_allowedWordLengths)
    {
        m_map[i] = new QMap < QString, int>;
        m_listmap[i] = new QStringList;
    }

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
        return m_map.contains(s.length()) && m_map[s.length()]->contains(s);
    }
}

void Dictionary::setWordLength(int l)
{
    if(m_allowedWordLengths.contains(l))
        m_wordLength = l;
    else
        m_wordLength = 5;
}

void Dictionary::loadFrequencyList(int numOfLetters, bool allowDoubleLetters)
{
    // read data
    QStringList list;
    int numOfWords = 0;
    QFile fileIn("://spoken/"+QString::number(numOfLetters) + "freq" + (allowDoubleLetters?"":"_NDL") + ".txt");
    qDebug() << "filename" << fileIn.fileName();
    if (fileIn.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream in(&fileIn);
        numOfWords = in.readLine().toInt();

        while (!in.atEnd())
            list += in.readLine();
    } else {
        qCritical() << "error opening output file\n";
    }
    fileIn.close();

    // remove oldSecretWords
    int count = 0;
    QFile fileIn2(documentsPath + QString::number(numOfLetters) + "oldSecretWords.txt");
    if (fileIn2.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream in(&fileIn2);
        while (!in.atEnd())
        {
            QString tmp = in.readLine().split(',').first();
            if(list.removeOne(tmp))
                count++;
        }
    } else {
        qCritical() << "error opening output file\n";
    }
    fileIn2.close();
    qDebug() << "Removed" << count << "words from list of" << numOfWords << "words.";

    // Store list as current list
    (*m_listmap[numOfLetters]).clear();
    (*m_listmap[numOfLetters]) = list;
}

void Dictionary::addToOldSecretWords(QString word, int numOfGuesses, int numOfRandom, bool won,
                                     QString time, bool twoPlayer)
{
    (*m_listmap[m_wordLength]).removeOne(word);

    // write data
    QFile fileOut(documentsPath + QString::number(word.length()) + "oldSecretWords.txt");
    if (fileOut.open(QFile::WriteOnly | QFile::Text | QFile::Append)) {
        QTextStream out(&fileOut);
        out << word << ','
            << numOfGuesses << ','
            << '(' << numOfRandom << ')' << ','
            << (won?"Won":"Lost") << ','
            << time << ','
            << (twoPlayer?"Wifi":"Quick Game")
            << '\n';
    } else {
        qCritical() << "error opening output file\n";
    }
    fileOut.close();
}

QString Dictionary::getPreviousGameStats()
{
    QString fileName = documentsPath + QString::number(this->wordLength()) + "oldSecretWords.txt";
    QString desc = QString("All stats of games with ") + QString::number(this->wordLength()) + " letters:\n\n"
            "Word - Guesses - (Random Guesses) - Win? - Time - Game Type\n";
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
//        return QString("Cannot read file %1:\n%2.")
//                             .arg(fileName)
//                             .arg(file.errorString());
        return QString("Play a game with %1 letters, before you can see any game stats.").arg(this->wordLength());
    }
    QTextStream in(&file);
    return desc + in.readAll().replace(',', " - ");
}

QString Dictionary::getNewSecretWord(int lowPercent, int highPercent)
{
//    if(false)
//    {
//        // factor in difficulty
////        qsrand(QDateTime::currentMSecsSinceEpoch());

//        QHash<QString, int>::const_iterator iter;
//        do
//        {
//            int i = qrand() % m_map[m_wordLength]->size();
//            iter = m_map[m_wordLength]->constBegin();
//            iter += i;
//            qDebug() << "SecretWord?" << iter.key() << iter.value() ;
//            qApp->processEvents();
//        }while(iter.value() < difficulty
//               || (!allowDoubleLetters && Dictionary::hasDoubleLetters(iter.key())));

//        return iter.key();
//    }
//    else
//    {
    // // difficulty selects 0-25%, 25-50%, 50-75%,
    // int i = qrand() %
    //         ((int)(m_listmap[m_wordLength]->size()
    //                *(qreal)(highPercent - lowPercent)/100));
    // i += m_listmap[m_wordLength]->size()*((qreal) lowPercent)/100;
    // QString word = (*m_listmap[m_wordLength]).at(i);
    // return (*m_listmap[m_wordLength]).at(i);

    int i = rand->bounded(
        m_listmap[m_wordLength]->size()*(qreal)(highPercent - lowPercent)/100); // Generate a random index
    i += m_listmap[m_wordLength]->size()*((qreal) lowPercent)/100;
    qDebug() << "i" << i;

    // QMap < int, QStringList *>::const_iterator iter;
    // iter = m_listmap[m_wordLength]->constBegin();
    // std::advance(iter, i);
    // return iter.key();
    return m_listmap[m_wordLength]->at(i);
}

void Dictionary::init()
{
    // TODO: Cache the word lists for faster loading later
    // if files don't exist
    QElapsedTimer time;
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

//    loadFrequencyList();

//    {
//    QFile dict("://dictionary1.txt");
//    dict.open(QFile::ReadOnly);
//    int count = 0;
//    while(!dict.atEnd())
//    {
//        word = dict.readLine().trimmed();

//        if(m_map.contains(word.length()))
//        {
//            if(!(*m_listmap[word.length()]).contains(word))
//                (*m_listmap[word.length()]) << word;
//        }
//        count++;
//        if(count == 1000)
//            qApp->processEvents();
//    }
//    dict.close();
//    qDebug() << "time?" << time.elapsed();
//    }

    emit ready();
}

void Dictionary::createShuffledListOfAvailableWords(int wordLength, bool allowDoubleLetters, int low_frequency, int high_frequency)
{
    // From the dictionary and the current settings, create a list of all possible words

    QStringList list;

    qDebug() << Q_FUNC_INFO << wordLength << allowDoubleLetters << low_frequency << high_frequency;

    QMap<QString, int>::const_iterator iter = m_map[wordLength]->constBegin();
    while (iter != m_map[wordLength]->constEnd())
    {
        if(iter.value() >= low_frequency && iter.value() <= high_frequency)
        {
            if(allowDoubleLetters || !Dictionary::hasDoubleLetters(iter.key()))
                list << iter.key() + QString::number(iter.value());
        }
        iter++;
    }

//    qsrand( QDateTime::currentMSecsSinceEpoch());

//    qDebug() << "ordered" << list;
    if(list.length() > 0)
        list = shuffle(list);
//    qDebug() << list.size();

//    qDebug() << *m_listmap[wordLength] << (*m_listmap[wordLength]).size();

    foreach(int numOfLetters, m_allowedWordLengths)
    {
        {
            // write data
            QFile fileOut(QString::number(numOfLetters) + "freq.txt");
            if (fileOut.open(QFile::WriteOnly | QFile::Text)) {
                QTextStream out(&fileOut);
                out << (*m_listmap[numOfLetters]).size() << '\n';
                for (int i = 0; i < (*m_listmap[numOfLetters]).size(); ++i)
                    out << (*m_listmap[numOfLetters]).at(i) << '\n';
            } else {
                qCritical() << "error opening output file\n";

            }
            fileOut.close();
        }
        (*m_listmap[numOfLetters]).sort(Qt::CaseInsensitive);

        QFile fileOut(QString::number(numOfLetters) + "alpha.txt");
        if (fileOut.open(QFile::WriteOnly | QFile::Text)) {
            QTextStream out(&fileOut);
            out << (*m_listmap[numOfLetters]).size() << '\n';
            for (int i = 0; i < (*m_listmap[numOfLetters]).size(); ++i)
                out << (*m_listmap[numOfLetters]).at(i) << '\n';
        } else {
            qCritical() << "error opening output file\n";

        }
        fileOut.close();

    }
}

void loadListOfAvaiableWords()
{
    // open the current filename that matches the settings in use
}

void saveCurrentIndexForList()
{
    // save the line number of the current list in use
}

void Dictionary::loadFrequencyList()
{
    QElapsedTimer time;
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

                if(!(*m_listmap[word.length()]).contains(word))
                    *m_listmap[word.length()] << word;
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


bool Dictionary::isWordRecentlyGuessed(QString str)
{
    return m_guessesThisRound.contains(str);
}

void Dictionary::addWordToListOfRecentGuesses(QString str)
{
    m_guessesThisRound.insert(str);
}

void Dictionary::resetListOfRecentGuesses()
{
    m_guessesThisRound.clear();
}
