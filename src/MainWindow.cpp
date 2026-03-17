#include "MainWindow.h"
#include "services/StudyService.h"
#include "widgets/StudyListWidget.h"

#include <QApplication>
#include <QCoreApplication>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QSplitter>
#include <QTabWidget>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("OncoFlow Monitor");
    resize(1280, 800);
    setMinimumSize(900, 600);

    setupMenuBar();
    setupToolBar();
    setupCentralLayout();
    setupStatusBar();
    loadStudies();
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

    m_cancelAction = toolbar->addAction(tr("Cancel"));
    m_cancelAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Period));
    m_cancelAction->setToolTip(tr("Cancel the active pipeline run"));
    m_cancelAction->setEnabled(false);

    toolbar->addSeparator();

    m_reloadAction = toolbar->addAction(tr("Reload Studies"));
    m_reloadAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
    m_reloadAction->setToolTip(tr("Reload mock studies from disk"));
    connect(m_reloadAction, &QAction::triggered, this, &MainWindow::onReloadStudies);
}

void MainWindow::setupCentralLayout()
{
    // --- Left panel: Study Selection ---
    auto *studyGroup = new QGroupBox(tr("Study Selection"));
    auto *studyLayout = new QVBoxLayout(studyGroup);
    m_studyListWidget = new StudyListWidget;
    studyLayout->addWidget(m_studyListWidget);
    studyGroup->setMinimumWidth(220);

    connect(m_studyListWidget, &StudyListWidget::studySelected, this, [this](const Study &s) {
        statusBar()->showMessage(tr("Selected: %1").arg(s.studyId), 3000);
    });

    // --- Middle panel: Pipeline Configuration ---
    auto *configGroup = new QGroupBox(tr("Pipeline Configuration"));
    auto *configLayout = new QVBoxLayout(configGroup);
    m_configPanel = new QLabel(tr("Configuration form will appear here."));
    static_cast<QLabel *>(m_configPanel)->setAlignment(Qt::AlignCenter);
    configLayout->addWidget(m_configPanel);

    // --- Right panel: Run Status ---
    auto *statusGroup = new QGroupBox(tr("Run Status"));
    auto *statusLayout = new QVBoxLayout(statusGroup);
    m_statusPanel = new QLabel(tr("Pipeline status will appear here."));
    static_cast<QLabel *>(m_statusPanel)->setAlignment(Qt::AlignCenter);
    statusLayout->addWidget(m_statusPanel);
    statusGroup->setMinimumWidth(260);

    // Top row: study | config | status
    auto *topSplitter = new QSplitter(Qt::Horizontal);
    topSplitter->addWidget(studyGroup);
    topSplitter->addWidget(configGroup);
    topSplitter->addWidget(statusGroup);
    topSplitter->setStretchFactor(0, 1);
    topSplitter->setStretchFactor(1, 2);
    topSplitter->setStretchFactor(2, 1);

    // --- Bottom area: Logs + History as tabs ---
    auto *bottomTabs = new QTabWidget;

    auto *logGroup = new QWidget;
    auto *logLayout = new QVBoxLayout(logGroup);
    m_logPanel = new QLabel(tr("Live logs will appear here."));
    static_cast<QLabel *>(m_logPanel)->setAlignment(Qt::AlignCenter);
    logLayout->addWidget(m_logPanel);
    bottomTabs->addTab(logGroup, tr("Logs"));

    auto *historyGroup = new QWidget;
    auto *historyLayout = new QVBoxLayout(historyGroup);
    m_historyPanel = new QLabel(tr("Run history will appear here."));
    static_cast<QLabel *>(m_historyPanel)->setAlignment(Qt::AlignCenter);
    historyLayout->addWidget(m_historyPanel);
    bottomTabs->addTab(historyGroup, tr("Run History"));

    bottomTabs->setMinimumHeight(180);

    // Main vertical splitter: top row | bottom tabs
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

void MainWindow::loadStudies()
{
    StudyService service;
    QString path = QCoreApplication::applicationDirPath() + "/assets/mock_studies.json";
    auto studies = service.loadStudies(path);
    m_studyListWidget->setStudies(studies);
    statusBar()->showMessage(tr("Loaded %1 studies.").arg(studies.size()), 3000);
}

void MainWindow::onReloadStudies()
{
    loadStudies();
}
