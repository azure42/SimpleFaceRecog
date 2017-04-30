#ifndef RECOG_H
#define RECOG_H

#include <QObject>
#include "stable.h"

using namespace std;
using namespace cv;
using namespace cv::face;

class Recog : public QThread
{
    Q_OBJECT

public:
    Recog();
private:
    void run();
};

#endif // RECOG_H
