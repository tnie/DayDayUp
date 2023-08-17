#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QtDebug>

namespace  {
    QSqlError prepare()
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        if(db.isValid())
        {
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

}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("shijz 报告");
    const QSqlError error = prepare();
    if(error.isValid())
    {
        qDebug() << error;
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

