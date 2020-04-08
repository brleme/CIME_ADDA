#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QtWidgets/QMainWindow>
#include <QtGui>
#include <QMainWindow>
#include "adda.h"
#include "bcm2835.h"
#define  rampPIN  RPI_BPLUS_GPIO_J8_40


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QTimer * timer_update = new QTimer(this);
    int an0,an1,an2,an3;
    ADDA adda;


private slots:
    void on_pushButton_2_clicked();
    void update();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
