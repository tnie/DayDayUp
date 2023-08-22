#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlError>

namespace data {
    QSqlError prepare();
    // true 使用本地的 sqlite； false 使用达梦数据库
    QSqlError prepareMarkNode(bool remote = false);
}

#endif // DATABASE_H
