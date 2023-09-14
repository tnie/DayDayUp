#include "database.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>

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
//            此文件通过可视化工具已经初始化 marknodes 表，并预先存储有 4 条数据
        }
    }
    return db.lastError();
}
