#include "RunSummaryDialog.h"
#include <QDesktopServices>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QUrl>
#include <QVBoxLayout>

RunSummaryDialog::RunSummaryDialog(const PipelineRun &run, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Run Summary — %1").arg(run.runId));
    setMinimumWidth(400);

    auto *form = new QFormLayout;
    form->addRow(tr("Run ID:"), new QLabel(run.runId));
    form->addRow(tr("Study ID:"), new QLabel(run.studyId));
    form->addRow(tr("Status:"), new QLabel(run.status));
    form->addRow(tr("Start:"), new QLabel(run.startTime.toString("yyyy-MM-dd hh:mm:ss")));
    form->addRow(tr("End:"), new QLabel(run.endTime.toString("yyyy-MM-dd hh:mm:ss")));

    qint64 elapsed = run.startTime.secsTo(run.endTime);
    form->addRow(tr("Elapsed:"), new QLabel(QString("%1s").arg(elapsed)));
    form->addRow(tr("Stage:"), new QLabel(run.currentStage));
    form->addRow(tr("Output:"), new QLabel(run.outputPath));

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(form);

    auto *btnRow = new QHBoxLayout;
    if (run.status == "Completed") {
        auto *openBtn = new QPushButton(tr("Open Output Directory"));
        connect(openBtn, &QPushButton::clicked, this, [path = run.outputPath]() {
            QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        });
        btnRow->addWidget(openBtn);
    }
    auto *closeBtn = new QPushButton(tr("Close"));
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    btnRow->addStretch();
    btnRow->addWidget(closeBtn);
    layout->addLayout(btnRow);
}
