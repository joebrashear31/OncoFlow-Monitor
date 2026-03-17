#include "StudyService.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

QList<Study> StudyService::loadStudies(const QString &jsonPath)
{
    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly)) {
        return fallbackStudies();
    }

    QJsonParseError err;
    auto doc = QJsonDocument::fromJson(file.readAll(), &err);
    if (err.error != QJsonParseError::NoError || !doc.isArray()) {
        return fallbackStudies();
    }

    QList<Study> studies;
    for (const auto &val : doc.array()) {
        auto obj = val.toObject();
        Study s;
        s.studyId = obj["studyId"].toString();
        s.patientAlias = obj["patientAlias"].toString();
        s.modality = obj["modality"].toString();
        s.acquisitionDate = QDate::fromString(obj["acquisitionDate"].toString(), Qt::ISODate);
        s.filePath = obj["filePath"].toString();
        s.status = obj["status"].toString();
        studies.append(s);
    }
    return studies;
}

QList<Study> StudyService::fallbackStudies() const
{
    return {
        {"CT_LUNG_001", "PATIENT_A", "CT", QDate(2026, 3, 1), "mock/ct_lung_001", "Ready"},
        {"CT_BRAIN_014", "PATIENT_B", "CT", QDate(2026, 3, 3), "mock/ct_brain_014", "Ready"},
        {"PET_CT_007", "PATIENT_C", "PET/CT", QDate(2026, 3, 5), "mock/pet_ct_007", "Ready"},
    };
}
