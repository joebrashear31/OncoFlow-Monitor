// TDD tests for StudyService::loadStudies()

#include <QtTest/QtTest>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTemporaryDir>

#include "services/StudyService.h"
#include "TestHelpers.h"

class tst_StudyService : public QObject
{
    Q_OBJECT

private:
    // Write a JSON array of study objects to a temp file.
    static QString writeStudiesJson(const QDir &dir, const QString &name, const QJsonArray &arr)
    {
        QJsonDocument doc(arr);
        const QString path = dir.absoluteFilePath(name);
        QFile f(path);
        f.open(QIODevice::WriteOnly);
        f.write(doc.toJson());
        return path;
    }

    static QJsonObject makeStudyObj(const QString &id,
                                    const QString &patient = "P",
                                    const QString &modality = "CT",
                                    const QString &date = "2026-03-01",
                                    const QString &filePath = "mock/x",
                                    const QString &status = "Ready")
    {
        QJsonObject o;
        o["studyId"]         = id;
        o["patientAlias"]    = patient;
        o["modality"]        = modality;
        o["acquisitionDate"] = date;
        o["filePath"]        = filePath;
        o["status"]          = status;
        return o;
    }

private slots:

    // ----------------------------------------------------------------
    // Happy path — valid JSON
    // ----------------------------------------------------------------

    void loadStudies_validJson_returnsCorrectCount()
    {
        QTemporaryDir tmp;
        QVERIFY(tmp.isValid());

        QJsonArray arr;
        arr.append(makeStudyObj("S001"));
        arr.append(makeStudyObj("S002"));
        arr.append(makeStudyObj("S003"));
        const QString path = writeStudiesJson(QDir(tmp.path()), "studies.json", arr);

        StudyService svc;
        const auto studies = svc.loadStudies(path);
        QCOMPARE(studies.size(), 3);
    }

    void loadStudies_validJson_studyIdParsedCorrectly()
    {
        QTemporaryDir tmp;
        QVERIFY(tmp.isValid());

        QJsonArray arr;
        arr.append(makeStudyObj("MY_STUDY_ID"));
        const QString path = writeStudiesJson(QDir(tmp.path()), "studies.json", arr);

        StudyService svc;
        const auto studies = svc.loadStudies(path);
        QCOMPARE(studies.at(0).studyId, QStringLiteral("MY_STUDY_ID"));
    }

    void loadStudies_validJson_acquisitionDateParsed()
    {
        QTemporaryDir tmp;
        QVERIFY(tmp.isValid());

        QJsonArray arr;
        arr.append(makeStudyObj("S001", "P", "CT", "2026-03-15"));
        const QString path = writeStudiesJson(QDir(tmp.path()), "studies.json", arr);

        StudyService svc;
        const auto studies = svc.loadStudies(path);
        QCOMPARE(studies.at(0).acquisitionDate, QDate(2026, 3, 15));
    }

    // ----------------------------------------------------------------
    // Fallback paths
    // ----------------------------------------------------------------

    void loadStudies_nonExistentPath_returnsFallback()
    {
        StudyService svc;
        const auto studies = svc.loadStudies("/this/path/definitely/does/not/exist.json");
        QVERIFY(!studies.isEmpty());
    }

    void loadStudies_emptyStringPath_returnsFallback()
    {
        StudyService svc;
        const auto studies = svc.loadStudies(QString());
        QVERIFY(!studies.isEmpty());
    }

    void loadStudies_invalidJson_returnsFallback()
    {
        QTemporaryDir tmp;
        QVERIFY(tmp.isValid());
        const QString path = tmp.filePath("bad.json");
        QFile f(path);
        f.open(QIODevice::WriteOnly);
        f.write("{ not valid json {{{{");

        StudyService svc;
        const auto studies = svc.loadStudies(path);
        QVERIFY(!studies.isEmpty());
    }

    void loadStudies_jsonObjectNotArray_returnsFallback()
    {
        QTemporaryDir tmp;
        QVERIFY(tmp.isValid());
        const QString path = tmp.filePath("object.json");
        QFile f(path);
        f.open(QIODevice::WriteOnly);
        f.write(QJsonDocument(QJsonObject{{"key", "value"}}).toJson());

        StudyService svc;
        const auto studies = svc.loadStudies(path);
        QVERIFY(!studies.isEmpty());
    }

    // ----------------------------------------------------------------
    // Edge cases
    // ----------------------------------------------------------------

    void loadStudies_emptyArray_returnsEmptyList()
    {
        QTemporaryDir tmp;
        QVERIFY(tmp.isValid());
        const QString path = writeStudiesJson(QDir(tmp.path()), "empty.json", QJsonArray{});

        StudyService svc;
        const auto studies = svc.loadStudies(path);
        QCOMPARE(studies.size(), 0);
    }

    void loadStudies_extraUnknownFields_parsedWithoutError()
    {
        QTemporaryDir tmp;
        QVERIFY(tmp.isValid());

        QJsonObject obj = makeStudyObj("X001");
        obj["unknownField"] = "some_value";
        obj["anotherExtra"] = 42;
        QJsonArray arr;
        arr.append(obj);
        const QString path = writeStudiesJson(QDir(tmp.path()), "studies.json", arr);

        StudyService svc;
        const auto studies = svc.loadStudies(path);
        QCOMPARE(studies.size(), 1);
        QCOMPARE(studies.at(0).studyId, QStringLiteral("X001"));
    }

    void loadStudies_fallbackCount_isThree()
    {
        StudyService svc;
        const auto studies = svc.loadStudies("/no/such/file.json");
        QCOMPARE(studies.size(), 3);
    }

    void loadStudies_fallbackStudies_allHaveNonEmptyStudyId()
    {
        StudyService svc;
        const auto studies = svc.loadStudies("/no/such/file.json");
        for (const auto &s : studies) {
            QVERIFY(!s.studyId.isEmpty());
        }
    }
};

QTEST_MAIN(tst_StudyService)
#include "tst_StudyService.moc"
