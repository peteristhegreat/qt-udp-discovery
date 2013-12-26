#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <QObject>
#include <QHash>
#include <QString>

class Dictionary : public QObject
{
    Q_OBJECT
public:
    explicit Dictionary(QObject *parent = 0);

signals:

public slots:

private:
    void init();
    QHash <QString, int> hash;
};

#endif // DICTIONARY_H
