#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMouseEvent>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

#include <QDebug>
#include <QDateTime>

#define MYLOG qDebug() << "[" << __FILE__ << ":" << __LINE__ << ":" << __func__ << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz")<< "]"

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    MYLOG << event;
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    MYLOG << event;
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    MYLOG << event;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    MYLOG << event;
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    MYLOG << event;
}
