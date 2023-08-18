#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QtDebug>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include "database.h"

namespace  {


}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("标记点管理");
    const QSqlError error = data::prepareMarkNode();
    if(error.isValid())
    {
        qDebug() << error;
    }
    if(QSqlTableModel *model = new QSqlTableModel(this))
    {
        model->setTable("marknodes");
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

