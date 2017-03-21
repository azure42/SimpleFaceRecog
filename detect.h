#ifndef DETECT_H
#define DETECT_H

#include "stable.h"

#include <QThread>
#include <QDebug>
#include <QTimer>
#include <QImage>
#include <QString>
#include <QDateTime>
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

using namespace std;
using namespace cv;

class Detect  : public QThread
{
    Q_OBJECT
public:
    Detect();
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
    Mat frame;


    QImage cvMat2QImage(const cv::Mat& mat);
    QTimer *timer;

    void run();

    const bool tryflip = 0;
    CascadeClassifier cascade, nestedCascade;
    const double scale = 1.3;

    QDateTime currentTime;

signals:
    void msgSend(QString);


};

#endif // DETECT_H
