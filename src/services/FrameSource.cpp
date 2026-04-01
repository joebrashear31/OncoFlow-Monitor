#include "FrameSource.h"

#include <QDir>
#include <QFileInfoList>    

bool FrameSource::loadFromFolder(const QString &folderPath)
{
    QDir dir(folderPath);
    if (!dir.exists()) return false;

    QStringList filters = {"*.png", "*.jpg", "*.jpeg", "*.bmp"};
    QFileInfoList fileInfoList = dir.entryInfoList(filters, QDir::Files, QDir::Name);
    framePaths_m.clear();
    for (const auto &fileInfo : fileInfoList)
        framePaths_m.append(fileInfo.absoluteFilePath());

    return !framePaths_m.isEmpty();
}

int FrameSource::frameCount() const
{
    return framePaths_m.size();
}  

QImage FrameSource::getFrame(int index) const
{
    if (index < 0 || index >= framePaths_m.size()) return QImage();
    return QImage(framePaths_m.at(index));
}

QString FrameSource::getFramePath(int index) const
{
    if (index < 0 || index >= framePaths_m.size()) return QString();
    return framePaths_m.at(index);
}

bool FrameSource::isLoaded() const
{
    return !framePaths_m.isEmpty();
}