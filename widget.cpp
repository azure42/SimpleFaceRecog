#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    detectThread = new Detect;
    collectThread = new Collect;
    trainThread = new Train;

    imgTimer = new QTimer(this);
    connect(imgTimer,SIGNAL(timeout()),this,SLOT(imgUpdateSlot()));
    connect(detectThread,SIGNAL(msgSend(QString)),this,SLOT(msgUpdateSlot(QString)));
    connect(detectThread,SIGNAL(nameLabelUpdate(QString)),this,SLOT(nameLabelUpdateSlot(QString)));

    connect(collectThread,SIGNAL(msgSend(QString)),this,SLOT(msgUpdateSlot(QString)));
    connect(trainThread,SIGNAL(msgSend(QString)),this,SLOT(msgUpdateSlot(QString)));
    connect(trainThread,SIGNAL(nameTableUpdate()),this,SLOT(listUpdateSlot()));

    //detectThread->start();
    imgTimer->start(300);
    mode = 0;//mode是为了区别界面刷新规则定义的，1是检测模式，2是录入模式

}

Widget::~Widget()
{
    delete ui;
    detectThread->exit();
    collectThread->exit();
    trainThread->exit();
    delete detectThread;
    delete collectThread;
    delete trainThread;
}

void Widget::imgUpdateSlot()
{
    QPixmap img;
    static QString str;
    switch(mode)
    {
    case 1 :
    {
        ui->mainImg->setPixmap(img.fromImage(detectThread->mainImg));
        ui->faceImg->setPixmap(img.fromImage(detectThread->faceImg));
        //if(!QString::compare(str))
    }break;
    case 2 :
    {
        ui->mainImg->setPixmap(img.fromImage(collectThread->mainImg));
        ui->faceImg->setPixmap(img.fromImage(collectThread->faceImg));
    }break;
    default:break;
    }
}

void Widget::msgUpdateSlot(QString msg)
{
    ui->resultView->addItem(msg);
    ui->resultView->scrollToBottom();
}

void Widget::listUpdateSlot()
{
    qDebug() << "进入listupdate";
    QDir picPath;
    picPath.setPath("./data");
    picPath.setFilter(QDir::Dirs | QDir::NoSymLinks |QDir::NoDotAndDotDot);
    if(!picPath.exists())
    {
        qDebug() << "pic path not exsits!!";
    }
//    QTableWidgetItem *newItem0 = new QTableWidgetItem(QString("hello"));
//    ui->nameTable->setItem(0,0,newItem0);

    for(unsigned int i = 0;i < picPath.count();i++)
    {
        QString dirName = picPath[i];
        QStringList strList = dirName.split("_");
        QTableWidgetItem *newItem0 = new QTableWidgetItem(strList.at(0));
        ui->nameTable->setItem(i,0,newItem0);
        QTableWidgetItem *newItem1 = new QTableWidgetItem(strList.at(1));
        ui->nameTable->setItem(i,1,newItem1);
    }
}

void Widget::nameLabelUpdateSlot(QString name)
{
    ui->nameLabel->setText(name);
}

void Widget::on_detectButton_clicked()
{
    mode = 1;
    //detectThread = new Detect;
    detectThread->start();

}

void Widget::on_saveButton_clicked()
{
    mode = 2;
    if(ui->lineEdit->text().isEmpty())
    {
        QMessageBox::warning(this,tr("错误"),tr("姓名为空"),QMessageBox::Ok);
    }
    else
    {
        if(collectThread->oldName == ui->lineEdit->text())
        {
            QMessageBox::warning(this,tr("错误"),tr("姓名已存在"),QMessageBox::Ok);
        }
        else
        {
            collectThread->oldName = collectThread->name;
            collectThread->name = ui->lineEdit->text();
            //recThread = new recThread;
            collectThread->start();
        }
    }

}


void Widget::on_trainButton_clicked()
{
    mode = 3;
    trainThread->start();
}

void Widget::on_recogButton_clicked()
{

}

void Widget::on_delButton_clicked()
{
    //TODO：将当前行对应的用户目录删除，重新进行训练。删除非空文件夹是个麻烦事儿
//    QDir picPath;
//    picPath.setPath("./data");
//    picPath.setFilter(QDir::Dirs | QDir::NoSymLinks |QDir::NoDotAndDotDot);
//    for(unsigned int i = 0;i < picPath.count();i++)
//    {
//        QString dirName = picPath[i];
//        QStringList strList = dirName.split("_");
//        if(ui->nameTable->itemAt(0,currentRow())->text() == strList.at(0));

//    }

}
