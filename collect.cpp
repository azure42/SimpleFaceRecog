#include "collect.h"
#define PICCNT 30
/*
 *录入若干人脸图片，并储存在num_name命名的文件夹内。
 * num自增，name在ui中录入
 */
Collect::Collect()
{
    qDebug() << "recognize";
    if(!cascade.load(CascadePATH))
        qDebug() << "cascade 1 load faild!";
    else
        qDebug() <<"cascade 1 load success!";
    if(!nestedCascade.load(NestedCascadePATH))
        qDebug() << "cascade 2 load faild";
    else
        qDebug() <<"cascade 1 load success!";

    faceCnt = 0;
    oldName = QString("N/A");

}

Collect::~Collect()
{
    capture.release();
}


void Collect::run()
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


    dataDir.setPath(DataPATH);//文件夹命名：num_name
    dataDir.setFilter(QDir::Dirs | QDir::NoSymLinks |QDir::NoDotAndDotDot);

    nameCnt = dataDir.count() ;//统计现有数据文件夹个数，减去的两个是.和..
    qDebug() << "nameCnt: " << nameCnt;
    savePath.setPath(dataDir.absolutePath() + "/" + QString::number(nameCnt) + "_" +name);

    sendMsg(name+"的人脸图片数据将存储到");
    sendMsg(savePath.path());
    //申请单个人脸识别数据存储文件夹
    if(savePath.exists(savePath.path()))
    {
        qDebug() << "存在";
        if(DeleteDirectory(savePath.path()))
        {
            qDebug() << "delete1";
            sendMsg(QString("目录已存在，清空数据成功"));
        }
        else
        {
            qDebug() << "delete2";
            sendMsg(QString("目录已存在，清空数据失败"));
        }
    }
    else
    {
        sendMsg(name+"目录不存在，尝试新建");
    }
    if(savePath.mkpath(savePath.path()))
    {
        qDebug() << "新文件夹";
        sendMsg(QString("新建图片存储目录成功"));
    }
    else
        sendMsg(QString("新建图片存储目录失败"));

    //捕获若干次人脸数据
    while(faceCnt < PICCNT)
    {
        capture >> frame;
        //if( frame.empty() )
        //break;

        Mat frame1 = frame.clone();
        detectAndSave( frame1, cascade, nestedCascade, scale, tryflip );
        mainImg = cvMat2QImage(frame1);
    }

    /*
    //开始对所有人脸数据进行训练
    double t = (double)cvGetTickCount();
    Ptr<FaceRecognizer> model = createEigenFaceRecognizer();
    model->train(images, labels);
    t = (double)cvGetTickCount() - t;
    qDebug() << "train success in " << t/((double)cvGetTickFrequency()*1000) << "ms";
    model->save(modelPATH);
    */

    faceCnt = 0;
    sendMsg(QString("--------------------------------"));
    sendMsg(name + "数据录入完成！");
    sendMsg(QString("--------------------------------"));

    this->quit();
}




void Collect::detectAndSave( Mat& img, CascadeClassifier& cascade,
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
    // printf( "detection time = %g ms\n", t/((double)cvGetTickFrequency()*1000.) );
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
            if(j == 1 && !smallImgROI.empty())
            //嵌套检测到较多的特征，可认为识别到人脸
            {
                faceCnt++;
                Mat tmpMat;
                //保存图片


               // cvtColor(smallImgROI,tmpMat,CV_BGR2GRAY);

                resize(smallImgROI,tmpMat,Size(100,100));
                //qDebug() << "channels1:" << smallImgROI.channels() << "channels2:" << img.channels();
                imwrite(format("%s/%d.bmp",savePath.path().toStdString().data(),faceCnt),tmpMat);
                //qDebug() << format("%s/%d.bmp",savePath.path().toStdString().data(),faceCnt);
                faceImg = cvMat2QImage(smallImgROI);
                images.push_back(tmpMat);
                labels.push_back(0);
                //qDebug() << nestedObjects.size();
                QString tmpStr = QString::number(t/((double)cvGetTickFrequency()*1000));
                sendMsg(QString::number(faceCnt)+QString("录入人脸，耗时 ") + tmpStr + " ms");
            }

            //
        }

    }
    //imshow( "result", img );
}

void Collect::sendMsg(QString msg1)
{
    currentTime = QDateTime::currentDateTime();
    QString timeStr = currentTime.toString("hh:mm:ss:zzz  ");
    msg = timeStr +  msg1;
    qDebug() << msg;

    emit msgSend(msg);
}

Mat Collect::norm_0_255(InputArray _src) {
    Mat src = _src.getMat();
    // Create and return normalized image:
    Mat dst;
    switch(src.channels()) {
    case 1:
        cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC1);
        break;
    case 3:
        cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC3);
        break;
    default:
        src.copyTo(dst);
        break;
    }
    return dst;
}




QImage Collect::cvMat2QImage(const cv::Mat& mat)
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

bool Collect::DeleteDirectory(const QString &path)
{
    if (path.isEmpty())
        return false;

    QDir dir(path);
    if(!dir.exists())
        return true;

    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();
    foreach (QFileInfo fi, fileList)
    {
        if (fi.isFile())
            fi.dir().remove(fi.fileName());
        else
            DeleteDirectory(fi.absoluteFilePath());
    }
    return dir.rmpath(dir.absolutePath());
}

