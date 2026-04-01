// TDD tests for ValidationService — written before verifying (RED), then greened.

#include <QtTest/QtTest>
#include "services/ValidationService.h"
#include "models/Study.h"
#include "models/PipelineConfig.h"
#include "TestHelpers.h"

class tst_ValidationService : public QObject
{
    Q_OBJECT

private slots:

    // ----------------------------------------------------------------
    // Happy path — no errors
    // ----------------------------------------------------------------

    void validate_validInputs_returnsEmptyList()
    {
        ValidationService svc;
        Study s = makeStudy();
        PipelineConfig cfg;
        QVERIFY(svc.validate(s, cfg).isEmpty());
    }

    // ----------------------------------------------------------------
    // Study field errors
    // ----------------------------------------------------------------

    void validate_emptyStudyId_returnsError()
    {
        ValidationService svc;
        Study s = makeStudy();
        s.studyId.clear();
        PipelineConfig cfg;
        const auto errors = svc.validate(s, cfg);
        QVERIFY(!errors.isEmpty());
    }

    // ----------------------------------------------------------------
    // Config field errors
    // ----------------------------------------------------------------

    void validate_emptyModelVersion_returnsError()
    {
        ValidationService svc;
        Study s = makeStudy();
        PipelineConfig cfg;
        cfg.modelVersion.clear();
        const auto errors = svc.validate(s, cfg);
        QVERIFY(!errors.isEmpty());
    }

    void validate_emptyRegistrationMode_returnsError()
    {
        ValidationService svc;
        Study s = makeStudy();
        PipelineConfig cfg;
        cfg.registrationMode.clear();
        QVERIFY(!svc.validate(s, cfg).isEmpty());
    }

    // ----------------------------------------------------------------
    // Threshold boundary tests
    // ----------------------------------------------------------------

    void validate_thresholdNegative_returnsError()
    {
        ValidationService svc;
        Study s = makeStudy();
        PipelineConfig cfg;
        cfg.threshold = -0.01;
        QVERIFY(!svc.validate(s, cfg).isEmpty());
    }

    void validate_thresholdAboveOne_returnsError()
    {
        ValidationService svc;
        Study s = makeStudy();
        PipelineConfig cfg;
        cfg.threshold = 1.01;
        QVERIFY(!svc.validate(s, cfg).isEmpty());
    }

    void validate_thresholdZeroBoundary_noError()
    {
        ValidationService svc;
        Study s = makeStudy();
        PipelineConfig cfg;
        cfg.threshold = 0.0;
        QVERIFY(svc.validate(s, cfg).isEmpty());
    }

    void validate_thresholdOneBoundary_noError()
    {
        ValidationService svc;
        Study s = makeStudy();
        PipelineConfig cfg;
        cfg.threshold = 1.0;
        QVERIFY(svc.validate(s, cfg).isEmpty());
    }

    // ----------------------------------------------------------------
    // Output directory errors
    // ----------------------------------------------------------------

    void validate_emptyOutputDir_returnsError()
    {
        ValidationService svc;
        Study s = makeStudy();
        PipelineConfig cfg;
        cfg.outputDir.clear();
        QVERIFY(!svc.validate(s, cfg).isEmpty());
    }

    void validate_whitespaceOnlyOutputDir_returnsError()
    {
        ValidationService svc;
        Study s = makeStudy();
        PipelineConfig cfg;
        cfg.outputDir = QStringLiteral("   ");
        QVERIFY(!svc.validate(s, cfg).isEmpty());
    }

    // ----------------------------------------------------------------
    // Multiple simultaneous errors
    // ----------------------------------------------------------------

    void validate_multipleErrors_returnsAllErrors()
    {
        ValidationService svc;
        Study s = makeStudy();
        s.studyId.clear();
        PipelineConfig cfg;
        cfg.modelVersion.clear();
        cfg.registrationMode.clear();
        // 3 distinct errors: studyId, modelVersion, registrationMode
        const auto errors = svc.validate(s, cfg);
        QVERIFY(errors.size() >= 3);
    }

    void validate_allFieldsInvalid_fiveErrors()
    {
        ValidationService svc;
        Study s;
        // studyId is empty (default)
        PipelineConfig cfg;
        cfg.modelVersion.clear();
        cfg.registrationMode.clear();
        cfg.threshold = -1.0;
        cfg.outputDir.clear();
        const auto errors = svc.validate(s, cfg);
        QCOMPARE(errors.size(), 5);
    }
};

QTEST_MAIN(tst_ValidationService)
#include "tst_ValidationService.moc"
