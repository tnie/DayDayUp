#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileSystemModel>

//摸索 Qt 的 Model/View 机制
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QFileSystemModel * model = new QFileSystemModel(this);
    model->setRootPath(QDir::currentPath());
    ui->treeView->setModel(model);
    ui->listView->setModel(model);
    ui->tableView->setModel(model);
    // listView 槽在 .ui 中的关联
    connect(ui->treeView, SIGNAL(clicked(QModelIndex)), ui->tableView, SLOT(setRootIndex(QModelIndex)));
    // 默认值 0x0a QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed
    QListView::EditTriggers aEditTriggers = ui->listView->editTriggers();
}

MainWindow::~MainWindow()
{
    delete ui;
}

