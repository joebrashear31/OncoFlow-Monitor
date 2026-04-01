// TDD tests for StudyListWidget — written RED first, then greened.

#include <QtTest/QtTest>
#include <QLineEdit>
#include <QSignalSpy>
#include <QTreeWidget>
#include "widgets/StudyListWidget.h"
#include "models/Study.h"
#include "TestHelpers.h"

static QList<Study> makeStudyList(int count)
{
    QList<Study> list;
    for (int i = 0; i < count; ++i) {
        list.append(makeStudy(QString("ST%1").arg(i + 1, 3, 10, QChar('0'))));
    }
    return list;
}

class tst_StudyListWidget : public QObject
{
    Q_OBJECT

private slots:

    // ----------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------

    void construction_nocrash()
    {
        StudyListWidget widget;
        QVERIFY(true);
    }

    void construction_hasNoSelection()
    {
        StudyListWidget widget;
        QVERIFY(!widget.hasSelection());
    }

    void construction_treeIsEmpty()
    {
        StudyListWidget widget;
        auto *tree = widget.findChild<QTreeWidget*>();
        QVERIFY(tree != nullptr);
        QCOMPARE(tree->topLevelItemCount(), 0);
    }

    // ----------------------------------------------------------------
    // setStudies
    // ----------------------------------------------------------------

    void setStudies_populatesTree()
    {
        StudyListWidget widget;
        auto *tree = widget.findChild<QTreeWidget*>();
        widget.setStudies(makeStudyList(3));
        QCOMPARE(tree->topLevelItemCount(), 3);
    }

    void setStudies_emptyList_clearsTree()
    {
        StudyListWidget widget;
        auto *tree = widget.findChild<QTreeWidget*>();
        widget.setStudies(makeStudyList(5));
        widget.setStudies({});
        QCOMPARE(tree->topLevelItemCount(), 0);
    }

    void setStudies_replacesExistingItems()
    {
        StudyListWidget widget;
        auto *tree = widget.findChild<QTreeWidget*>();
        widget.setStudies(makeStudyList(5));
        widget.setStudies(makeStudyList(2));
        QCOMPARE(tree->topLevelItemCount(), 2);
    }

    void setStudies_studyIdInFirstColumn()
    {
        StudyListWidget widget;
        auto *tree = widget.findChild<QTreeWidget*>();
        Study s = makeStudy("MYID");
        widget.setStudies({s});
        QCOMPARE(tree->topLevelItem(0)->text(0), QString("MYID"));
    }

    void setStudies_noSelectionAfterLoad()
    {
        StudyListWidget widget;
        widget.setStudies(makeStudyList(3));
        QVERIFY(!widget.hasSelection());
    }

    // ----------------------------------------------------------------
    // Selection
    // ----------------------------------------------------------------

    void selectItem_hasSelectionBecomesTrue()
    {
        StudyListWidget widget;
        auto *tree = widget.findChild<QTreeWidget*>();
        widget.setStudies(makeStudyList(3));
        tree->setCurrentItem(tree->topLevelItem(0));
        QVERIFY(widget.hasSelection());
    }

    void selectedStudy_returnsCorrectStudy()
    {
        StudyListWidget widget;
        auto *tree = widget.findChild<QTreeWidget*>();
        Study s = makeStudy("TARGET");
        widget.setStudies({s});
        tree->setCurrentItem(tree->topLevelItem(0));
        QCOMPARE(widget.selectedStudy().studyId, QString("TARGET"));
    }

    void selectedStudy_whenNoSelection_returnsEmpty()
    {
        StudyListWidget widget;
        widget.setStudies(makeStudyList(3));
        Study s = widget.selectedStudy();
        QVERIFY(s.studyId.isEmpty());
    }

    void studySelected_signal_emittedOnClick()
    {
        StudyListWidget widget;
        auto *tree = widget.findChild<QTreeWidget*>();
        QSignalSpy spy(&widget, &StudyListWidget::studySelected);
        widget.setStudies(makeStudyList(3));
        tree->setCurrentItem(tree->topLevelItem(1));
        QCOMPARE(spy.count(), 1);
    }

    void studySelected_signal_carriesCorrectStudy()
    {
        StudyListWidget widget;
        auto *tree = widget.findChild<QTreeWidget*>();
        QSignalSpy spy(&widget, &StudyListWidget::studySelected);
        Study s = makeStudy("EMITTED");
        widget.setStudies({s});
        tree->setCurrentItem(tree->topLevelItem(0));
        QCOMPARE(spy.count(), 1);
        Study emitted = qvariant_cast<Study>(spy.at(0).at(0));
        QCOMPARE(emitted.studyId, QString("EMITTED"));
    }

    // ----------------------------------------------------------------
    // Search / filter
    // ----------------------------------------------------------------

    void searchFilter_filtersTree()
    {
        StudyListWidget widget;
        auto *tree   = widget.findChild<QTreeWidget*>();
        auto *search = widget.findChild<QLineEdit*>();
        QVERIFY(search != nullptr);

        Study a = makeStudy("ALPHA_001");
        Study b = makeStudy("BETA_002");
        Study c = makeStudy("ALPHA_003");
        widget.setStudies({a, b, c});

        search->setText("ALPHA");
        QCOMPARE(tree->topLevelItemCount(), 2);
    }

    void searchFilter_caseInsensitive()
    {
        StudyListWidget widget;
        auto *tree   = widget.findChild<QTreeWidget*>();
        auto *search = widget.findChild<QLineEdit*>();

        Study s = makeStudy("GAMMA_001");
        widget.setStudies({s});
        search->setText("gamma");
        QCOMPARE(tree->topLevelItemCount(), 1);
    }

    void searchFilter_clearRestoresAll()
    {
        StudyListWidget widget;
        auto *tree   = widget.findChild<QTreeWidget*>();
        auto *search = widget.findChild<QLineEdit*>();

        widget.setStudies(makeStudyList(4));
        search->setText("ST001");
        QCOMPARE(tree->topLevelItemCount(), 1);
        search->clear();
        QCOMPARE(tree->topLevelItemCount(), 4);
    }

    void searchFilter_noMatch_emptyTree()
    {
        StudyListWidget widget;
        auto *tree   = widget.findChild<QTreeWidget*>();
        auto *search = widget.findChild<QLineEdit*>();

        widget.setStudies(makeStudyList(3));
        search->setText("ZZZNOMATCH");
        QCOMPARE(tree->topLevelItemCount(), 0);
    }
};

QTEST_MAIN(tst_StudyListWidget)
#include "tst_StudyListWidget.moc"
