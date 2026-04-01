// TDD tests for PipelineWorker — written RED first, then greened.

#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QThread>
#include "services/PipelineWorker.h"
#include "models/PipelineConfig.h"
#include "models/LogEntry.h"
#include "TestHelpers.h"

class tst_PipelineWorker : public QObject
{
    Q_OBJECT

private slots:

    // ----------------------------------------------------------------
    // Cancel-before-run — synchronous, fast
    // ----------------------------------------------------------------

    void cancelBeforeRun_emitsRunCanceled()
    {
        PipelineWorker worker("RUN_001", "ST001", PipelineConfig{});
        QSignalSpy spy(&worker, &PipelineWorker::runCanceled);
        worker.requestCancel();
        worker.run();
        QCOMPARE(spy.count(), 1);
    }

    void cancelBeforeRun_doesNotEmitStageStarted()
    {
        PipelineWorker worker("RUN_001", "ST001", PipelineConfig{});
        QSignalSpy spy(&worker, &PipelineWorker::stageStarted);
        worker.requestCancel();
        worker.run();
        QCOMPARE(spy.count(), 0);
    }

    void cancelBeforeRun_doesNotEmitRunFinished()
    {
        PipelineWorker worker("RUN_001", "ST001", PipelineConfig{});
        QSignalSpy spy(&worker, &PipelineWorker::runFinished);
        worker.requestCancel();
        worker.run();
        QCOMPARE(spy.count(), 0);
    }

    void cancelBeforeRun_emitsWarningLog()
    {
        PipelineWorker worker("RUN_TESTID", "ST001", PipelineConfig{});
        QSignalSpy spy(&worker, &PipelineWorker::logGenerated);
        worker.requestCancel();
        worker.run();
        // At least one log entry (the cancel warning) should be emitted
        QVERIFY(spy.count() >= 1);
        // The cancel log should carry the correct runId
        LogEntry entry = qvariant_cast<LogEntry>(spy.last().at(0));
        QCOMPARE(entry.runId, QString("RUN_TESTID"));
    }

    // ----------------------------------------------------------------
    // Threaded run — uses QSignalSpy::wait so the test stays responsive
    // Cancel immediately after first stageStarted to keep tests fast
    // ----------------------------------------------------------------

    void run_emitsStageStarted()
    {
        PipelineWorker worker("RUN_002", "ST001", PipelineConfig{});
        QThread thread;
        worker.moveToThread(&thread);
        QSignalSpy spy(&worker, &PipelineWorker::stageStarted);
        connect(&thread, &QThread::started, &worker, &PipelineWorker::run);

        thread.start();
        QVERIFY(spy.wait(5000));
        QVERIFY(!spy.isEmpty());

        worker.requestCancel();
        thread.quit();
        QVERIFY(thread.wait(5000));
    }

    void run_stageStartedArg_isNonEmpty()
    {
        PipelineWorker worker("RUN_002", "ST001", PipelineConfig{});
        QThread thread;
        worker.moveToThread(&thread);
        QSignalSpy spy(&worker, &PipelineWorker::stageStarted);
        connect(&thread, &QThread::started, &worker, &PipelineWorker::run);

        thread.start();
        QVERIFY(spy.wait(5000));

        QString stageName = spy.at(0).at(0).toString();
        QVERIFY(!stageName.isEmpty());

        worker.requestCancel();
        thread.quit();
        QVERIFY(thread.wait(5000));
    }

    void run_emitsLogGenerated()
    {
        PipelineWorker worker("RUN_003", "ST001", PipelineConfig{});
        QThread thread;
        worker.moveToThread(&thread);
        QSignalSpy spy(&worker, &PipelineWorker::logGenerated);
        connect(&thread, &QThread::started, &worker, &PipelineWorker::run);

        thread.start();
        QVERIFY(spy.wait(5000));
        QVERIFY(spy.count() >= 1);

        worker.requestCancel();
        thread.quit();
        QVERIFY(thread.wait(5000));
    }

    void run_logGenerated_hasCorrectRunId()
    {
        PipelineWorker worker("RUN_MYID", "ST001", PipelineConfig{});
        QThread thread;
        worker.moveToThread(&thread);
        QSignalSpy spy(&worker, &PipelineWorker::logGenerated);
        connect(&thread, &QThread::started, &worker, &PipelineWorker::run);

        thread.start();
        QVERIFY(spy.wait(5000));

        LogEntry entry = qvariant_cast<LogEntry>(spy.at(0).at(0));
        QCOMPARE(entry.runId, QString("RUN_MYID"));

        worker.requestCancel();
        thread.quit();
        QVERIFY(thread.wait(5000));
    }

    void run_cancelMidRun_emitsRunCanceled()
    {
        PipelineWorker worker("RUN_004", "ST001", PipelineConfig{});
        QThread thread;
        worker.moveToThread(&thread);
        QSignalSpy stageSpy(&worker, &PipelineWorker::stageStarted);
        QSignalSpy cancelSpy(&worker, &PipelineWorker::runCanceled);
        connect(&thread, &QThread::started, &worker, &PipelineWorker::run);

        thread.start();
        // Wait for at least one stage to start, then cancel
        QVERIFY(stageSpy.wait(5000));
        worker.requestCancel();

        // runCanceled should arrive within one 100ms sleep step
        QVERIFY(cancelSpy.wait(5000));
        QCOMPARE(cancelSpy.count(), 1);

        thread.quit();
        QVERIFY(thread.wait(5000));
    }

    void run_cancelMidRun_doesNotEmitRunFinished()
    {
        PipelineWorker worker("RUN_005", "ST001", PipelineConfig{});
        QThread thread;
        worker.moveToThread(&thread);
        QSignalSpy stageSpy(&worker, &PipelineWorker::stageStarted);
        QSignalSpy finishedSpy(&worker, &PipelineWorker::runFinished);
        QSignalSpy cancelSpy(&worker, &PipelineWorker::runCanceled);
        connect(&thread, &QThread::started, &worker, &PipelineWorker::run);

        thread.start();
        QVERIFY(stageSpy.wait(5000));
        worker.requestCancel();
        QVERIFY(cancelSpy.wait(5000));

        QCOMPARE(finishedSpy.count(), 0);

        thread.quit();
        QVERIFY(thread.wait(5000));
    }
};

QTEST_MAIN(tst_PipelineWorker)
#include "tst_PipelineWorker.moc"
