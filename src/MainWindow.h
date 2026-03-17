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

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onReloadStudies();

private:
    void setupMenuBar();
    void setupToolBar();
    void setupCentralLayout();
    void setupStatusBar();
    void loadStudies();

    // Panels
    StudyListWidget *m_studyListWidget = nullptr;
    QWidget *m_configPanel = nullptr;
    QWidget *m_statusPanel = nullptr;
    QWidget *m_logPanel = nullptr;
    QWidget *m_historyPanel = nullptr;

    // Toolbar actions
    QAction *m_runAction = nullptr;
    QAction *m_cancelAction = nullptr;
    QAction *m_reloadAction = nullptr;
};
