#include "detect.h"

Detect::Detect()
{
    qDebug() << "detect";

    timer = new QTimer(this);

    //    string inputName;
    if(!cascade.load(CascadePATH))
        qDebug() << "cascade 1 load faild!";
    if(!nestedCascade.load(NestedCascadePATH))
        qDebug() << "cascade 2 load faild!";
    timer->start(100);
}

Detect::~Detect()
{
    capture.release();
}

void Detect::run()
{
    capture.open(0);
    if( capture.isOpened() )
    {
        sendMsg(QString("摄像头初始化成功"));
    }
    else
    {
        sendMsg(QString("摄像头初始化失败"));
    }

    picPath.setPath("./data");
    picPath.setFilter(QDir::Dirs | QDir::NoSymLinks |QDir::NoDotAndDotDot);
    if(!picPath.exists())
    {
        qDebug() << "pic path not exsits!!";
    }

    //从文件夹名得到名字和id的对应关系，存入hashmap，便于查询
    for(unsigned int i = 0;i < picPath.count();i++)
    {
        QString dirName = picPath[i];
        QStringList strList = dirName.split("_");
        QString tmp = strList.at(0);
        int num = tmp.toInt();
        nameTable.insert(num,strList.at(1));
    }

    Ptr<FaceRecognizer> model = createEigenFaceRecognizer();
    model->load("./model.yml");
    int predictedLabel = -1;
    double confidence = 0.0;
    for(;;)
    {
    capture >> frame;

    Mat frame1 = frame.clone();
    detectAndDraw( frame1, cascade, nestedCascade, scale, tryflip );
    mainImg = cvMat2QImage(frame1);
    if(!faceMat.empty())
        model->predict(faceMat, predictedLabel, confidence);

    if(confidence >= 70)
    {
        sendMsg(QString("识别为： ")+nameTable[predictedLabel]+"  置信率："+QString::number(confidence));
        emit(nameLabelUpdate(nameTable[predictedLabel]));
       //break;
    }
    }
}


QImage Detect::cvMat2QImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {
        qDebug() << "CV_8UC4";
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}


void Detect::detectAndDraw( Mat& img, CascadeClassifier& cascade,
                    CascadeClassifier& nestedCascade,
                    double scale, bool tryflip )
{
    double t = 0;
    vector<Rect> faces, faces2;
    const static Scalar colors[] =
    {
        Scalar(255,0,0),
        Scalar(255,128,0),
        Scalar(255,255,0),
        Scalar(0,255,0),
        Scalar(0,128,255),
        Scalar(0,255,255),
        Scalar(0,0,255),
        Scalar(255,0,255)
    };
    Mat gray, smallImg;

    cvtColor( img, gray, COLOR_BGR2GRAY );
    double fx = 1 / scale;
    resize( gray, smallImg, Size(), fx, fx, INTER_LINEAR );
    equalizeHist( smallImg, smallImg );

    t = (double)cvGetTickCount();
    cascade.detectMultiScale( smallImg, faces,
        1.1, 2, 0
        //|CASCADE_FIND_BIGGEST_OBJECT
        //|CASCADE_DO_ROUGH_SEARCH
        |CASCADE_SCALE_IMAGE,
        Size(30, 30) );
    if( tryflip )
    {
        flip(smallImg, smallImg, 1);
        cascade.detectMultiScale( smallImg, faces2,
                                 1.1, 2, 0
                                 //|CASCADE_FIND_BIGGEST_OBJECT
                                 //|CASCADE_DO_ROUGH_SEARCH
                                 |CASCADE_SCALE_IMAGE,
                                 Size(30, 30) );
        for( vector<Rect>::const_iterator r = faces2.begin(); r != faces2.end(); r++ )
        {
            faces.push_back(Rect(smallImg.cols - r->x - r->width, r->y, r->width, r->height));
        }
    }
    t = (double)cvGetTickCount() - t;
   // printf( "detection time = %g ms\\n", t/((double)cvGetTickFrequency()*1000.) );
    for ( size_t i = 0; i < faces.size(); i++ )
    {
        Rect r = faces[i];
        Mat smallImgROI;
        vector<Rect> nestedObjects;
        Point center;
        Scalar color = colors[i%8];
        int radius;

        double aspect_ratio = (double)r.width/r.height;
        if( 0.75 < aspect_ratio && aspect_ratio < 1.3 )
        {
            center.x = cvRound((r.x + r.width*0.5)*scale);
            center.y = cvRound((r.y + r.height*0.5)*scale);
            radius = cvRound((r.width + r.height)*0.25*scale);
            circle( img, center, radius, color, 3, 8, 0 );
        }
        else
            rectangle( img, cvPoint(cvRound(r.x*scale), cvRound(r.y*scale)),
                       cvPoint(cvRound((r.x + r.width-1)*scale), cvRound((r.y + r.height-1)*scale)),
                       color, 3, 8, 0);
        if( nestedCascade.empty() )
            continue;
        smallImgROI = smallImg( r );
        nestedCascade.detectMultiScale( smallImgROI, nestedObjects,
            1.1, 2, 0
            //|CASCADE_FIND_BIGGEST_OBJECT
            //|CASCADE_DO_ROUGH_SEARCH
            //|CASCADE_DO_CANNY_PRUNING
            |CASCADE_SCALE_IMAGE,
            Size(30, 30) );
        for ( size_t j = 0; j < nestedObjects.size(); j++ )
        {
            Rect nr = nestedObjects[j];
            center.x = cvRound((r.x + nr.x + nr.width*0.5)*scale);
            center.y = cvRound((r.y + nr.y + nr.height*0.5)*scale);
            radius = cvRound((nr.width + nr.height)*0.25*scale);
            circle( img, center, radius, color, 3, 8, 0 );
            if(j == 1)
            {


            //qDebug() << nestedObjects.size();
            QString tmpStr = QString::number(t/((double)cvGetTickFrequency()*1000));
            //sendMsg(QString("检测到人脸，耗时 ") + tmpStr + " ms");
            resize(smallImgROI,faceMat,Size(100,100));
            faceImg = cvMat2QImage(faceMat);
            }

      //
        }
//        Mat cutImg = getRectSubPix(smallImgROI,Size(nestedObjects[0].wid))

    }
    //imshow( "result", img );
}

void Detect::sendMsg(QString msg1)
{
    currentTime = QDateTime::currentDateTime();
    QString timeStr = currentTime.toString("hh:mm:ss:zzz  ");
    msg = timeStr +  msg1;

    emit msgSend(msg);
}


