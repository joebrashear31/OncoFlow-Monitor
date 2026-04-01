// TDD tests for RunSummaryDialog — written RED first, then greened.

#include <QtTest/QtTest>
#include <QDialog>
#include <QLabel>
#include "widgets/RunSummaryDialog.h"
#include "models/PipelineRun.h"
#include "TestHelpers.h"

class tst_RunSummaryDialog : public QObject
{
    Q_OBJECT

private slots:

    // ----------------------------------------------------------------
    // Construction with various run statuses
    // ----------------------------------------------------------------

    void construction_completedRun_nocrash()
    {
        PipelineRun run = makePipelineRun("RUN_0001", "Completed");
        RunSummaryDialog dlg(run);
        QVERIFY(true);
    }

    void construction_failedRun_nocrash()
    {
        PipelineRun run = makePipelineRun("RUN_0002", "Failed");
        RunSummaryDialog dlg(run);
        QVERIFY(true);
    }

    void construction_canceledRun_nocrash()
    {
        PipelineRun run = makePipelineRun("RUN_0003", "Canceled");
        RunSummaryDialog dlg(run);
        QVERIFY(true);
    }

    void construction_isADialog()
    {
        PipelineRun run = makePipelineRun("RUN_0001", "Completed");
        RunSummaryDialog dlg(run);
        QVERIFY(qobject_cast<QDialog*>(&dlg) != nullptr);
    }

    // ----------------------------------------------------------------
    // Content verification — runId and studyId appear somewhere in labels
    // ----------------------------------------------------------------

    void content_runIdAppearsInDialog()
    {
        PipelineRun run = makePipelineRun("RUN_FINDME", "Completed");
        RunSummaryDialog dlg(run);
        bool found = false;
        for (auto *l : dlg.findChildren<QLabel*>()) {
            if (l->text().contains("RUN_FINDME")) { found = true; break; }
        }
        QVERIFY(found);
    }

    void content_studyIdAppearsInDialog()
    {
        PipelineRun run = makePipelineRun("RUN_0001", "Completed");
        run.studyId = "STUDY_FINDME";
        RunSummaryDialog dlg(run);
        bool found = false;
        for (auto *l : dlg.findChildren<QLabel*>()) {
            if (l->text().contains("STUDY_FINDME")) { found = true; break; }
        }
        QVERIFY(found);
    }

    void content_statusAppearsInDialog()
    {
        PipelineRun run = makePipelineRun("RUN_0001", "Completed");
        RunSummaryDialog dlg(run);
        bool found = false;
        for (auto *l : dlg.findChildren<QLabel*>()) {
            if (l->text().contains("Completed")) { found = true; break; }
        }
        QVERIFY(found);
    }

    void content_outputPathAppearsForCompletedRun()
    {
        PipelineRun run = makePipelineRun("RUN_0001", "Completed");
        run.outputPath = "./output/RUN_0001";
        RunSummaryDialog dlg(run);
        bool found = false;
        for (auto *l : dlg.findChildren<QLabel*>()) {
            if (l->text().contains("./output/RUN_0001")) { found = true; break; }
        }
        QVERIFY(found);
    }

    // ----------------------------------------------------------------
    // Parent widget relationship
    // ----------------------------------------------------------------

    void construction_withParent_nocrash()
    {
        QWidget parent;
        PipelineRun run = makePipelineRun("RUN_0001", "Completed");
        RunSummaryDialog dlg(run, &parent);
        QVERIFY(true);
    }
};

QTEST_MAIN(tst_RunSummaryDialog)
#include "tst_RunSummaryDialog.moc"
