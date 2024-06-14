#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <QThread>

class WorkerThread : public QThread
{
    Q_OBJECT
public:
    WorkerThread(QObject* parent = nullptr);;
    void run() override;
signals:
    void resultReady(const QString &s);
};

class MyObject: public QObject
{
    Q_OBJECT
public:
    void startWorkInAThread();
public slots:
     void handleResults(const QString &);
};


#endif // WORKERTHREAD_H
