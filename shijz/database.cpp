#include "database.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QSqlDriver>

QSqlError data::prepare()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    if(db.isValid())
    {
//            Unable to execute multiple statements at a time
        db.setDatabaseName("shijz.sqlite");
        if(bool ok = db.open())
        {
            const QString createTable=R"(
CREATE TABLE IF NOT EXISTS Persons
(
PersonID int PRIMARY KEY,
Name varchar(255),
Address varchar(255),
City varchar(255)
);
)";
            QSqlQuery create(createTable);
            if(create.isActive())
            {
                const QString insertItem=R"(
INSERT OR IGNORE INTO PERSONS (PersonID, Name, Address, City)
VALUES (1, 'niel', 'aha', 'Beijing');
)";
                QSqlQuery insert(insertItem);
                if(insert.isActive())
                {
                    QSqlQuery query("select Name from Persons;");
                    while (query.next()) {
                        const QString name = query.value(0).toString();
                        qDebug() << name;
                    }
                    return query.lastError();
                }
                return insert.lastError();
            }
            return create.lastError();
        }
    }
    return db.lastError();
}

QString driverFeatureName(QSqlDriver::DriverFeature f)
{
#ifndef _DriverFeatureName_
#define _DriverFeatureName_(x) \
    case QSqlDriver::DriverFeature::x: return #x;

    switch (f) {
      _DriverFeatureName_(Transactions);
      _DriverFeatureName_(QuerySize);
      _DriverFeatureName_(BLOB);
      _DriverFeatureName_(Unicode);
      _DriverFeatureName_(PreparedQueries);
      _DriverFeatureName_(NamedPlaceholders);
      _DriverFeatureName_(PositionalPlaceholders);
      _DriverFeatureName_(LastInsertId);
      _DriverFeatureName_(BatchOperations);
      _DriverFeatureName_(SimpleLocking);
      _DriverFeatureName_(LowPrecisionNumbers);
      _DriverFeatureName_(EventNotifications);
      _DriverFeatureName_(FinishQuery);
      _DriverFeatureName_(MultipleResultSets);
      _DriverFeatureName_(CancelQuery);
    default:
        return "unknow driver feature";
    }
#undef _DriverFeatureName_
#endif
}

QSqlError data::prepareMarkNode(bool remote /*= false*/)
{
//    用户需要保证 ODBC 驱动以及 k10 数据源等配置正确
    QSqlDatabase db = QSqlDatabase::addDatabase(remote ? "QODBC" : "QSQLITE");
    if(db.isValid())
    {
        //            Unable to execute multiple statements at a time
        db.setDatabaseName(remote ? "k10" : "dzht.sqlite");
        db.setConnectOptions("QSQLITE_BUSY_TIMEOUT=3000");
        if(bool ok = db.open())
        {
            qDebug() << db.connectionName() << "connectOptions: " << db.connectOptions();
            if(QSqlDriver *driver = db.driver())
            {
                for (int i=0; i<= QSqlDriver::CancelQuery; ++i)
                {
                    QSqlDriver::DriverFeature f = static_cast<QSqlDriver::DriverFeature>(i);
                    qDebug() << "hasFeature(" << driverFeatureName(f)
                             << ")" << driver->hasFeature(f);
                }

            }
//            此文件通过可视化工具已经初始化 marknodes 表，并预先存储有 4 条数据
        }
    }
    return db.lastError();
}
