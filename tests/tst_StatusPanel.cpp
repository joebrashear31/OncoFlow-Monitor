// TDD tests for StatusPanel — written RED first, then greened.

#include <QtTest/QtTest>
#include <QLabel>
#include <QProgressBar>
#include <QTreeWidget>
#include "widgets/StatusPanel.h"

class tst_StatusPanel : public QObject
{
    Q_OBJECT

private slots:

    // ----------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------

    void construction_nocrash()
    {
        StatusPanel panel;
        QVERIFY(true);
    }

    void construction_progressBarInitialValue_isZero()
    {
        StatusPanel panel;
        auto *pb = panel.findChild<QProgressBar*>();
        QVERIFY(pb != nullptr);
        QCOMPARE(pb->value(), 0);
    }

    void construction_stageTree_hasExpectedStageCount()
    {
        StatusPanel panel;
        auto *tree = panel.findChild<QTreeWidget*>();
        QVERIFY(tree != nullptr);
        // 7 pipeline stages: Ingestion, Preprocessing, Segmentation,
        // Registration, Metrics Generation, Validation, Packaging Complete
        QCOMPARE(tree->topLevelItemCount(), 7);
    }

    void construction_stageTree_allPending()
    {
        StatusPanel panel;
        auto *tree = panel.findChild<QTreeWidget*>();
        QVERIFY(tree != nullptr);
        for (int i = 0; i < tree->topLevelItemCount(); ++i) {
            QCOMPARE(tree->topLevelItem(i)->text(1), QString("Pending"));
        }
    }

    // ----------------------------------------------------------------
    // setStudyId
    // ----------------------------------------------------------------

    void setStudyId_reflectedInLabel()
    {
        StatusPanel panel;
        panel.setStudyId("CT_LUNG_001");
        bool found = false;
        for (auto *l : panel.findChildren<QLabel*>()) {
            if (l->text() == "CT_LUNG_001") { found = true; break; }
        }
        QVERIFY(found);
    }

    // ----------------------------------------------------------------
    // setRunStatus
    // ----------------------------------------------------------------

    void setRunStatus_reflectedInLabel()
    {
        StatusPanel panel;
        panel.setRunStatus("Running");
        bool found = false;
        for (auto *l : panel.findChildren<QLabel*>()) {
            if (l->text() == "Running") { found = true; break; }
        }
        QVERIFY(found);
    }

    void setRunStatus_multipleValues_nocrash()
    {
        StatusPanel panel;
        panel.setRunStatus("Running");
        panel.setRunStatus("Completed");
        panel.setRunStatus("Failed");
        panel.setRunStatus("Canceled");
        QVERIFY(true);
    }

    // ----------------------------------------------------------------
    // setCurrentStage
    // ----------------------------------------------------------------

    void setCurrentStage_reflectedInLabel()
    {
        StatusPanel panel;
        panel.setCurrentStage("Segmentation");
        bool found = false;
        for (auto *l : panel.findChildren<QLabel*>()) {
            if (l->text() == "Segmentation") { found = true; break; }
        }
        QVERIFY(found);
    }

    // ----------------------------------------------------------------
    // setProgress
    // ----------------------------------------------------------------

    void setProgress_valueUpdated()
    {
        StatusPanel panel;
        auto *pb = panel.findChild<QProgressBar*>();
        QVERIFY(pb != nullptr);
        panel.setProgress(50);
        QCOMPARE(pb->value(), 50);
    }

    void setProgress_boundaries()
    {
        StatusPanel panel;
        auto *pb = panel.findChild<QProgressBar*>();
        panel.setProgress(0);
        QCOMPARE(pb->value(), 0);
        panel.setProgress(100);
        QCOMPARE(pb->value(), 100);
    }

    // ----------------------------------------------------------------
    // setStageState
    // ----------------------------------------------------------------

    void setStageState_knownStage_updatesTreeItem()
    {
        StatusPanel panel;
        auto *tree = panel.findChild<QTreeWidget*>();
        QVERIFY(tree != nullptr);
        panel.setStageState("Ingestion", "Active");
        // Find the Ingestion item and verify text
        for (int i = 0; i < tree->topLevelItemCount(); ++i) {
            auto *item = tree->topLevelItem(i);
            if (item->text(0) == "Ingestion") {
                QCOMPARE(item->text(1), QString("Active"));
                return;
            }
        }
        QFAIL("Ingestion stage item not found");
    }

    void setStageState_completed_updatesItem()
    {
        StatusPanel panel;
        auto *tree = panel.findChild<QTreeWidget*>();
        panel.setStageState("Preprocessing", "Completed");
        for (int i = 0; i < tree->topLevelItemCount(); ++i) {
            auto *item = tree->topLevelItem(i);
            if (item->text(0) == "Preprocessing") {
                QCOMPARE(item->text(1), QString("Completed"));
                return;
            }
        }
        QFAIL("Preprocessing stage item not found");
    }

    void setStageState_unknownStage_nocrash()
    {
        StatusPanel panel;
        panel.setStageState("NonExistentStage", "Active");
        QVERIFY(true);
    }

    // ----------------------------------------------------------------
    // startTimer / stopTimer
    // ----------------------------------------------------------------

    void startStopTimer_nocrash()
    {
        StatusPanel panel;
        panel.startTimer();
        panel.stopTimer();
        QVERIFY(true);
    }

    // ----------------------------------------------------------------
    // reset
    // ----------------------------------------------------------------

    void reset_progressBarReturnsToZero()
    {
        StatusPanel panel;
        panel.setProgress(75);
        panel.reset();
        auto *pb = panel.findChild<QProgressBar*>();
        QCOMPARE(pb->value(), 0);
    }

    void reset_stageTree_allPending()
    {
        StatusPanel panel;
        panel.setStageState("Ingestion", "Completed");
        panel.setStageState("Preprocessing", "Active");
        panel.reset();
        auto *tree = panel.findChild<QTreeWidget*>();
        for (int i = 0; i < tree->topLevelItemCount(); ++i) {
            QCOMPARE(tree->topLevelItem(i)->text(1), QString("Pending"));
        }
    }

    void reset_statusLabel_showsIdle()
    {
        StatusPanel panel;
        panel.setRunStatus("Running");
        panel.reset();
        bool found = false;
        for (auto *l : panel.findChildren<QLabel*>()) {
            if (l->text() == "Idle") { found = true; break; }
        }
        QVERIFY(found);
    }

    // ----------------------------------------------------------------
    // Full workflow smoke test
    // ----------------------------------------------------------------

    void fullWorkflow_nocrash()
    {
        StatusPanel panel;
        panel.setStudyId("STUDY_001");
        panel.setRunStatus("Running");
        panel.startTimer();
        panel.setCurrentStage("Ingestion");
        panel.setStageState("Ingestion", "Active");
        panel.setProgress(14);
        panel.setStageState("Ingestion", "Completed");
        panel.setCurrentStage("Preprocessing");
        panel.setStageState("Preprocessing", "Active");
        panel.setProgress(28);
        panel.stopTimer();
        panel.setRunStatus("Completed");
        panel.reset();
        QVERIFY(true);
    }
};

QTEST_MAIN(tst_StatusPanel)
#include "tst_StatusPanel.moc"
