#pragma once

#include <QMainWindow>
#include <QSplitter>
#include <QToolBar>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QAction>
#include <QDateTime>
#include <QWidget>

class StudyListWidget;
class ConfigPanel;
class StatusPanel;
class LogPanel;
class HistoryPanel;
class FrameViewer;
class PipelineController;
class HistoryService;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onReloadStudies();
    void onRunPipeline();
    void onCancelPipeline();
    void onRunningChanged(bool running);
    void onStageStarted(const QString &stage);
    void onStageCompleted(const QString &stage);
    void onRunFinished();
    void onRunCanceled();

private:
    void setupMenuBar();
    void setupToolBar();
    void setupCentralLayout();
    void setupStatusBar();
    void connectPipeline();
    void loadStudies();

    // Panels
    StudyListWidget *m_studyListWidget = nullptr;
    ConfigPanel *m_configPanel = nullptr;
    StatusPanel *m_statusPanel = nullptr;
    LogPanel *m_logPanel = nullptr;
    HistoryPanel *m_historyPanel = nullptr;
    FrameViewer *m_frameViewer = nullptr;

    // Pipeline
    PipelineController *m_pipelineController = nullptr;
    HistoryService *m_historyService = nullptr;

    // Current run tracking
    QString m_currentStudyId;
    QDateTime m_currentRunStart;

    // Toolbar actions
    QAction *m_runAction = nullptr;
    QAction *m_cancelAction = nullptr;
    QAction *m_reloadAction = nullptr;
};
