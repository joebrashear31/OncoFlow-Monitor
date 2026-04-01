#include "MainWindow.h"
#include "services/HistoryService.h"
#include "services/PipelineController.h"
#include "services/StudyService.h"
#include "services/ValidationService.h"
#include "widgets/ConfigPanel.h"
#include "widgets/LogPanel.h"
#include "widgets/StatusPanel.h"
#include "widgets/HistoryPanel.h"
#include "widgets/RunSummaryDialog.h"
#include "widgets/StudyListWidget.h"

#include <QApplication>
#include <QCoreApplication>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QSplitter>
#include <QTabWidget>
#include <QVBoxLayout>

// Window with all the widgets and controls for the OncoFlow Monitor application. It manages the main UI layout, connects signals and slots, and coordinates interactions between the different components.
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("OncoFlow Monitor");
    resize(1280, 800);
    setMinimumSize(900, 600);

    m_pipelineController = new PipelineController(this);
    QString histDir = QCoreApplication::applicationDirPath() + "/data/history";
    m_historyService = new HistoryService(histDir);

    setupMenuBar();
    setupToolBar();
    setupCentralLayout();
    setupStatusBar();
    connectPipeline();
    loadStudies();
    m_historyPanel->setRuns(m_historyService->loadHistory());
    m_configPanel->setPresetsDir(QCoreApplication::applicationDirPath() + "/data/presets");
}

void MainWindow::setupMenuBar()
{
    auto *fileMenu = menuBar()->addMenu(tr("&File"));
    auto *reloadAction = fileMenu->addAction(tr("&Reload Studies"));
    connect(reloadAction, &QAction::triggered, this, &MainWindow::onReloadStudies);
    fileMenu->addSeparator();
    auto *exitAction = fileMenu->addAction(tr("E&xit"));
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, qApp, &QApplication::quit);

    auto *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("&About"), this, [this]() {
        QMessageBox::about(this, tr("About OncoFlow Monitor"),
            tr("OncoFlow Monitor v1.0\n\n"
               "A desktop application for configuring and monitoring\n"
               "a simulated medical imaging processing pipeline."));
    });
}

void MainWindow::setupToolBar()
{
    auto *toolbar = addToolBar(tr("Main Toolbar"));
    toolbar->setMovable(false);
    toolbar->setIconSize(QSize(20, 20));

    m_runAction = toolbar->addAction(tr("Run Pipeline"));
    m_runAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));
    m_runAction->setToolTip(tr("Launch the pipeline for the selected study"));
    connect(m_runAction, &QAction::triggered, this, &MainWindow::onRunPipeline);

    m_cancelAction = toolbar->addAction(tr("Cancel"));
    m_cancelAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Period));
    m_cancelAction->setToolTip(tr("Cancel the active pipeline run"));
    m_cancelAction->setEnabled(false);
    connect(m_cancelAction, &QAction::triggered, this, &MainWindow::onCancelPipeline);

    toolbar->addSeparator();

    m_reloadAction = toolbar->addAction(tr("Reload Studies"));
    m_reloadAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
    m_reloadAction->setToolTip(tr("Reload mock studies from disk"));
    connect(m_reloadAction, &QAction::triggered, this, &MainWindow::onReloadStudies);
}

void MainWindow::setupCentralLayout()
{
    // Left: Study Selection
    auto *studyGroup = new QGroupBox(tr("Study Selection"));
    auto *studyLayout = new QVBoxLayout(studyGroup);
    m_studyListWidget = new StudyListWidget;
    studyLayout->addWidget(m_studyListWidget);
    studyGroup->setMinimumWidth(220);

    connect(m_studyListWidget, &StudyListWidget::studySelected, this, [this](const Study &s) {
        statusBar()->showMessage(tr("Selected: %1").arg(s.studyId), 3000);
    });

    // Middle: Config
    auto *configGroup = new QGroupBox(tr("Pipeline Configuration"));
    auto *configLayout = new QVBoxLayout(configGroup);
    m_configPanel = new ConfigPanel;
    configLayout->addWidget(m_configPanel);

    // Right: Status
    auto *statusGroup = new QGroupBox(tr("Run Status"));
    auto *statusLayout = new QVBoxLayout(statusGroup);
    m_statusPanel = new StatusPanel;
    statusLayout->addWidget(m_statusPanel);
    statusGroup->setMinimumWidth(260);

    auto *topSplitter = new QSplitter(Qt::Horizontal);
    topSplitter->addWidget(studyGroup);
    topSplitter->addWidget(configGroup);
    topSplitter->addWidget(statusGroup);
    topSplitter->setStretchFactor(0, 1);
    topSplitter->setStretchFactor(1, 2);
    topSplitter->setStretchFactor(2, 1);

    // Bottom: Logs + History tabs
    auto *bottomTabs = new QTabWidget;
    m_logPanel = new LogPanel;
    bottomTabs->addTab(m_logPanel, tr("Logs"));

    m_historyPanel = new HistoryPanel;
    connect(m_historyPanel, &HistoryPanel::runSelected, this, [this](const PipelineRun &run) {
        auto *dlg = new RunSummaryDialog(run, this);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->show();
    });
    bottomTabs->addTab(m_historyPanel, tr("Run History"));
    bottomTabs->setMinimumHeight(180);

    auto *mainSplitter = new QSplitter(Qt::Vertical);
    mainSplitter->addWidget(topSplitter);
    mainSplitter->addWidget(bottomTabs);
    mainSplitter->setStretchFactor(0, 3);
    mainSplitter->setStretchFactor(1, 1);
    setCentralWidget(mainSplitter);
}

