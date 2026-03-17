#include "PipelineController.h"
#include "PipelineWorker.h"

PipelineController::PipelineController(QObject *parent) : QObject(parent) {}

PipelineController::~PipelineController()
{
    if (m_running) cancel();
    cleanup();
}

bool PipelineController::isRunning() const { return m_running; }

void PipelineController::start(const QString &studyId, const PipelineConfig &config)
{
    if (m_running) return;

    QString runId = QString("RUN_%1").arg(++m_runCounter, 4, 10, QChar('0'));

    m_thread = new QThread;
    m_worker = new PipelineWorker(runId, studyId, config);
    m_worker->moveToThread(m_thread);

    connect(m_thread, &QThread::started, m_worker, &PipelineWorker::run);
    connect(m_worker, &PipelineWorker::stageStarted, this, &PipelineController::stageStarted);
    connect(m_worker, &PipelineWorker::stageCompleted, this, &PipelineController::stageCompleted);
    connect(m_worker, &PipelineWorker::progressUpdated, this, &PipelineController::progressUpdated);
    connect(m_worker, &PipelineWorker::logGenerated, this, &PipelineController::logGenerated);
    connect(m_worker, &PipelineWorker::runFinished, this, &PipelineController::onRunFinished);
    connect(m_worker, &PipelineWorker::runFailed, this, &PipelineController::onRunFailed);
    connect(m_worker, &PipelineWorker::runCanceled, this, &PipelineController::onRunCanceled);

    m_running = true;
    emit runningChanged(true);
    m_thread->start();
}

void PipelineController::cancel()
{
    if (!m_running || !m_worker) return;
    m_worker->requestCancel();
}

void PipelineController::onRunFinished(const PipelineRun &run)
{
    m_running = false;
    emit runFinished(run);
    emit runningChanged(false);
    cleanup();
}

void PipelineController::onRunFailed(const QString &reason)
{
    m_running = false;
    emit runFailed(reason);
    emit runningChanged(false);
    cleanup();
}

void PipelineController::onRunCanceled()
{
    m_running = false;
    emit runCanceled();
    emit runningChanged(false);
    cleanup();
}

void PipelineController::cleanup()
{
    if (m_thread) {
        m_thread->quit();
        m_thread->wait();
        delete m_worker;
        delete m_thread;
        m_worker = nullptr;
        m_thread = nullptr;
    }
}
