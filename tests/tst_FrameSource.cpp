// TDD test suite for FrameSource
// Tests are written before verifying they compile+fail, then we green them.

#include <QtTest/QtTest>
#include <QTemporaryDir>
#include <QImage>
#include <QFile>
#include <QDir>

#include "services/FrameSource.h"

class tst_FrameSource : public QObject
{
    Q_OBJECT

private:
    // Helper: write a minimal valid 1x1 PNG into a directory.
    // Returns the absolute path written.
    static QString writePng(const QDir &dir, const QString &name)
    {
        QImage img(1, 1, QImage::Format_RGB32);
        img.fill(Qt::red);
        const QString path = dir.absoluteFilePath(name);
        img.save(path, "PNG");
        return path;
    }

    static QString writeJpg(const QDir &dir, const QString &name)
    {
        QImage img(1, 1, QImage::Format_RGB32);
        img.fill(Qt::blue);
        const QString path = dir.absoluteFilePath(name);
        img.save(path, "JPEG");
        return path;
    }

private slots:

    // ----------------------------------------------------------------
    // isLoaded / frameCount — initial state
    // ----------------------------------------------------------------

    void initialState_isNotLoaded()
    {
        FrameSource src;
        QVERIFY(!src.isLoaded());
    }

    void initialState_frameCountIsZero()
    {
        FrameSource src;
        QCOMPARE(src.frameCount(), 0);
    }

    // ----------------------------------------------------------------
    // loadFromFolder — invalid / missing paths
    // ----------------------------------------------------------------

    void loadFromFolder_nonExistentPath_returnsFalse()
    {
        FrameSource src;
        QVERIFY(!src.loadFromFolder("/this/path/does/not/exist/ever"));
    }

    void loadFromFolder_nonExistentPath_remainsUnloaded()
    {
        FrameSource src;
        src.loadFromFolder("/this/path/does/not/exist/ever");
        QVERIFY(!src.isLoaded());
        QCOMPARE(src.frameCount(), 0);
    }

    void loadFromFolder_emptyString_returnsFalse()
    {
        FrameSource src;
        QVERIFY(!src.loadFromFolder(QString()));
    }

    void loadFromFolder_emptyDirectory_returnsFalse()
    {
        QTemporaryDir tmpDir;
        QVERIFY(tmpDir.isValid());
        FrameSource src;
        QVERIFY(!src.loadFromFolder(tmpDir.path()));
    }

    void loadFromFolder_emptyDirectory_remainsUnloaded()
    {
        QTemporaryDir tmpDir;
        QVERIFY(tmpDir.isValid());
        FrameSource src;
        src.loadFromFolder(tmpDir.path());
        QVERIFY(!src.isLoaded());
        QCOMPARE(src.frameCount(), 0);
    }

    void loadFromFolder_directoryWithNoImages_returnsFalse()
    {
        QTemporaryDir tmpDir;
        QVERIFY(tmpDir.isValid());
        // Write a file that matches none of the supported extensions
        QFile txt(tmpDir.filePath("readme.txt"));
        txt.open(QIODevice::WriteOnly);
        txt.write("hello");
        txt.close();

        FrameSource src;
        QVERIFY(!src.loadFromFolder(tmpDir.path()));
    }

    // ----------------------------------------------------------------
    // loadFromFolder — happy paths
    // ----------------------------------------------------------------

    void loadFromFolder_singlePng_returnsTrue()
    {
        QTemporaryDir tmpDir;
        QVERIFY(tmpDir.isValid());
        writePng(QDir(tmpDir.path()), "frame0001.png");

        FrameSource src;
        QVERIFY(src.loadFromFolder(tmpDir.path()));
    }

    void loadFromFolder_singlePng_isLoaded()
    {
        QTemporaryDir tmpDir;
        QVERIFY(tmpDir.isValid());
        writePng(QDir(tmpDir.path()), "frame0001.png");

        FrameSource src;
        src.loadFromFolder(tmpDir.path());
        QVERIFY(src.isLoaded());
    }

    void loadFromFolder_singlePng_frameCountIsOne()
    {
        QTemporaryDir tmpDir;
        QVERIFY(tmpDir.isValid());
        writePng(QDir(tmpDir.path()), "frame0001.png");

        FrameSource src;
        src.loadFromFolder(tmpDir.path());
        QCOMPARE(src.frameCount(), 1);
    }

    void loadFromFolder_multiplePngs_correctCount()
    {
        QTemporaryDir tmpDir;
        QVERIFY(tmpDir.isValid());
        QDir dir(tmpDir.path());
        writePng(dir, "a.png");
        writePng(dir, "b.png");
        writePng(dir, "c.png");

        FrameSource src;
        src.loadFromFolder(tmpDir.path());
        QCOMPARE(src.frameCount(), 3);
    }

