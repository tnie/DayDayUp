#include "geodelegate.h"
#include <QGeoCoordinate>
#include <QLineEdit>

GeoDelegate::GeoDelegate(bool isLongitude, QObject *parent )
    : QStyledItemDelegate(parent), isLongitude_(isLongitude)
{

}


void GeoDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    const double value = index.model()->data(index).toDouble();
    if(QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(editor))
    {
        const QString co = QGeoCoordinate(value, value).toString();
        lineEdit->setText(co);
    }
    else
    {

    }
}


QWidget *GeoDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QLineEdit *editor = new QLineEdit(parent);
    editor->setFrame(false);
    return editor;
}

void GeoDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit * lineEdit = static_cast<QLineEdit*>(editor);
    QString content = lineEdit->text();
    const double value = content.toDouble();
    model->setData(index, value);
}

void GeoDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}

QString GeoDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    bool ok = false;
    const double v = value.toDouble(&ok);
    if(ok)
    {
        QGeoCoordinate co(0, 0);
        isLongitude_ ?  co.setLongitude(v) : co.setLatitude(v);
        if(co.isValid())
        {
            const QString str = co.toString();
            const QString latitude = str.left(str.indexOf(','));
            const QString longitude = str.mid(str.indexOf(',') + 1);
            return isLongitude_ ? longitude : latitude;
        }
    }
    return QStyledItemDelegate::displayText(value, locale);
}
