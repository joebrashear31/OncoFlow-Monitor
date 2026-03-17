#pragma once

#include <QString>
#include <QDateTime>

struct PipelineRun {
    QString runId;
    QString studyId;
    QDateTime startTime;
    QDateTime endTime;
    QString status;       // "Running", "Completed", "Failed", "Canceled"
    QString currentStage;
    QString outputPath;
};
