#pragma once

#include <QMainWindow>
#include <QSplitter>
#include <QToolBar>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QAction>
#include <QWidget>

class StudyListWidget;
class ConfigPanel;
class StatusPanel;
class LogPanel;
class PipelineController;

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
    QWidget *m_historyPanel = nullptr;

    // Pipeline
    PipelineController *m_pipelineController = nullptr;

    // Toolbar actions
    QAction *m_runAction = nullptr;
    QAction *m_cancelAction = nullptr;
    QAction *m_reloadAction = nullptr;
};
