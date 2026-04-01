#pragma once 

#include <QString>
#include <QStringList>
#include <QImage>

class FrameSource
{
public:
    bool loadFromFolder(const QString &folderPath);
    int frameCount() const;
    QImage getFrame(int index) const;
    QString getFramePath(int index) const;
    bool isLoaded() const;
private:
    QStringList framePaths_m;
};

