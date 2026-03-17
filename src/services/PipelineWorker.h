#pragma once

#include "models/LogEntry.h"
#include "models/PipelineConfig.h"
#include "models/PipelineRun.h"
#include <QObject>
#include <QAtomicInt>

class PipelineWorker : public QObject {
    Q_OBJECT
public:
    explicit PipelineWorker(const QString &runId, const QString &studyId,
                            const PipelineConfig &config, QObject *parent = nullptr);

    void requestCancel();

public slots:
    void run();

signals:
    void stageStarted(const QString &stage);
    void stageCompleted(const QString &stage);
    void progressUpdated(int percent);
    void logGenerated(const LogEntry &entry);
    void runFinished(const PipelineRun &run);
    void runFailed(const QString &reason);
    void runCanceled();

private:
    void emitLog(const QString &severity, const QString &message);
    bool isCanceled() const;

    QString m_runId;
    QString m_studyId;
    PipelineConfig m_config;
    QAtomicInt m_cancelFlag{0};
    QStringList m_stages;
};
