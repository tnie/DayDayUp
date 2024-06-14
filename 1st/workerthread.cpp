#include "workerthread.h"
#include <QDebug>

void MyObject::startWorkInAThread()
{
    WorkerThread *workerThread = new WorkerThread(this);
    connect(workerThread, &WorkerThread::resultReady, this, &MyObject::handleResults);
    connect(workerThread, &WorkerThread::finished, workerThread, &QObject::deleteLater);
    qDebug() << __FUNCTION__ << QThread::currentThreadId();
    workerThread->start();
}

void MyObject::handleResults(const QString &)
{
    qDebug() << __FUNCTION__ << QThread::currentThreadId();
}

WorkerThread::WorkerThread(QObject *parent):
    QThread(parent) {}

void WorkerThread::run() {
    QString result;
    qDebug() << __FUNCTION__ << QThread::currentThreadId();
    /* ... here is the expensive or blocking operation ... */
    this->sleep(5);
    emit resultReady(result);
}
