#pragma once

#include "models/Study.h"

#include <QList>
#include <QString>

class StudyService {
public:
    StudyService() = default;

    QList<Study> loadStudies(const QString &jsonPath);

private:
    QList<Study> fallbackStudies() const;
};
