#include "shuffle.h"
#include <QDateTime>
#include <QChar>

//QString shuffle(QString str)
//{
//    std::vector<QChar> v;
//    for(int i = 0; i < str.length(); i++)
//    {
//        v.push_back(str.at(i));
//    }

//    std::random_device rd;
//    std::mt19937 g(rd());

//    std::shuffle(v.begin(), v.end(), g);

////    copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
////    std::cout << "\n";
//    QString retVal;
//    for(unsigned int i = 0; i < v.size(); i++)
//    {
//        retVal += v.at(i);
//    }
//    return retVal;
//}

QString shuffle(QString str)
{
//    QStringList strList;
//    strList << "first" << "second" << "third" << "fourth" << "fifth" << "sixth";
//    qDebug() << strList;

    qsrand( QDateTime::currentMSecsSinceEpoch());//QTime(0,0,0).secsTo(QTime::currentTime()) );

    for( int i = str.count() - 1 ; i > 0 ; --i )
    {
        int random = qrand() % str.count();
//        qSwap(str[i],str[random]);
        QChar c = str.at(i);
        str[i] = str.at(random);
        str[random] = c;
    }
    return str;
}
