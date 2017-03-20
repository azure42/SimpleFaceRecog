#ifndef WIDGET_H
#define WIDGET_H

#include "stable.h"
#include <QWidget>
#include "detect.h"
#include "recognize.h"
#include <QTimer>
#include <QDebug>
#include <QString>
#include<QMessageBox>


namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    Ui::Widget *ui;
    Detect *detectThread;
    Recognize *recThread;
    QTimer *imgTimer;
    int mode;

public slots:
    void imgUpdate();
    void msgUpdate(QString msg);
private slots:
    void on_detectButton_clicked();
    void on_saveButton_clicked();
};

#endif // WIDGET_H
