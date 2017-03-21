#ifndef RECOGNIZE_H
#define RECOGNIZE_H

#include "stable.h"
#include <QObject>
#include <QThread>
#include <QDebug>
#include <QTimer>
#include <QImage>
#include <QString>
#include <QDateTime>
#include <QDir>
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/face/facerec.hpp"
#include "opencv2/face.hpp"
using namespace std;
using namespace cv;
using namespace cv::face;

class Recognize : public QThread
{
    Q_OBJECT
public:
    Recognize();
    QString msg,name,oldName;
    QImage mainImg,faceImg;
    Mat norm_0_255(InputArray _src) ;

    ~Recognize();
private:
    VideoCapture capture;
    Mat frame;
    CascadeClassifier cascade, nestedCascade;
    void run();
    void sendMsg(QString msg1);
    void detectAndSave(Mat &img, CascadeClassifier &cascade, CascadeClassifier &nestedCascade, double scale, bool tryflip);
    QImage cvMat2QImage(const cv::Mat &mat);
    const bool tryflip = 0;
    const double scale = 1.3;
    QDateTime currentTime;
    vector<Mat> images;
    vector<int> labels;
    int faceCnt,nameCnt;
    QDir savePath,dataDir;



    bool DeleteDirectory(const QString &path);
signals:
    void msgSend(QString);


};

#endif // RECOGNIZE_H
