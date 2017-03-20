#ifndef STABLE_H
#define STABLE_H

#include <QWidget>
#include <QObject>
#include <QThread>
#include <QDebug>
#include <QTimer>
#include <QImage>
#include <QString>
#include <QDateTime>
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/face/facerec.hpp"
#include "opencv2/face.hpp"

#ifdef WIN32
    #define CascadePATH "E:/haarcascade_frontalface_alt.xml"
    #define NestedCascadePATH "E:/haarcascade_frontalface_alt.xml"
    #define DataPATH "E:/data/"
    #define modelPATH "E:/eigenfaces_at.yml"
#else //不是windows 也可以#elif defined(linux)

#endif

#endif // STABLE_H
