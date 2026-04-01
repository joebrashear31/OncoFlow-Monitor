// TDD tests for LogPanel — written RED first, then greened.

#include <QtTest/QtTest>
#include <QCheckBox>
#include <QLineEdit>
#include <QTreeWidget>
#include "widgets/LogPanel.h"
#include "models/LogEntry.h"

static LogEntry makeLog(const QString &severity, const QString &message,
                        const QString &runId = "RUN_001")
{
    LogEntry e;
    e.timestamp = QDateTime::currentDateTime();
    e.severity  = severity;
    e.message   = message;
    e.runId     = runId;
    return e;
}

class tst_LogPanel : public QObject
{
    Q_OBJECT

private slots:

    // ----------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------

    void construction_nocrash()
    {
        LogPanel panel;
        QVERIFY(true);
    }

    void construction_treeIsEmpty()
    {
        LogPanel panel;
        auto *tree = panel.findChild<QTreeWidget*>();
        QVERIFY(tree != nullptr);
        QCOMPARE(tree->topLevelItemCount(), 0);
    }

    void construction_severityFilters_allChecked()
    {
        LogPanel panel;
        auto boxes = panel.findChildren<QCheckBox*>();
        for (auto *cb : boxes) {
            QVERIFY(cb->isChecked());
        }
    }

    // ----------------------------------------------------------------
    // appendLog — happy paths
    // ----------------------------------------------------------------

    void appendLog_info_appearsInTree()
    {
        LogPanel panel;
        auto *tree = panel.findChild<QTreeWidget*>();
        panel.appendLog(makeLog("Info", "Hello world"));
        QCOMPARE(tree->topLevelItemCount(), 1);
    }

    void appendLog_warning_appearsInTree()
    {
        LogPanel panel;
        auto *tree = panel.findChild<QTreeWidget*>();
        panel.appendLog(makeLog("Warning", "Something odd"));
        QCOMPARE(tree->topLevelItemCount(), 1);
    }

    void appendLog_error_appearsInTree()
    {
        LogPanel panel;
        auto *tree = panel.findChild<QTreeWidget*>();
        panel.appendLog(makeLog("Error", "Critical failure"));
        QCOMPARE(tree->topLevelItemCount(), 1);
    }

    void appendLog_multipleEntries_allAppearsInTree()
    {
        LogPanel panel;
        auto *tree = panel.findChild<QTreeWidget*>();
        panel.appendLog(makeLog("Info",    "msg1"));
        panel.appendLog(makeLog("Warning", "msg2"));
        panel.appendLog(makeLog("Error",   "msg3"));
        QCOMPARE(tree->topLevelItemCount(), 3);
    }

    void appendLog_severityColumnMatchesEntry()
    {
        LogPanel panel;
        auto *tree = panel.findChild<QTreeWidget*>();
        panel.appendLog(makeLog("Warning", "test warning"));
        QCOMPARE(tree->topLevelItem(0)->text(1), QString("Warning"));
    }

    void appendLog_messageColumnMatchesEntry()
    {
        LogPanel panel;
        auto *tree = panel.findChild<QTreeWidget*>();
        panel.appendLog(makeLog("Info", "my unique message"));
        QCOMPARE(tree->topLevelItem(0)->text(2), QString("my unique message"));
    }

    // ----------------------------------------------------------------
    // clearLogs
    // ----------------------------------------------------------------

    void clearLogs_removesAllTreeItems()
    {
        LogPanel panel;
        auto *tree = panel.findChild<QTreeWidget*>();
        panel.appendLog(makeLog("Info",  "a"));
        panel.appendLog(makeLog("Error", "b"));
        QCOMPARE(tree->topLevelItemCount(), 2);
        panel.clearLogs();
        QCOMPARE(tree->topLevelItemCount(), 0);
    }

    void clearLogs_idempotent()
    {
        LogPanel panel;
        panel.clearLogs();
        panel.clearLogs();
        auto *tree = panel.findChild<QTreeWidget*>();
        QCOMPARE(tree->topLevelItemCount(), 0);
    }

    void clearLogs_afterClear_appendWorksAgain()
    {
        LogPanel panel;
        auto *tree = panel.findChild<QTreeWidget*>();
        panel.appendLog(makeLog("Info", "before"));
        panel.clearLogs();
        panel.appendLog(makeLog("Info", "after"));
        QCOMPARE(tree->topLevelItemCount(), 1);
    }

