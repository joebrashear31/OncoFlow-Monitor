#include "HistoryPanel.h"
#include <QHeaderView>
#include <QVBoxLayout>

HistoryPanel::HistoryPanel(QWidget *parent) : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_tree = new QTreeWidget;
    m_tree->setHeaderLabels({tr("Run ID"), tr("Study"), tr("Start Time"), tr("Status"), tr("Stage")});
    m_tree->setRootIsDecorated(false);
    m_tree->setAlternatingRowColors(true);
    m_tree->header()->setStretchLastSection(true);
    m_tree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    layout->addWidget(m_tree);

    connect(m_tree, &QTreeWidget::itemSelectionChanged, this, [this]() {
        auto items = m_tree->selectedItems();
        if (items.isEmpty()) return;
        int idx = items.first()->data(0, Qt::UserRole).toInt();
        if (idx >= 0 && idx < m_runs.size())
            emit runSelected(m_runs.at(idx));
    });
}

void HistoryPanel::setRuns(const QList<PipelineRun> &runs)
{
    m_runs = runs;
    m_tree->clear();
    for (const auto &r : runs)
        addRunItem(r);
}

void HistoryPanel::addRun(const PipelineRun &run)
{
    m_runs.append(run);
    addRunItem(run);
}

void HistoryPanel::addRunItem(const PipelineRun &run)
{
    auto *item = new QTreeWidgetItem(m_tree);
    item->setText(0, run.runId);
    item->setText(1, run.studyId);
    item->setText(2, run.startTime.toString("yyyy-MM-dd hh:mm:ss"));
    item->setText(3, run.status);
    item->setText(4, run.currentStage);
    item->setData(0, Qt::UserRole, m_runs.size() - 1);

    if (run.status == "Completed") item->setForeground(3, QColor(0, 150, 0));
    else if (run.status == "Failed") item->setForeground(3, Qt::red);
    else if (run.status == "Canceled") item->setForeground(3, QColor(200, 150, 0));
}
