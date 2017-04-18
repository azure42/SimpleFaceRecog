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
    connect(imgTimer,SIGNAL(timeout()),this,SLOT(imgUpdate()));
    connect(detectThread,SIGNAL(msgSend(QString)),this,SLOT(msgUpdate(QString)));
    connect(collectThread,SIGNAL(msgSend(QString)),this,SLOT(msgUpdate(QString)));
    connect(trainThread,SIGNAL(msgSend(QString)),this,SLOT(msgUpdate(QString)));

    //detectThread->start();
    imgTimer->start(300);
    mode = 0;//mode是为了区别界面刷新规则定义的，1是检测模式，2是识别模式

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

void Widget::imgUpdate()
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

void Widget::msgUpdate(QString msg)
{
    ui->resultView->addItem(msg);
    ui->resultView->scrollToBottom();
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