    void loadFromFolder_jpgFiles_detected()
    {
        QTemporaryDir tmpDir;
        QVERIFY(tmpDir.isValid());
        writeJpg(QDir(tmpDir.path()), "frame.jpg");

        FrameSource src;
        QVERIFY(src.loadFromFolder(tmpDir.path()));
        QCOMPARE(src.frameCount(), 1);
    }

    void loadFromFolder_bmpFile_detected()
    {
        QTemporaryDir tmpDir;
        QVERIFY(tmpDir.isValid());
        QImage img(1, 1, QImage::Format_RGB32);
        img.fill(Qt::green);
        img.save(QDir(tmpDir.path()).absoluteFilePath("frame.bmp"), "BMP");

        FrameSource src;
        QVERIFY(src.loadFromFolder(tmpDir.path()));
        QCOMPARE(src.frameCount(), 1);
    }

    void loadFromFolder_mixedExtensions_onlyImagesIncluded()
    {
        QTemporaryDir tmpDir;
        QVERIFY(tmpDir.isValid());
        QDir dir(tmpDir.path());
        writePng(dir, "frame1.png");
        writeJpg(dir, "frame2.jpg");
        // Non-image files should be ignored
        QFile txt(dir.absoluteFilePath("notes.txt"));
        txt.open(QIODevice::WriteOnly); txt.write("x"); txt.close();
        QFile xml(dir.absoluteFilePath("data.xml"));
        xml.open(QIODevice::WriteOnly); xml.write("<x/>"); xml.close();

        FrameSource src;
        src.loadFromFolder(tmpDir.path());
        QCOMPARE(src.frameCount(), 2);
    }

    // ----------------------------------------------------------------
    // loadFromFolder — sorting guarantee
    // ----------------------------------------------------------------

    void loadFromFolder_framesAreSortedByName()
    {
        QTemporaryDir tmpDir;
        QVERIFY(tmpDir.isValid());
        QDir dir(tmpDir.path());
        // Create in reverse alphabetical order; loader must sort by name
        writePng(dir, "frame003.png");
        writePng(dir, "frame001.png");
        writePng(dir, "frame002.png");

        FrameSource src;
        src.loadFromFolder(tmpDir.path());
        QCOMPARE(src.frameCount(), 3);
        QVERIFY(src.getFramePath(0).endsWith("frame001.png"));
        QVERIFY(src.getFramePath(1).endsWith("frame002.png"));
        QVERIFY(src.getFramePath(2).endsWith("frame003.png"));
    }

    // ----------------------------------------------------------------
    // loadFromFolder — reload / reset behaviour
    // ----------------------------------------------------------------

    void loadFromFolder_reloadEmptyDir_clearsState()
    {
        QTemporaryDir dirA;
        QVERIFY(dirA.isValid());
        writePng(QDir(dirA.path()), "frame.png");

        QTemporaryDir dirB;  // empty
        QVERIFY(dirB.isValid());

        FrameSource src;
        src.loadFromFolder(dirA.path());
        QVERIFY(src.isLoaded());

        // Reload with empty dir — state must be cleared
        src.loadFromFolder(dirB.path());
        QVERIFY(!src.isLoaded());
        QCOMPARE(src.frameCount(), 0);
    }

    void loadFromFolder_reloadNewDir_updatesFrameList()
    {
        QTemporaryDir dirA;
        QVERIFY(dirA.isValid());
        writePng(QDir(dirA.path()), "a.png");
        writePng(QDir(dirA.path()), "b.png");

        QTemporaryDir dirB;
        QVERIFY(dirB.isValid());
        writePng(QDir(dirB.path()), "only.png");

        FrameSource src;
        src.loadFromFolder(dirA.path());
        QCOMPARE(src.frameCount(), 2);

        src.loadFromFolder(dirB.path());
        QCOMPARE(src.frameCount(), 1);
    }

    // ----------------------------------------------------------------
    // getFrame — boundary and invalid indices
    // ----------------------------------------------------------------

    void getFrame_negativeIndex_returnsNullImage()
    {
        QTemporaryDir tmpDir;
        QVERIFY(tmpDir.isValid());
        writePng(QDir(tmpDir.path()), "f.png");

        FrameSource src;
        src.loadFromFolder(tmpDir.path());
        QVERIFY(src.getFrame(-1).isNull());
    }

