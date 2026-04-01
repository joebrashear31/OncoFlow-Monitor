// TDD tests for HistoryPanel — written RED first, then greened.

#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QTreeWidget>
#include "widgets/HistoryPanel.h"
#include "models/PipelineRun.h"
#include "TestHelpers.h"

class tst_HistoryPanel : public QObject
{
    Q_OBJECT

private slots:

    // ----------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------

    void construction_nocrash()
    {
        HistoryPanel panel;
        QVERIFY(true);
    }

    void construction_treeIsEmpty()
    {
        HistoryPanel panel;
        auto *tree = panel.findChild<QTreeWidget*>();
        QVERIFY(tree != nullptr);
        QCOMPARE(tree->topLevelItemCount(), 0);
    }

    // ----------------------------------------------------------------
    // setRuns
    // ----------------------------------------------------------------

    void setRuns_populatesTree()
    {
        HistoryPanel panel;
        auto *tree = panel.findChild<QTreeWidget*>();
        QList<PipelineRun> runs = {
            makePipelineRun("RUN_0001", "Completed"),
            makePipelineRun("RUN_0002", "Failed"),
            makePipelineRun("RUN_0003", "Canceled"),
        };
        panel.setRuns(runs);
        QCOMPARE(tree->topLevelItemCount(), 3);
    }

    void setRuns_replacesExistingItems()
    {
        HistoryPanel panel;
        auto *tree = panel.findChild<QTreeWidget*>();
        panel.setRuns({makePipelineRun("RUN_0001"), makePipelineRun("RUN_0002")});
        panel.setRuns({makePipelineRun("RUN_0003")});
        QCOMPARE(tree->topLevelItemCount(), 1);
    }

    void setRuns_emptyList_clearsTree()
    {
        HistoryPanel panel;
        auto *tree = panel.findChild<QTreeWidget*>();
        panel.setRuns({makePipelineRun("RUN_0001")});
        panel.setRuns({});
        QCOMPARE(tree->topLevelItemCount(), 0);
    }

    void setRuns_runIdInFirstColumn()
    {
        HistoryPanel panel;
        auto *tree = panel.findChild<QTreeWidget*>();
        panel.setRuns({makePipelineRun("RUN_XYZ", "Completed")});
        QCOMPARE(tree->topLevelItem(0)->text(0), QString("RUN_XYZ"));
    }

    void setRuns_statusAppearsInTree()
    {
        HistoryPanel panel;
        auto *tree = panel.findChild<QTreeWidget*>();
        panel.setRuns({makePipelineRun("RUN_0001", "Failed")});
        // Column layout: 0=Run ID, 1=Study, 2=Start Time, 3=Status, 4=Stage
        QCOMPARE(tree->topLevelItem(0)->text(3), QString("Failed"));
    }

    // ----------------------------------------------------------------
    // addRun
    // ----------------------------------------------------------------

    void addRun_appendsItem()
    {
        HistoryPanel panel;
        auto *tree = panel.findChild<QTreeWidget*>();
        panel.addRun(makePipelineRun("RUN_0001"));
        QCOMPARE(tree->topLevelItemCount(), 1);
    }

    void addRun_multipleRuns_allPresent()
    {
        HistoryPanel panel;
        auto *tree = panel.findChild<QTreeWidget*>();
        panel.addRun(makePipelineRun("RUN_0001"));
        panel.addRun(makePipelineRun("RUN_0002"));
        panel.addRun(makePipelineRun("RUN_0003"));
        QCOMPARE(tree->topLevelItemCount(), 3);
    }

    void addRun_afterSetRuns_appendsToExisting()
    {
        HistoryPanel panel;
        auto *tree = panel.findChild<QTreeWidget*>();
        panel.setRuns({makePipelineRun("RUN_0001"), makePipelineRun("RUN_0002")});
        panel.addRun(makePipelineRun("RUN_0003"));
        QCOMPARE(tree->topLevelItemCount(), 3);
    }

    // ----------------------------------------------------------------
    // runSelected signal
    // ----------------------------------------------------------------

    void runSelected_emittedOnItemClick()
    {
        HistoryPanel panel;
        auto *tree = panel.findChild<QTreeWidget*>();
        QSignalSpy spy(&panel, &HistoryPanel::runSelected);
        panel.addRun(makePipelineRun("RUN_0001", "Completed"));
        tree->setCurrentItem(tree->topLevelItem(0));
        QCOMPARE(spy.count(), 1);
    }

    void runSelected_carriesCorrectRun()
    {
        HistoryPanel panel;
        auto *tree = panel.findChild<QTreeWidget*>();
        QSignalSpy spy(&panel, &HistoryPanel::runSelected);
        PipelineRun run = makePipelineRun("RUN_CHECK", "Completed");
        panel.addRun(run);
        tree->setCurrentItem(tree->topLevelItem(0));
        QCOMPARE(spy.count(), 1);
        PipelineRun emitted = qvariant_cast<PipelineRun>(spy.at(0).at(0));
        QCOMPARE(emitted.runId, QString("RUN_CHECK"));
    }
};

QTEST_MAIN(tst_HistoryPanel)
#include "tst_HistoryPanel.moc"
