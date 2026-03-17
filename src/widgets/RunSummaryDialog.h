#pragma once

#include "models/PipelineRun.h"
#include <QDialog>

class RunSummaryDialog : public QDialog {
    Q_OBJECT
public:
    explicit RunSummaryDialog(const PipelineRun &run, QWidget *parent = nullptr);
};
