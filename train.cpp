#include "train.h"
#include <QDebug>

#include <QDirIterator>

Train::Train()
{

    picPath.setPath("./data");
    picPath.setFilter(QDir::Dirs | QDir::NoSymLinks |QDir::NoDotAndDotDot);
    if(!picPath.exists())
    {
        qDebug() << "pic path not exsits!!";
        return;
    }
    qDebug() <<"picPath: "<< picPath;


}

Train::~Train()
{
}

void Train::run()
{

    picPath.refresh();
    for(unsigned int i = 0;i < picPath.count();i++)
    {
        //文件夹遍历，每个文件夹名的格式为num_name，
        //分别储存起来，一一对应，并将num作为label进行训练
        int picCnt = 0;
        QString dirName = picPath[i];
        qDebug() << "dir: " <<dirName;
        QStringList strList = dirName.split("_");
        //qDebug() << strList.at(0) << "---" <<strList.at(1);
        //qDebug() << "strCnt: " <<strList.count();

        //qDebug() << picPath.path()+"/"+dirName;


        QDirIterator picIt(picPath.path() + "/" + dirName);
        while(true)
        {
            //文件夹内的图片遍历，将所有bmp读取为mat后压入vector待训练
            //qDebug() << "picName: " << picIt.filePath();
            if(picIt.fileName().indexOf("bmp") != -1)
            {
                picCnt ++;
                qDebug() << "picPath: " << picIt.filePath();
                //qDebug() <<picIt.fileName().indexOf("bmp");
                label.push_back(i);
                Mat tmpMat = imread(picIt.filePath().toStdString(),0);
                Mat tmpMat2;
                resize(tmpMat,tmpMat2,Size(100,100));

                image.push_back(tmpMat2);
                qDebug() << i<<"-"<<tmpMat2.empty();
               // imshow(QString::number(i).toStdString(),tmpMat);
                qDebug() << tmpMat2.rows <<"x" <<tmpMat2.cols;
                tmpMat.release();
            }
            if(!picIt.hasNext()) break;
            picIt.next();


        }
        //qDebug() << "dir finished-------------" << i;
        QString tmpStr = QString("编号") + strList.at(0) + " 姓名" + strList.at(1);
        sendMsg(tmpStr);
        tmpStr = QString("共") + QString::number(picCnt) + "张";
        sendMsg(tmpStr);

    }
    double t = (double)cvGetTickCount();
    Ptr<FaceRecognizer> model = createEigenFaceRecognizer();
    model->train(image, label);
    t = (double)cvGetTickCount() - t;
    QString tmpStr = QString("训练完成，用时") +
            QString::number(t/((double)cvGetTickFrequency()*1000)) + "ms";
    sendMsg(tmpStr);
    //qDebug() << "train success in " << t/((double)cvGetTickFrequency()*1000) << "ms";
    model->save("./model.yml");
    this->quit();
}

void Train::sendMsg(QString msg1)
{
    currentTime = QDateTime::currentDateTime();
    QString timeStr = currentTime.toString("hh:mm:ss:zzz  ");
    msg = timeStr +  msg1;
    qDebug() << msg;

    emit msgSend(msg);
}
