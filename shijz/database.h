#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlError>

namespace data {
    QSqlError prepare();
    QSqlError prepareMarkNode();
}

#endif // DATABASE_H
