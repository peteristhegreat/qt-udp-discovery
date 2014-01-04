#include "shuffle.h"
#include <QChar>

QString shuffle(QString str)
{
    std::vector<QChar> v;
    for(int i = 0; i < str.length(); i++)
    {
        v.push_back(str.at(i));
    }

    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(v.begin(), v.end(), g);

//    copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
//    std::cout << "\n";
    QString retVal;
    for(unsigned int i = 0; i < v.size(); i++)
    {
        retVal += v.at(i);
    }
    return retVal;
}
