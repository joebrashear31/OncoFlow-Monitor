#pragma once

#include "models/Study.h"
#include "models/PipelineRun.h"
#include "models/LogEntry.h"

#include <QDate>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QMetaType>
#include <QString>

// Register custom types so QSignalSpy can capture them as QVariant arguments.
Q_DECLARE_METATYPE(Study)
Q_DECLARE_METATYPE(PipelineRun)
Q_DECLARE_METATYPE(LogEntry)

// Returns a fully populated Study for use in tests.
inline Study makeStudy(const QString &id = QStringLiteral("ST001"))
{
    Study s;
    s.studyId        = id;
    s.patientAlias   = QStringLiteral("PATIENT_TEST");
    s.modality       = QStringLiteral("CT");
    s.acquisitionDate = QDate(2026, 3, 15);
    s.filePath       = QStringLiteral("mock/") + id.toLower();
    s.status         = QStringLiteral("Ready");
    return s;
}

// Returns a fully populated PipelineRun for use in tests.
inline PipelineRun makePipelineRun(const QString &runId  = QStringLiteral("RUN_0001"),
                                   const QString &status = QStringLiteral("Completed"))
{
    PipelineRun r;
    r.runId        = runId;
    r.studyId      = QStringLiteral("ST001");
    r.startTime    = QDateTime(QDate(2026, 3, 15), QTime(10, 0, 0));
    r.endTime      = QDateTime(QDate(2026, 3, 15), QTime(10, 1, 30));
    r.status       = status;
    r.currentStage = QStringLiteral("Packaging Complete");
    r.outputPath   = QStringLiteral("./output/") + runId;
    return r;
}

// Writes doc to dir/name; returns true on success.
inline bool writeJsonFile(const QDir &dir, const QString &name, const QJsonDocument &doc)
{
    QFile f(dir.absoluteFilePath(name));
    if (!f.open(QIODevice::WriteOnly))
        return false;
    f.write(doc.toJson());
    return true;
}
