#include "CPushButton.h"
#include <QDebug>
#include <QMouseEvent>
#include <QMessageBox>

CPushButton::CPushButton(QWidget *parent) : QPushButton(parent)
{
    connect(this, &QPushButton::clicked, [this](){
        QMessageBox::information(this, "提示", "按钮被按下");
    });
}


void CPushButton::mousePressEvent(QMouseEvent *event)
{
    qDebug() << __FUNCTION__ << this->metaObject()->className() << event;
    QPushButton::mousePressEvent(event);
}




void CPushButton::mouseMoveEvent(QMouseEvent *event)
{
    qDebug() << __FUNCTION__  << this->metaObject()->className()<< event;
    QPushButton::mouseMoveEvent(event);
}
