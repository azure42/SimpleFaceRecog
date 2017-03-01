#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    detectThread = new Detect;
    imgTimer = new QTimer(this);
    connect(imgTimer,SIGNAL(timeout()),this,SLOT(imgUpdate()));
    connect(detectThread,SIGNAL(msgSend()),this,SLOT(msgUpdate()));
    detectThread->start();
    imgTimer->start(300);

}

Widget::~Widget()
{
    delete ui;
}

void Widget::imgUpdate()
{
    QPixmap img;
    static QString str;
    ui->mainImg->setPixmap(img.fromImage(detectThread->mainImg));
    ui->faceImg->setPixmap(img.fromImage(detectThread->faceImg));
    //if(!QString::compare(str))
}

void Widget::msgUpdate()
{
    ui->resultView->addItem(detectThread->msg);
    ui->resultView->scrollToBottom();
}
