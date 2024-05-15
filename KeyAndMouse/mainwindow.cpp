#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMouseEvent>
#include <QKeyEvent>

#include <QDebug>
#include <QDateTime>

namespace  {
static MainWindow * self= nullptr;
void MyMessageHandler(QtMsgType t, const QMessageLogContext & c, const QString & l)
{
    if(self)
    {
        self->appendMessage(t, c, l);
    }
}
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/icon/KM.ico"));
    const QString h = QString("%1 %2").arg(__DATE__).arg(__TIME__);
    qDebug() << h;
    ui->textBrowser->append(h);
    self = this;
    handler = qInstallMessageHandler(MyMessageHandler);
}

MainWindow::~MainWindow()
{
    self = nullptr;
    delete ui;
}

void MainWindow::appendMessage(QtMsgType t, const QMessageLogContext & c, const QString & l)
{
    QString text = l;
    text.replace("PressEvent", R"(<font color="#FF0000">PressEvent</font>)");
    ui->textBrowser->append(text);
    if(handler)
    {
        handler(t, c, l);
    }
}

//#define MYLOG qDebug() << "[" << __FILE__ << ":" << __LINE__ << ":" << __func__ << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz")<< "]"
#define MYLOG qDebug() << "[" << __func__ << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz")<< "]"

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