    // ----------------------------------------------------------------
    // Severity filter: uncheck Info hides Info entries
    // ----------------------------------------------------------------

    void filter_uncheckInfo_hidesInfoEntries()
    {
        LogPanel panel;
        auto *tree = panel.findChild<QTreeWidget*>();

        panel.appendLog(makeLog("Info",  "info msg"));
        panel.appendLog(makeLog("Error", "error msg"));
        QCOMPARE(tree->topLevelItemCount(), 2);

        // Uncheck Info checkbox
        for (auto *cb : panel.findChildren<QCheckBox*>()) {
            if (cb->text() == "Info") { cb->setChecked(false); break; }
        }
        // applyFilters() is triggered by checkStateChanged
        QCOMPARE(tree->topLevelItemCount(), 1);
        QCOMPARE(tree->topLevelItem(0)->text(1), QString("Error"));
    }

    void filter_uncheckWarning_hidesWarningEntries()
    {
        LogPanel panel;
        auto *tree = panel.findChild<QTreeWidget*>();

        panel.appendLog(makeLog("Warning", "warn msg"));
        panel.appendLog(makeLog("Info",    "info msg"));

        for (auto *cb : panel.findChildren<QCheckBox*>()) {
            if (cb->text() == "Warning") { cb->setChecked(false); break; }
        }
        QCOMPARE(tree->topLevelItemCount(), 1);
        QCOMPARE(tree->topLevelItem(0)->text(1), QString("Info"));
    }

    void filter_uncheckError_hidesErrorEntries()
    {
        LogPanel panel;
        auto *tree = panel.findChild<QTreeWidget*>();

        panel.appendLog(makeLog("Error", "err msg"));
        panel.appendLog(makeLog("Info",  "info msg"));

        for (auto *cb : panel.findChildren<QCheckBox*>()) {
            if (cb->text() == "Error") { cb->setChecked(false); break; }
        }
        QCOMPARE(tree->topLevelItemCount(), 1);
        QCOMPARE(tree->topLevelItem(0)->text(1), QString("Info"));
    }

    void filter_recheckSeverity_restoresEntries()
    {
        LogPanel panel;
        auto *tree = panel.findChild<QTreeWidget*>();
        panel.appendLog(makeLog("Info", "msg"));

        QCheckBox *infoCb = nullptr;
        for (auto *cb : panel.findChildren<QCheckBox*>()) {
            if (cb->text() == "Info") { infoCb = cb; break; }
        }
        QVERIFY(infoCb != nullptr);

        infoCb->setChecked(false);
        QCOMPARE(tree->topLevelItemCount(), 0);
        infoCb->setChecked(true);
        QCOMPARE(tree->topLevelItemCount(), 1);
    }

    // ----------------------------------------------------------------
    // Search filter
    // ----------------------------------------------------------------

    void filter_searchText_onlyMatchingRowsVisible()
    {
        LogPanel panel;
        auto *tree = panel.findChild<QTreeWidget*>();
        auto *search = panel.findChild<QLineEdit*>();
        QVERIFY(search != nullptr);

        panel.appendLog(makeLog("Info", "alpha message"));
        panel.appendLog(makeLog("Info", "beta message"));
        panel.appendLog(makeLog("Info", "gamma log"));

        search->setText("beta");
        QCOMPARE(tree->topLevelItemCount(), 1);
        QCOMPARE(tree->topLevelItem(0)->text(2), QString("beta message"));
    }

    void filter_searchCaseInsensitive()
    {
        LogPanel panel;
        auto *tree = panel.findChild<QTreeWidget*>();
        auto *search = panel.findChild<QLineEdit*>();

        panel.appendLog(makeLog("Info", "Alpha Message"));
        panel.appendLog(makeLog("Info", "Beta Message"));

        search->setText("ALPHA");
        QCOMPARE(tree->topLevelItemCount(), 1);
    }

    void filter_clearSearch_restoresAll()
    {
        LogPanel panel;
        auto *tree = panel.findChild<QTreeWidget*>();
        auto *search = panel.findChild<QLineEdit*>();

        panel.appendLog(makeLog("Info", "one"));
        panel.appendLog(makeLog("Info", "two"));

        search->setText("one");
        QCOMPARE(tree->topLevelItemCount(), 1);
        search->clear();
        QCOMPARE(tree->topLevelItemCount(), 2);
    }
};

QTEST_MAIN(tst_LogPanel)
#include "tst_LogPanel.moc"
