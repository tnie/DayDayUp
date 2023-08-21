#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QtDebug>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include "database.h"

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
            model->setHeaderData(0, Qt::Horizontal, tr("代号"));
            model->setHeaderData(1, Qt::Horizontal, tr("纬度"));
            model->setHeaderData(2, Qt::Horizontal, tr("经度"));
            // TODO 魔术数字；调换列的顺序；修改内容就出错
            constexpr int nameIdx = 11;
            model->setHeaderData(nameIdx, Qt::Horizontal, tr("航路点名称"));
            ui->writableView->setModel(model);
            //NOTE 只能关联整张表格，可以在 view 中隐藏不关心的列
            for (int i = 3; i < model->columnCount(); ++i) {
                ui->writableView->hideColumn(i);
            }
            ui->writableView->showColumn(nameIdx);
            ui->writableView->show();
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

