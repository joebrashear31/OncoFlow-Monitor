#include "PipelineWorker.h"
#include <QThread>
#include <QRandomGenerator>

PipelineWorker::PipelineWorker(const QString &runId, const QString &studyId,
                               const PipelineConfig &config, QObject *parent)
    : QObject(parent), m_runId(runId), m_studyId(studyId), m_config(config)
{
    m_stages = {"Ingestion", "Preprocessing", "Segmentation", "Registration",
                "Metrics Generation", "Validation", "Packaging Complete"};
}

void PipelineWorker::run()
{
    emitLog("Info", QString("Pipeline started for study %1 [run %2]").arg(m_studyId, m_runId));
    emitLog("Info", QString("Model: %1, Registration: %2, Threshold: %3")
            .arg(m_config.modelVersion, m_config.registrationMode)
            .arg(m_config.threshold, 0, 'f', 2));

    QDateTime startTime = QDateTime::currentDateTime();
    int totalStages = m_stages.size();

    for (int i = 0; i < totalStages; ++i) {
        if (isCanceled()) {
            emitLog("Warning", "Pipeline canceled by user.");
            emit runCanceled();
            return;
        }

        const QString &stage = m_stages[i];
        emit stageStarted(stage);
        emitLog("Info", QString("Stage started: %1").arg(stage));

        // Simulate work: 1-3 seconds in 100ms steps
        int durationMs = QRandomGenerator::global()->bounded(1000, 3000);
        int steps = durationMs / 100;
        for (int s = 0; s < steps; ++s) {
            if (isCanceled()) {
                emitLog("Warning", "Pipeline canceled by user.");
                emit runCanceled();
                return;
            }
            QThread::msleep(100);
        }

        // Random chance of a warning
        if (QRandomGenerator::global()->bounded(100) < 15) {
            emitLog("Warning", QString("Minor anomaly detected during %1").arg(stage));
        }

        emit stageCompleted(stage);
        emitLog("Info", QString("Stage completed: %1").arg(stage));
        emit progressUpdated(static_cast<int>(((i + 1) * 100) / totalStages));
    }

    PipelineRun result;
    result.runId = m_runId;
    result.studyId = m_studyId;
    result.startTime = startTime;
    result.endTime = QDateTime::currentDateTime();
    result.status = "Completed";
    result.currentStage = m_stages.last();
    result.outputPath = m_config.outputDir + "/" + m_runId;

    emitLog("Info", QString("Pipeline completed successfully in %1s")
            .arg(result.startTime.secsTo(result.endTime)));
    emit runFinished(result);
}

void PipelineWorker::emitLog(const QString &severity, const QString &message)
{
    emit logGenerated({QDateTime::currentDateTime(), severity, message, m_runId});
}

bool PipelineWorker::isCanceled() const
{
    return m_cancelFlag.loadRelaxed() != 0;
}

void PipelineWorker::requestCancel()
{
    m_cancelFlag.storeRelaxed(1);
}
