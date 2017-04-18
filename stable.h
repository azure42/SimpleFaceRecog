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
#include <QDir>
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/face/facerec.hpp"
#include "opencv2/face.hpp"

#ifdef WIN32
    #define CascadePATH "./haarcascade_frontalface_alt.xml"
    #define NestedCascadePATH "./haarcascade_eye_tree_eyeglasses.xml"
    #define DataPATH "./data/"
    #define modelPATH "./eigenfaces_at.yml"
#else //不是windows 也可以#elif defined(linux)
    #define CascadePATH "./haarcascade_frontalface_alt.xml"
    #define NestedCascadePATH "./haarcascade_frontalface_alt.xml"
    #define DataPATH "./data/"
    #define modelPATH "./eigenfaces_at.yml"
#endif

#endif // STABLE_H
