#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    bcm2835_gpio_fsel(rampPIN, BCM2835_GPIO_FSEL_OUTP);
    connect(timer_update,SIGNAL(timeout()),this,SLOT(update()));
    timer_update->start(50);

}
void MainWindow::update(){
    an0 = adda.ReadChannel(0);
    ui->spinBox_an0->setValue(an0);

    if (ui->checkBox_adda->isChecked()){
        ui->verticalSlider_da0->setValue(an0);
    }
    if (ui->checkBox_addaBOTH->isChecked()){
        ui->verticalSlider_da0->setValue(an0);
        ui->verticalSlider_da1->setValue(an0);
    }
    if (ui->checkBox_ramp->isChecked()){
        bcm2835_gpio_write(rampPIN,HIGH);
    }else{
        bcm2835_gpio_write(rampPIN,LOW);
    }
    adda.SetDAC0(ui->spinBox_da0->value());
    adda.SetDAC1(ui->spinBox_da1->value());

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_2_clicked()
{
    close();
}
