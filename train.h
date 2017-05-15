#ifndef TRAIN_H
#define TRAIN_H

#include <QObject>
#include "stable.h"
#include <vector>
using namespace std;
using namespace cv;
using namespace cv::face;

class Train: public QThread
{
    Q_OBJECT
public:
    Train();
    ~Train();
    QString msg;
private:
    QDir picPath;
    void run();
    vector<Mat> image;
    vector<int> label;
    void sendMsg(QString msg1);
    QDateTime currentTime;

signals:
    void msgSend(QString);
    void nameTableUpdate();

};

#endif // TRAIN_H
