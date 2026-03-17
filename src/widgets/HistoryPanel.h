#pragma once

#include "models/PipelineRun.h"
#include <QWidget>
#include <QTreeWidget>

class HistoryPanel : public QWidget {
    Q_OBJECT
public:
    explicit HistoryPanel(QWidget *parent = nullptr);
    void setRuns(const QList<PipelineRun> &runs);
    void addRun(const PipelineRun &run);

signals:
    void runSelected(const PipelineRun &run);

private:
    void addRunItem(const PipelineRun &run);
    QTreeWidget *m_tree;
    QList<PipelineRun> m_runs;
};