void MainWindow::setupStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::connectPipeline()
{
    connect(m_pipelineController, &PipelineController::runningChanged, this, &MainWindow::onRunningChanged);
    connect(m_pipelineController, &PipelineController::stageStarted, this, &MainWindow::onStageStarted);
    connect(m_pipelineController, &PipelineController::stageCompleted, this, &MainWindow::onStageCompleted);
    connect(m_pipelineController, &PipelineController::progressUpdated, m_statusPanel, &StatusPanel::setProgress);
    connect(m_pipelineController, &PipelineController::logGenerated, m_logPanel, &LogPanel::appendLog);
    connect(m_pipelineController, &PipelineController::runFinished, this, [this](const PipelineRun &run) {
        m_historyService->saveRun(run);
        m_historyPanel->addRun(run);
        onRunFinished();
        auto *dlg = new RunSummaryDialog(run, this);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->show();
    });
    connect(m_pipelineController, &PipelineController::runCanceled, this, &MainWindow::onRunCanceled);
}

void MainWindow::loadStudies()
{
    StudyService service;
    QString path = QCoreApplication::applicationDirPath() + "/assets/mock_studies.json";
    auto studies = service.loadStudies(path);
    m_studyListWidget->setStudies(studies);
    statusBar()->showMessage(tr("Loaded %1 studies.").arg(studies.size()), 3000);
}

void MainWindow::onRunPipeline()
{
    Study study = m_studyListWidget->hasSelection() ? m_studyListWidget->selectedStudy() : Study{};
    PipelineConfig config = m_configPanel->currentConfig();

    ValidationService validator;
    QStringList errors = validator.validate(study, config);
    if (!errors.isEmpty()) {
        QMessageBox::warning(this, tr("Validation Failed"), errors.join("\n"));
        return;
    }

    m_statusPanel->reset();
    m_statusPanel->setStudyId(study.studyId);
    m_statusPanel->setRunStatus("Running");
    m_statusPanel->startTimer();
    m_logPanel->clearLogs();

    m_currentStudyId = study.studyId;
    m_currentRunStart = QDateTime::currentDateTime();
    m_pipelineController->start(study.studyId, config);
}

void MainWindow::onCancelPipeline()
{
    m_pipelineController->cancel();
}

void MainWindow::onRunningChanged(bool running)
{
    m_runAction->setEnabled(!running);
    m_cancelAction->setEnabled(running);
}

void MainWindow::onStageStarted(const QString &stage)
{
    m_statusPanel->setCurrentStage(stage);
    m_statusPanel->setStageState(stage, "Active");
}

void MainWindow::onStageCompleted(const QString &stage)
{
    m_statusPanel->setStageState(stage, "Completed");
}

void MainWindow::onRunFinished()
{
    m_statusPanel->stopTimer();
    m_statusPanel->setRunStatus("Completed");
    statusBar()->showMessage(tr("Pipeline completed successfully."), 5000);
}

void MainWindow::onRunCanceled()
{
    m_statusPanel->stopTimer();
    m_statusPanel->setRunStatus("Canceled");
    statusBar()->showMessage(tr("Pipeline canceled."), 5000);

    PipelineRun canceledRun;
    canceledRun.runId = QString("RUN_CANCELED_%1").arg(QDateTime::currentDateTime().toString("hhmmss"));
    canceledRun.studyId = m_currentStudyId;
    canceledRun.startTime = m_currentRunStart;
    canceledRun.endTime = QDateTime::currentDateTime();
    canceledRun.status = "Canceled";
    canceledRun.currentStage = "Canceled";
    m_historyService->saveRun(canceledRun);
    m_historyPanel->addRun(canceledRun);
}

void MainWindow::onReloadStudies()
{
    loadStudies();
}
