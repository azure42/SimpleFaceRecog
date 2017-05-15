#ifndef DETECT_H
#define DETECT_H

#include "stable.h"
#include <QPair>
#include <QHash>

using namespace std;
using namespace cv;
using namespace cv::face;

class Detect  : public QThread
{
    Q_OBJECT
public:
    Detect();
    ~Detect();
    QImage mainImg,faceImg;
    QString msg;

private:

    void detectAndDraw( Mat& img, CascadeClassifier& cascade,
                        CascadeClassifier& nestedCascade,
                        double scale, bool tryflip );
    void sendMsg(QString);
    string cascadeName;
    string nestedCascadeName;

    VideoCapture capture;
    Mat frame,faceMat;


    QImage cvMat2QImage(const cv::Mat& mat);
    QTimer *timer;

    void run();

    const bool tryflip = 0;
    CascadeClassifier cascade, nestedCascade;
    const double scale = 1.3;

    QDateTime currentTime;

    QHash<int,QString> nameTable;
    QDir picPath;


signals:
    void msgSend(QString);
    void nameLabelUpdate(QString);


};

#endif // DETECT_H
