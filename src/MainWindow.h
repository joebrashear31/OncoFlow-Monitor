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

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onReloadStudies();
    void onRunPipeline();

private:
    void setupMenuBar();
    void setupToolBar();
    void setupCentralLayout();
    void setupStatusBar();
    void loadStudies();

    // Panels
    StudyListWidget *m_studyListWidget = nullptr;
    ConfigPanel *m_configPanel = nullptr;
    StatusPanel *m_statusPanel = nullptr;
    LogPanel *m_logPanel = nullptr;
    QWidget *m_historyPanel = nullptr;

    // Toolbar actions
    QAction *m_runAction = nullptr;
    QAction *m_cancelAction = nullptr;
    QAction *m_reloadAction = nullptr;
};
