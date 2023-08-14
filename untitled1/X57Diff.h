#ifndef X57DIFF_H
#define X57DIFF_H
#include <QDir>
#include <QMap>
#include <QDebug>
//摸索 Qt 的文件库
class X57Diff
{
    QMap<QString /*fileName*/, qint64 /*size*/> fileSizeMap;
    const QDir chartDir;
    void initSizeMap()
    {
        qDebug() << chartDir.absolutePath();
        const QFileInfoList fileInfoList = chartDir.entryInfoList(QStringList{"*.X57", "*.x57"}, QDir::Files | QDir::NoDotAndDotDot);
        for(int i=0; i<fileInfoList.size(); ++i)
        {
            const QFileInfo& fileInfo = fileInfoList[i];
            qDebug() << fileInfo.fileName() << fileInfo.size();
            fileSizeMap.insert(fileInfo.fileName(), fileInfo.size());
        }
    }
public:
    X57Diff(const QString& dir): chartDir(dir) {
        reload();
    }
    void reload()
    {
        fileSizeMap.clear();
        initSizeMap();
    }
    bool contains(const QString& fileName, qint64 size)
    {
        return fileSizeMap.value(fileName, -1) == size;
    }
    bool contains(const QFileInfo& fileInfo)
    {
        return contains(fileInfo.fileName(), fileInfo.size());
    }
};

#endif // X57DIFF_H
