// TDD test suite for FrameViewer
// Headless tests — QApplication is created but no window is shown.
// We set QT_QPA_PLATFORM=offscreen so no display is required.

#include <QtTest/QtTest>
#include <QTemporaryDir>
#include <QDir>
#include <QImage>
#include <QApplication>

// FrameViewer is a QWidget — needs QApplication (provided by QTEST_MAIN).
#include "widgets/FrameViewer.h"

class tst_FrameViewer : public QObject
{
    Q_OBJECT

private:
    // Write a minimal 2x2 PNG into dir and return its path.
    static QString writePng(const QDir &dir, const QString &name,
                            QColor color = Qt::red)
    {
        QImage img(2, 2, QImage::Format_RGB32);
        img.fill(color);
        const QString path = dir.absoluteFilePath(name);
        img.save(path, "PNG");
        return path;
    }

private slots:

    // ----------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------

    void construction_doesNotCrash()
    {
        // Simply constructing without a parent must not crash or throw.
        FrameViewer viewer;
        QVERIFY(true);  // reaching here is the assertion
    }

    void construction_withParentWidget_doesNotCrash()
    {
        // Parented construction
        QWidget parent;
        FrameViewer viewer(&parent);
        QVERIFY(true);
    }

    void construction_isNotPlaying()
    {
        FrameViewer viewer;
        QVERIFY(!viewer.isPlaying());
    }

    // ----------------------------------------------------------------
    // clearFrame
    // ----------------------------------------------------------------

    void clearFrame_onFreshViewer_doesNotCrash()
    {
        FrameViewer viewer;
        viewer.clearFrame();  // must not crash on a viewer with no frame
        QVERIFY(true);
    }

    void clearFrame_afterClear_isNotPlaying()
    {
        FrameViewer viewer;
        viewer.clearFrame();
        QVERIFY(!viewer.isPlaying());
    }

    void clearFrame_calledMultipleTimes_doesNotCrash()
    {
        FrameViewer viewer;
        viewer.clearFrame();
        viewer.clearFrame();
        viewer.clearFrame();
        QVERIFY(true);
    }

    // ----------------------------------------------------------------
    // setFrame
    // ----------------------------------------------------------------

    void setFrame_nullImage_doesNotCrash()
    {
        FrameViewer viewer;
        viewer.setFrame(QImage());
        QVERIFY(true);
    }

    void setFrame_validImage_doesNotCrash()
    {
        FrameViewer viewer;
        QImage img(4, 4, QImage::Format_RGB32);
        img.fill(Qt::cyan);
        viewer.setFrame(img);
        QVERIFY(true);
    }

    void setFrame_thenClear_doesNotCrash()
    {
        FrameViewer viewer;
        QImage img(4, 4, QImage::Format_RGB32);
        img.fill(Qt::magenta);
        viewer.setFrame(img);
        viewer.clearFrame();
        QVERIFY(true);
    }

    // ----------------------------------------------------------------
    // loadFolder — invalid paths
    // ----------------------------------------------------------------

    void loadFolder_nonExistentPath_returnsFalse()
    {
        FrameViewer viewer;
        QVERIFY(!viewer.loadFolder("/no/such/folder/exists"));
    }

    void loadFolder_emptyDirectory_returnsFalse()
    {
        QTemporaryDir tmp;
        QVERIFY(tmp.isValid());
        FrameViewer viewer;
        QVERIFY(!viewer.loadFolder(tmp.path()));
    }

    void loadFolder_noSupportedImages_returnsFalse()
    {
        QTemporaryDir tmp;
        QVERIFY(tmp.isValid());
        QFile f(QDir(tmp.path()).absoluteFilePath("doc.txt"));
        f.open(QIODevice::WriteOnly); f.write("x"); f.close();

        FrameViewer viewer;
        QVERIFY(!viewer.loadFolder(tmp.path()));
    }

    // ----------------------------------------------------------------
    // loadFolder — happy paths
    // ----------------------------------------------------------------

    void loadFolder_withImages_returnsTrue()
    {
        QTemporaryDir tmp;
        QVERIFY(tmp.isValid());
        writePng(QDir(tmp.path()), "frame0001.png");

        FrameViewer viewer;
        QVERIFY(viewer.loadFolder(tmp.path()));
    }

    void loadFolder_withImages_notPlayingAfterLoad()
    {
        // loadFolder loads but does NOT auto-play
        QTemporaryDir tmp;
        QVERIFY(tmp.isValid());
        writePng(QDir(tmp.path()), "frame0001.png");

        FrameViewer viewer;
        viewer.loadFolder(tmp.path());
        QVERIFY(!viewer.isPlaying());
    }

