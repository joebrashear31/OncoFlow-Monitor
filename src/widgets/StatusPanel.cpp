#include "StatusPanel.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHeaderView>

const QStringList &StatusPanel::stageNames()
{
    static const QStringList names = {
        "Ingestion", "Preprocessing", "Segmentation", "Registration",
        "Metrics Generation", "Validation", "Packaging Complete"
    };
    return names;
}

StatusPanel::StatusPanel(QWidget *parent) : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);

    auto *form = new QFormLayout;
    m_studyIdLabel = new QLabel(tr("—"));
    m_statusLabel = new QLabel(tr("Idle"));
    m_stageLabel = new QLabel(tr("—"));
    m_elapsedLabel = new QLabel(tr("0s"));
    form->addRow(tr("Study:"), m_studyIdLabel);
    form->addRow(tr("Status:"), m_statusLabel);
    form->addRow(tr("Stage:"), m_stageLabel);
    form->addRow(tr("Elapsed:"), m_elapsedLabel);
    layout->addLayout(form);

    m_progressBar = new QProgressBar;
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    layout->addWidget(m_progressBar);

    m_stageTree = new QTreeWidget;
    m_stageTree->setHeaderLabels({tr("Stage"), tr("State")});
    m_stageTree->setRootIsDecorated(false);
    m_stageTree->header()->setStretchLastSection(true);
    for (const auto &name : stageNames()) {
        auto *item = new QTreeWidgetItem(m_stageTree, {name, tr("Pending")});
        item->setForeground(1, Qt::gray);
    }
    layout->addWidget(m_stageTree);

    m_tickTimer = new QTimer(this);
    connect(m_tickTimer, &QTimer::timeout, this, [this]() {
        qint64 secs = m_elapsed.elapsed() / 1000;
        m_elapsedLabel->setText(QString("%1s").arg(secs));
    });
}

void StatusPanel::setStudyId(const QString &id) { m_studyIdLabel->setText(id); }
void StatusPanel::setRunStatus(const QString &status) { m_statusLabel->setText(status); }
void StatusPanel::setCurrentStage(const QString &stage) { m_stageLabel->setText(stage); }
void StatusPanel::setProgress(int percent) { m_progressBar->setValue(percent); }

void StatusPanel::setStageState(const QString &stage, const QString &state)
{
    auto *item = stageItem(stage);
    if (!item) return;
    item->setText(1, state);

    QColor color = Qt::gray;
    if (state == "Active") color = QColor(0, 120, 215);
    else if (state == "Completed") color = QColor(0, 150, 0);
    else if (state == "Failed") color = Qt::red;
    item->setForeground(1, color);
}

void StatusPanel::startTimer()
{
    m_elapsed.start();
    m_tickTimer->start(500);
}

void StatusPanel::stopTimer()
{
    m_tickTimer->stop();
    qint64 secs = m_elapsed.elapsed() / 1000;
    m_elapsedLabel->setText(QString("%1s").arg(secs));
}

void StatusPanel::reset()
{
    m_studyIdLabel->setText(tr("—"));
    m_statusLabel->setText(tr("Idle"));
    m_stageLabel->setText(tr("—"));
    m_elapsedLabel->setText(tr("0s"));
    m_progressBar->setValue(0);
    m_tickTimer->stop();
    for (int i = 0; i < m_stageTree->topLevelItemCount(); ++i) {
        auto *item = m_stageTree->topLevelItem(i);
        item->setText(1, tr("Pending"));
        item->setForeground(1, Qt::gray);
    }
}

QTreeWidgetItem *StatusPanel::stageItem(const QString &stage) const
{
    for (int i = 0; i < m_stageTree->topLevelItemCount(); ++i) {
        if (m_stageTree->topLevelItem(i)->text(0) == stage)
            return m_stageTree->topLevelItem(i);
    }
    return nullptr;
}
