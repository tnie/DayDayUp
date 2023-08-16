#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QTimer>
#include <QPixmap>

int MainWindow::time_ = 0;

//摸索 Qt 的隐式共享
void test1()
{
    QPixmap p1, p2;
    p1.load("image.bmp");
//    隐式共享
    p2 = p1;                        // p1 and p2 share data

    QPainter paint;
//    写时拷贝，难道拷贝操作是放到 QPainter 中实现的吗？
    paint.begin(&p2);               // cuts p2 loose from p1
    paint.drawText(0,50, "Hi");
    paint.end();

}

//摸索 Qt 的绘制过程
void MainWindow::paint()
{
    qDebug("%s", __FUNCTION__);


    QPainter painter(pix);
    QPen pen;
    pen.setColor(Qt::white);
    painter.setPen(pen);
    painter.drawEllipse(0,0,this->width(), this->height());
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    test1();
    pix = new QPixmap(this->width()/2, this->height()/2);
    QTimer::singleShot(1000*5, this, SLOT(paint()));
}

void MainWindow::paintEvent(QPaintEvent *event)
{
//    return ;
    if(time_ < 10)
    {
        paint();
        qDebug("%d: %s", time_, __FUNCTION__);
        ++time_;
    }
    QPainter painter(this);
    painter.drawPixmap(0,0, this->width()/3, this->height()/3, *pix);
}


MainWindow::~MainWindow()
{
    delete ui;
}

