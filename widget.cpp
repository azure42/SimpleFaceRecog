#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    detectThread = new Detect;
    recThread = new Recognize;

    imgTimer = new QTimer(this);
    connect(imgTimer,SIGNAL(timeout()),this,SLOT(imgUpdate()));
    connect(detectThread,SIGNAL(msgSend(QString)),this,SLOT(msgUpdate(QString)));
    connect(recThread,SIGNAL(msgSend(QString)),this,SLOT(msgUpdate(QString)));

    //detectThread->start();
    imgTimer->start(300);
    mode = 0;//mode是为了区别界面刷新规则定义的，1是检测模式，2是识别模式

}

Widget::~Widget()
{
    delete ui;
    detectThread->exit();
    recThread->exit();
    delete detectThread;
    delete recThread;
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
        ui->mainImg->setPixmap(img.fromImage(recThread->mainImg));
        ui->faceImg->setPixmap(img.fromImage(recThread->faceImg));
    }break;
    default:break;
    }
}

void Widget::msgUpdate(QString msg)
{
    switch(mode)
    {
    case 1:
    {
        ui->resultView->addItem(msg);
        ui->resultView->scrollToBottom();
    }
    case 2:
    {
        ui->resultView->addItem(msg);
        ui->resultView->scrollToBottom();
    }
    }

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
        if(recThread->oldName == ui->lineEdit->text())
        {
            QMessageBox::warning(this,tr("错误"),tr("姓名已存在"),QMessageBox::Ok);
        }
        else
        {
            recThread->oldName = recThread->name;
            recThread->name = ui->lineEdit->text();
            //recThread = new recThread;
            recThread->start();
        }
    }

}

