#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QtDebug>
#include <QSqlQueryModel>
#include <QSqlTableModel>

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
    if(QSqlQueryModel *model = new QSqlQueryModel(this))
    {
        model->setQuery("SELECT Name, City FROM Persons");
        if(model->lastError().isValid())
        {
            qDebug() << model->lastError();
        }
        else
        {
            model->setHeaderData(0, Qt::Horizontal, tr("Name"));
            model->setHeaderData(1, Qt::Horizontal, tr("City"));

            ui->tableView->setModel(model);
            ui->tableView->show();
        }
    }
    if(QSqlTableModel *model = new QSqlTableModel(this))
    {
        model->setTable("Persons");
        model->setEditStrategy(QSqlTableModel::OnFieldChange);
        if(model->select())
        {
            model->setHeaderData(0, Qt::Horizontal, tr("ID"));
            model->setHeaderData(1, Qt::Horizontal, tr("Name"));
//NOTE 只能关联整张表格，可以在 view 中隐藏不关心的列
            ui->writableView->setModel(model);
            ui->writableView->hideColumn(0); // don't show the ID
            ui->writableView->show();
            ui->writableView2->setModel(model);
            ui->writableView2->hideColumn(0);
            ui->writableView2->hideColumn(2);
            ui->writableView2->show();
        }
        else
        {
            qDebug() << model->lastError();
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

