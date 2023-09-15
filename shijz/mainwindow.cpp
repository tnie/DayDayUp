#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QtDebug>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include "database.h"
#include <QMessageBox>
#include <QElapsedTimer>
#include <QSqlRecord>
#include <QPainter>
#include <QProgressBar>
#include <QTimer>

constexpr int _COUNT_  = 1000*10;     //数据库插入多少条

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , longitudeDelegate_(true)
    , latitudeDelegate_(false)
{
    ui->setupUi(this);
    this->setWindowTitle(QString("标记点管理 [pid %1]").arg(qApp->applicationPid()));
    const QSqlError error = data::prepareMarkNode();
    if(error.isValid())
    {
        qCritical() << error;
    }
    initGui();
//    usingTableModel();  // fail
//    queryNodes(1);  // empty table
//    insertNodes();
//    queryNodes(2);  // print
//    usingQueryModel();
//    ui->centralwidget->hide();
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    QTimer::singleShot(50, this, SLOT(insertNodes()));
}

void MainWindow::usingTableModel()
{
    if(QSqlTableModel *model = new QSqlTableModel(this))
    {
//        TODO 能不能用达梦作为 model 的源
        model->setTable("marknodes");
        if(model->lastError().isValid())
        {
            qCritical() << model->lastError();
        }
//        model->setEditStrategy(QSqlTableModel::OnFieldChange);
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
            qCritical() << model->lastError();
        }
    }
}

void MainWindow::usingQueryModel()
{
    if(QSqlQueryModel *model = new QSqlQueryModel(this))
    {
//        NOTE Index 是 SQL 中的保留关键词，用作列名需要双引号；
//        另一种方式 CONCAT(Latitude, Longitude)
        model->setQuery(R"(SELECT "Index", Latitude, Longitude, Name FROM marknodes;)");
        if(model->lastError().isValid())
        {
            qCritical() << model->lastError();
        }
        else
        {
            model->setHeaderData(0, Qt::Horizontal, tr("代号"));
            model->setHeaderData(1, Qt::Horizontal, tr("纬度"));
            model->setHeaderData(2, Qt::Horizontal, tr("经度"));
            model->setHeaderData(3, Qt::Horizontal, tr("航路点名称"));

            ui->tableView->setModel(model);
            ui->tableView->setItemDelegateForColumn(1, &latitudeDelegate_);
            ui->tableView->setItemDelegateForColumn(2, &longitudeDelegate_);
            ui->tableView->show();

//            while (model->canFetchMore()) {
//                model->fetchMore();
//            }
        }
    }
}

void addNode(QSqlQuery &q, double latitude, double longitude)
{
    q.addBindValue(latitude);
    q.addBindValue(longitude);
    q.exec();
}

//参考 Qt 示例 books
void MainWindow::insertNodes()
{
    QSqlQuery query;
    if (!query.prepare("insert into marknodes(latitude, longitude) values(?, ?)"))
    {
        const QString title = QString("prepare [pid %1]").arg(qApp->applicationPid());
        QMessageBox::critical(this, title, query.lastError().text());
        return;
    }
    QElapsedTimer timer;
    for (int i = 0; i< _COUNT_; ++i)
    {
        timer.start();
        addNode(query, i, i);
        qDebug("addNode(%d) cost %lld ms", i, timer.elapsed());
        if(query.lastError().isValid())
        {
            const QString title = QString("exec [pid %1]").arg(qApp->applicationPid());
            qCritical() << title << query.lastError();
            ui->textBrowser->append(query.lastError().text());
//            QMessageBox::critical(this, title, query.lastError().text());
//            return;
        }
        else
        {
            ui->progressBar->setValue(i+1);
        }
    }

}

void MainWindow::queryNodes(int step)
{
    static QSqlQuery query;
    if(1 == step)
    {
        query.setForwardOnly(true);
        qDebug() << "isForwardOnly() " << query.isForwardOnly();
        if(!query.exec("select latitude, longitude from marknodes"))
        {
            const QString title = QString("select [pid %1]").arg(qApp->applicationPid());
            QMessageBox::critical(this, title, query.lastError().text());
            return;
        }

        qDebug() << "isForwardOnly() " << query.isForwardOnly();
        qDebug() << "isSelect() " << query.isSelect();
    //    An active QSqlQuery is one that has been exec()'d successfully but not yet finished with.
        qDebug() << "isActive() " << query.isActive();
        qDebug() << "size()= " << query.size();     // -1
    }
    else if( 2 == step)
    {
        qDebug() << "2. isActive() " << query.isActive();
        while (query.next()) {
            QSqlRecord r = query.record();
            qDebug() << r;
        }
    }
}

QPoint MainWindow::mapping(double latitude, double longitude) const
{
    const int height = this->size().height();
    int y = latitude * height / 180 + height / 2;
    const int width = this->size().width();
    int x = longitude * width / 360 + width / 2;
    return QPoint(x, y);
}

void MainWindow::initGui()
{
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(_COUNT_);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnSubmit_clicked()
{
    QAbstractItemModel * m = ui->writableView->model();
    if(QSqlTableModel *model = static_cast<QSqlTableModel*>(m))
    {
        if(!model->isDirty())
        {
            QMessageBox::information(this, "提交", "没有修改任何内容");
            return;
        }
        if(false == model->submitAll())
        {
            QMessageBox::critical(this, "提交", model->lastError().text());
        }
        else
        {
            QMessageBox::information(this, "提交", "提交成功");
        }
    }
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    return;
    QAbstractItemModel * m = ui->tableView->model();
    if(QSqlQueryModel *model = static_cast<QSqlQueryModel*>(m))
    {
        QPainter painter(this);
        painter.drawEllipse(QPoint(), 5, 5);    // 左上角留痕
        for (int row = 0; row < model->rowCount(); ++row) {
            const QSqlRecord record = model->record(row);
            const double latitude = record.value("Latitude").toDouble();
            const double longitude = record.value("Longitude").toDouble();
            QPoint p = mapping(latitude, longitude);
            painter.drawEllipse(p, 5, 5);
        }
    }
}
