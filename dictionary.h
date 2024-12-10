#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QFile>
#include <QTextStream>
#include <QSet>

class Dictionary : public QObject
{
    Q_OBJECT
public:
    explicit Dictionary(QObject *parent = 0);
    bool contains(QString, bool correct_length = true);
    int wordLength(){ return m_wordLength;}
    QString getNewSecretWord(int difficulty, bool allowDoubleLetters);
    QString getNewSecretWord(int lowPercent, int highPercent);
    QList <int> getWordLengths(){ return m_allowedWordLengths;}

    static bool hasDoubleLetters(QString word)
    {
//        bool hasDoubleLetters = false;

        for(int i = 0; i < word.length(); i++)
        {
            for(int j = i+1; j < word.length(); j++)
            {
                if(word.at(j) == word.at(i))
                {
                    return true;
                }
            }
        }
        return false;
    }
    void createShuffledListOfAvailableWords(int wordLength = 5, bool allowDoubleLetters = true, int low_frequency = 5, int high_frequency = 167640);
    void loadListOfAvaiableWords();
    void saveCurrentIndexForList();
    QString getAlphaWordList(bool allowDoubleLetters)
    {
        QString fileName = "://spoken/"+QString::number(this->wordLength())
                + "alpha" + (allowDoubleLetters?"":"_NDL") + ".txt";
//        QString desc = QString("List of words sorted alphabetically, ")
//                + (allowDoubleLetters?"with":"without") + " double letters\nCount: ";
        QString desc = "Count: ";
        QFile file(fileName);
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            return QString("Cannot read file %1:\n%2.")
                                 .arg(fileName)
                                 .arg(file.errorString());
        }
        QTextStream in(&file);
        return desc + in.readAll();
    }

    QString getFreqWordList(bool allowDoubleLetters)
    {
        QString fileName = "://spoken/"+QString::number(this->wordLength())
                + "freq" + (allowDoubleLetters?"":"_NDL") + ".txt";
//        QString desc = QString("List of words sorted by frequency (Britian spoken), ")
//                + (allowDoubleLetters?"with":"without") + " double letters\nCount: ";
        QString desc = "Count: ";
        QFile file(fileName);
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            return QString("Cannot read file %1:\n%2.")
                                 .arg(fileName)
                                 .arg(file.errorString());
        }
        QTextStream in(&file);
        return desc + in.readAll();
    }
    bool isWordRecentlyGuessed(QString);

    QString getPreviousGameStats();

signals:
    void ready();	
public slots:
    void addWordToListOfRecentGuesses(QString);
    void resetListOfRecentGuesses();
    void loadFrequencyList();
    void loadFrequencyList(int numOfLetters, bool allowDoubleLetters);
    void addToOldSecretWords(QString word, int numOfGuesses, int numOfRandom, bool won,
                             QString time, bool twoPlayer);
    void init();
    void setWordLength(int);
    void setWordLength(QString str){ setWordLength(str.toInt());}

private:
    QSet <QString> m_guessesThisRound;
    QList <int> m_allowedWordLengths;
    int m_wordLength;
    QMap < int, QMap<QString, int> *> m_map;
    QMap < int, QStringList *> m_listmap;
    QString documentsPath;
//    QMap <QString, int> six_letter_words;
//    QMap <QString, int> five_letter_words;
};

#endif // DICTIONARY_H
