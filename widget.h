#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "detect.h"
#include <QTimer>
#include <QDebug>
#include <QString>
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
    QTimer *imgTimer;

public slots:
    void imgUpdate();
    void msgUpdate();
};

#endif // WIDGET_H
