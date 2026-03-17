#pragma once

#include "models/LogEntry.h"
#include "models/PipelineConfig.h"
#include "models/PipelineRun.h"
#include <QObject>
#include <QThread>

class PipelineWorker;

class PipelineController : public QObject {
    Q_OBJECT
public:
    explicit PipelineController(QObject *parent = nullptr);
    ~PipelineController() override;

    bool isRunning() const;
    void start(const QString &studyId, const PipelineConfig &config);
    void cancel();

signals:
    void stageStarted(const QString &stage);
    void stageCompleted(const QString &stage);
    void progressUpdated(int percent);
    void logGenerated(const LogEntry &entry);
    void runFinished(const PipelineRun &run);
    void runFailed(const QString &reason);
    void runCanceled();
    void runningChanged(bool running);

private slots:
    void onRunFinished(const PipelineRun &run);
    void onRunFailed(const QString &reason);
    void onRunCanceled();

private:
    void cleanup();

    QThread *m_thread = nullptr;
    PipelineWorker *m_worker = nullptr;
    bool m_running = false;
    int m_runCounter = 0;
};
