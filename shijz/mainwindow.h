#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "geodelegate.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnSubmit_clicked();
    void insertNodes();

private:
    Ui::MainWindow *ui;
    GeoDelegate longitudeDelegate_;
    GeoDelegate latitudeDelegate_;
    void usingTableModel();
    void usingQueryModel();
    void queryNodes(int step);
    QPoint mapping(double latitude, double longitude) const;
    void initGui();

    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent *event) override;

    // QWidget interface
protected:
    virtual void showEvent(QShowEvent *event) override;
};

#endif // MAINWINDOW_H
