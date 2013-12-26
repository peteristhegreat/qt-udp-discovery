#include "dictionary.h"
#include <QFile>
#include <QTextStream>
#include <QTimer>

Dictionary::Dictionary(QObject *parent) :
    QObject(parent)
{
    // start a delayed init
    QTimer * t = new QTimer;
    t->setSingleShot(true);
    QObject::connect(t, SIGNAL(timeout()), this, SLOT(init()));
    t->start(500);
}

void Dictionary::init()
{
    // if files don't exist

    // load dictionary.txt, and generate x-letter-words lists

    // load 6-letter-words.txt
    // load 5-letter-words.txt
    // load 4-letter-words.txt
}
