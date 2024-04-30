#include <QCoreApplication>
#include <QEvent>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QEvent e(QEvent::User);
//    a.sendEvent(&a, &e);
    a.postEvent(&a, new QEvent(QEvent::User));
    return a.exec();
}