    void getFrame_indexEqualToCount_returnsNullImage()
    {
        QTemporaryDir tmpDir;
        QVERIFY(tmpDir.isValid());
        writePng(QDir(tmpDir.path()), "f.png");

        FrameSource src;
        src.loadFromFolder(tmpDir.path());
        QVERIFY(src.getFrame(src.frameCount()).isNull());
    }

    void getFrame_largeIndex_returnsNullImage()
    {
        QTemporaryDir tmpDir;
        QVERIFY(tmpDir.isValid());
        writePng(QDir(tmpDir.path()), "f.png");

        FrameSource src;
        src.loadFromFolder(tmpDir.path());
        QVERIFY(src.getFrame(99999).isNull());
    }

    void getFrame_validIndex_returnsNonNullImage()
    {
        QTemporaryDir tmpDir;
        QVERIFY(tmpDir.isValid());
        writePng(QDir(tmpDir.path()), "f.png");

        FrameSource src;
        src.loadFromFolder(tmpDir.path());
        QVERIFY(!src.getFrame(0).isNull());
    }

    void getFrame_validIndex_returnsCorrectDimensions()
    {
        QTemporaryDir tmpDir;
        QVERIFY(tmpDir.isValid());
        // Write a 4x3 image so we can verify dimensions come back correctly
        QImage img(4, 3, QImage::Format_RGB32);
        img.fill(Qt::white);
        img.save(QDir(tmpDir.path()).absoluteFilePath("sized.png"), "PNG");

        FrameSource src;
        src.loadFromFolder(tmpDir.path());
        QImage loaded = src.getFrame(0);
        QCOMPARE(loaded.width(),  4);
        QCOMPARE(loaded.height(), 3);
    }

    void getFrame_whenNotLoaded_returnsNullImage()
    {
        FrameSource src;
        QVERIFY(src.getFrame(0).isNull());
    }

    // ----------------------------------------------------------------
    // getFramePath — boundary and invalid indices
    // ----------------------------------------------------------------

    void getFramePath_negativeIndex_returnsEmptyString()
    {
        QTemporaryDir tmpDir;
        QVERIFY(tmpDir.isValid());
        writePng(QDir(tmpDir.path()), "f.png");

        FrameSource src;
        src.loadFromFolder(tmpDir.path());
        QVERIFY(src.getFramePath(-1).isEmpty());
    }

    void getFramePath_indexEqualToCount_returnsEmptyString()
    {
        QTemporaryDir tmpDir;
        QVERIFY(tmpDir.isValid());
        writePng(QDir(tmpDir.path()), "f.png");

        FrameSource src;
        src.loadFromFolder(tmpDir.path());
        QVERIFY(src.getFramePath(src.frameCount()).isEmpty());
    }

    void getFramePath_validIndex_returnsAbsolutePath()
    {
        QTemporaryDir tmpDir;
        QVERIFY(tmpDir.isValid());
        const QString written = writePng(QDir(tmpDir.path()), "f.png");

        FrameSource src;
        src.loadFromFolder(tmpDir.path());
        QCOMPARE(src.getFramePath(0), written);
    }

    void getFramePath_whenNotLoaded_returnsEmptyString()
    {
        FrameSource src;
        QVERIFY(src.getFramePath(0).isEmpty());
    }

    // ----------------------------------------------------------------
    // Large dataset — performance smoke test
    // ----------------------------------------------------------------

    void loadFromFolder_manyImages_completesInReasonableTime()
    {
        // 200 tiny 1×1 PNGs; load should complete well under 5 s
        QTemporaryDir tmpDir;
        QVERIFY(tmpDir.isValid());
        QDir dir(tmpDir.path());

        for (int i = 0; i < 200; ++i)
        {
            QImage img(1, 1, QImage::Format_RGB32);
            img.fill(Qt::black);
            img.save(dir.absoluteFilePath(QString("frame%1.png").arg(i, 4, 10, QChar('0'))), "PNG");
        }

        FrameSource src;
        QBENCHMARK_ONCE {
            src.loadFromFolder(tmpDir.path());
        }
        QCOMPARE(src.frameCount(), 200);
    }

    // ----------------------------------------------------------------
    // Special characters in path
    // ----------------------------------------------------------------

    void loadFromFolder_pathWithSpaces_works()
    {
        QTemporaryDir base;
        QVERIFY(base.isValid());
        // Create a sub-directory whose name contains spaces
        QDir d(base.path());
        d.mkdir("my frames folder");
        d.cd("my frames folder");
        writePng(d, "img.png");

        FrameSource src;
        QVERIFY(src.loadFromFolder(d.absolutePath()));
        QCOMPARE(src.frameCount(), 1);
    }
};

QTEST_MAIN(tst_FrameSource)
#include "tst_FrameSource.moc"
