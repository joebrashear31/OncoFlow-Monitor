// TDD tests for HistoryService — written RED first, then greened.

#include <QtTest/QtTest>
#include <QTemporaryDir>
#include "services/HistoryService.h"
#include "models/PipelineRun.h"
#include "TestHelpers.h"

class tst_HistoryService : public QObject
{
    Q_OBJECT

private slots:

    // ----------------------------------------------------------------
    // loadHistory — empty / missing file
    // ----------------------------------------------------------------

    void loadHistory_noFile_returnsEmptyList()
    {
        QTemporaryDir tmpDir;
        QVERIFY(tmpDir.isValid());
        HistoryService svc(tmpDir.filePath("nonexistent_subdir"));
        QVERIFY(svc.loadHistory().isEmpty());
    }

    void loadHistory_emptyDir_returnsEmptyList()
    {
        QTemporaryDir tmpDir;
        QVERIFY(tmpDir.isValid());
        HistoryService svc(tmpDir.path());
        QVERIFY(svc.loadHistory().isEmpty());
    }

    // ----------------------------------------------------------------
    // saveRun + loadHistory roundtrip
    // ----------------------------------------------------------------

    void saveRun_thenLoad_returnsOneRun()
    {
        QTemporaryDir tmpDir;
        QVERIFY(tmpDir.isValid());
        HistoryService svc(tmpDir.path());

        svc.saveRun(makePipelineRun("RUN_0001", "Completed"));
        auto runs = svc.loadHistory();
        QCOMPARE(runs.size(), 1);
    }

    void saveRun_thenLoad_runIdPreserved()
    {
        QTemporaryDir tmpDir;
        QVERIFY(tmpDir.isValid());
        HistoryService svc(tmpDir.path());

        svc.saveRun(makePipelineRun("RUN_XYZ", "Completed"));
        auto runs = svc.loadHistory();
        QCOMPARE(runs.at(0).runId, QString("RUN_XYZ"));
    }

    void saveRun_thenLoad_statusPreserved()
    {
        QTemporaryDir tmpDir;
        QVERIFY(tmpDir.isValid());
        HistoryService svc(tmpDir.path());

        svc.saveRun(makePipelineRun("RUN_0001", "Failed"));
        QCOMPARE(svc.loadHistory().at(0).status, QString("Failed"));
    }

    void saveRun_thenLoad_studyIdPreserved()
    {
        QTemporaryDir tmpDir;
        QVERIFY(tmpDir.isValid());
        HistoryService svc(tmpDir.path());

        PipelineRun run = makePipelineRun("RUN_0001", "Completed");
        run.studyId = "CT_LUNG_999";
        svc.saveRun(run);
        QCOMPARE(svc.loadHistory().at(0).studyId, QString("CT_LUNG_999"));
    }

    void saveRun_thenLoad_outputPathPreserved()
    {
        QTemporaryDir tmpDir;
        QVERIFY(tmpDir.isValid());
        HistoryService svc(tmpDir.path());

        PipelineRun run = makePipelineRun("RUN_0001", "Completed");
        run.outputPath = "/tmp/special/output";
        svc.saveRun(run);
        QCOMPARE(svc.loadHistory().at(0).outputPath, QString("/tmp/special/output"));
    }

    void saveRun_multiple_allPersisted()
    {
        QTemporaryDir tmpDir;
        QVERIFY(tmpDir.isValid());
        HistoryService svc(tmpDir.path());

        svc.saveRun(makePipelineRun("RUN_0001", "Completed"));
        svc.saveRun(makePipelineRun("RUN_0002", "Failed"));
        svc.saveRun(makePipelineRun("RUN_0003", "Canceled"));
        QCOMPARE(svc.loadHistory().size(), 3);
    }

    void saveRun_multiple_orderPreserved()
    {
        QTemporaryDir tmpDir;
        QVERIFY(tmpDir.isValid());
        HistoryService svc(tmpDir.path());

        svc.saveRun(makePipelineRun("RUN_FIRST",  "Completed"));
        svc.saveRun(makePipelineRun("RUN_SECOND", "Failed"));
        auto runs = svc.loadHistory();
        QCOMPARE(runs.at(0).runId, QString("RUN_FIRST"));
        QCOMPARE(runs.at(1).runId, QString("RUN_SECOND"));
    }

    void saveRun_persistsAcrossServiceInstances()
    {
        QTemporaryDir tmpDir;
        QVERIFY(tmpDir.isValid());

        {
            HistoryService svc(tmpDir.path());
            svc.saveRun(makePipelineRun("RUN_PERSIST", "Completed"));
        }

        // New service instance pointing at same dir
        HistoryService svc2(tmpDir.path());
        auto runs = svc2.loadHistory();
        QCOMPARE(runs.size(), 1);
        QCOMPARE(runs.at(0).runId, QString("RUN_PERSIST"));
    }
};

QTEST_GUILESS_MAIN(tst_HistoryService)
#include "tst_HistoryService.moc"
