#pragma once

#include <QString>
#include <QDateTime>

struct Study {
    QString studyId;
    QString patientAlias;
    QString modality;
    QDate acquisitionDate;
    QString filePath;
    QString status;
};