    void loadFolder_withMultipleImages_returnsTrue()
    {
        QTemporaryDir tmp;
        QVERIFY(tmp.isValid());
        QDir dir(tmp.path());
        writePng(dir, "a.png", Qt::red);
        writePng(dir, "b.png", Qt::green);
        writePng(dir, "c.png", Qt::blue);

        FrameViewer viewer;
        QVERIFY(viewer.loadFolder(tmp.path()));
    }

    // ----------------------------------------------------------------
    // play / stop
    // ----------------------------------------------------------------

    void playStop_withoutLoadedFolder_doesNotCrash()
    {
        FrameViewer viewer;
        viewer.play();
        viewer.stop();
        QVERIFY(true);
    }

    void play_afterLoadFolder_isPlaying()
    {
        QTemporaryDir tmp;
        QVERIFY(tmp.isValid());
        writePng(QDir(tmp.path()), "frame0001.png");

        FrameViewer viewer;
        viewer.loadFolder(tmp.path());
        viewer.play();
        QVERIFY(viewer.isPlaying());
    }

    void stop_afterPlay_isNotPlaying()
    {
        QTemporaryDir tmp;
        QVERIFY(tmp.isValid());
        writePng(QDir(tmp.path()), "frame0001.png");

        FrameViewer viewer;
        viewer.loadFolder(tmp.path());
        viewer.play();
        viewer.stop();
        QVERIFY(!viewer.isPlaying());
    }

    void playStop_calledRepeatedly_doesNotCrash()
    {
        QTemporaryDir tmp;
        QVERIFY(tmp.isValid());
        writePng(QDir(tmp.path()), "f.png");

        FrameViewer viewer;
        viewer.loadFolder(tmp.path());
        for (int i = 0; i < 5; ++i)
        {
            viewer.play();
            viewer.stop();
        }
        QVERIFY(true);
    }

    // ----------------------------------------------------------------
    // loadFolder then reload
    // ----------------------------------------------------------------

    void loadFolder_reloadDifferentFolder_returnsTrue()
    {
        QTemporaryDir a, b;
        QVERIFY(a.isValid() && b.isValid());
        writePng(QDir(a.path()), "f1.png");
        writePng(QDir(b.path()), "f2.png");

        FrameViewer viewer;
        viewer.loadFolder(a.path());
        QVERIFY(viewer.loadFolder(b.path()));
    }

    void loadFolder_reloadStopsPlayback()
    {
        QTemporaryDir a, b;
        QVERIFY(a.isValid() && b.isValid());
        writePng(QDir(a.path()), "f1.png");
        writePng(QDir(b.path()), "f2.png");

        FrameViewer viewer;
        viewer.loadFolder(a.path());
        viewer.play();
        QVERIFY(viewer.isPlaying());

        viewer.loadFolder(b.path());
        QVERIFY(!viewer.isPlaying());
    }

    // ----------------------------------------------------------------
    // loadVideo — non-existent file (no crash contract)
    // ----------------------------------------------------------------

    void loadVideo_nonExistentFile_doesNotCrash()
    {
        FrameViewer viewer;
        // loadVideo always returns true (it just sets the source URL).
        // The important contract is: no crash.
        viewer.loadVideo("/no/such/video.mp4");
        QVERIFY(true);
    }

    // ----------------------------------------------------------------
    // Timer-driven frame advancement (exercises onNextFrame slot)
    // ----------------------------------------------------------------

    void play_timerAdvancesFrames()
    {
        // Load three frames, start playback, let the timer fire at least twice,
        // then confirm we are still playing (timer still active) and no crash.
        QTemporaryDir tmp;
        QVERIFY(tmp.isValid());
        QDir dir(tmp.path());
        writePng(dir, "frame001.png", Qt::red);
        writePng(dir, "frame002.png", Qt::green);
        writePng(dir, "frame003.png", Qt::blue);

        FrameViewer viewer;
        QVERIFY(viewer.loadFolder(tmp.path()));
        viewer.play();
        QVERIFY(viewer.isPlaying());

        // Wait long enough for the 100 ms timer to fire several times.
        // QTest::qWait processes the event loop so the timer slots execute.
        QTest::qWait(350);

        // After waiting, the viewer should still be in playing state.
        QVERIFY(viewer.isPlaying());

        viewer.stop();
        QVERIFY(!viewer.isPlaying());
    }

    void play_singleFrameFolder_wrapsAroundWithoutCrash()
    {
        // A folder with only one frame: the modulo wrap must not crash.
        QTemporaryDir tmp;
        QVERIFY(tmp.isValid());
        writePng(QDir(tmp.path()), "only.png");

        FrameViewer viewer;
        viewer.loadFolder(tmp.path());
        viewer.play();
        QTest::qWait(350);  // timer fires multiple times on a single frame
        viewer.stop();
        QVERIFY(!viewer.isPlaying());
    }
};

QTEST_MAIN(tst_FrameViewer)
#include "tst_FrameViewer.moc"
