#ifndef WIDGET_H
#define WIDGET_H

#include "stable.h"
#include <QWidget>
#include <QTimer>
#include <QDebug>
#include <QString>
#include<QMessageBox>

#include "detect.h"
#include "collect.h"
#include "train.h"

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
    Collect *collectThread;
    Train *trainThread;
    QTimer *imgTimer;
    int mode;

public slots:
    void imgUpdateSlot();
    void msgUpdateSlot(QString msg);
    void listUpdateSlot();
    void nameLabelUpdateSlot(QString name);
private slots:
    void on_detectButton_clicked();
    void on_saveButton_clicked();
    void on_trainButton_clicked();
    void on_recogButton_clicked();

    void on_delButton_clicked();
};

#endif // WIDGET_H
