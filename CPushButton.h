#ifndef CPUSHBUTTON_H
#define CPUSHBUTTON_H

#include <QPushButton>
#include <QGraphicsView>

class CPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit CPushButton(QWidget *parent = nullptr);

signals:


    // QWidget interface
protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // CPUSHBUTTON_H
