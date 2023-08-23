#ifndef GEODELEGATE_H
#define GEODELEGATE_H

#include <QStyledItemDelegate>

class GeoDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    GeoDelegate(bool isLongitude, QObject *parent = nullptr);

    // QAbstractItemDelegate interface
public:
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    // QStyledItemDelegate interface
public:
    virtual QString displayText(const QVariant &value, const QLocale &locale) const override;

private:
    const bool isLongitude_; // true 经度；false 纬度
};

#endif // GEODELEGATE_H
