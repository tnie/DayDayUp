#include <QCoreApplication>
#include <QEvent>
#include <QtDebug>
#include "test.h"

int main(int argc, char *argv[])
{
    QMessageLogger(__FILE__, __LINE__, __FUNCSIG__).debug("Hello, kitty");
    QCoreApplication a(argc, argv);
    QEvent e(QEvent::User);
//    a.sendEvent(&a, &e);
    a.postEvent(&a, new QEvent(QEvent::User));
    moc::test2();
    return a.exec();
}
