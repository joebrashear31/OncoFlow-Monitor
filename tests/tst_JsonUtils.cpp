// TDD tests for JsonUtils::configToJson / configFromJson

#include <QtTest/QtTest>
#include <QJsonObject>
#include <QJsonValue>

#include "utils/JsonUtils.h"
#include "models/PipelineConfig.h"

class tst_JsonUtils : public QObject
{
    Q_OBJECT

private slots:

    // ----------------------------------------------------------------
    // configToJson
    // ----------------------------------------------------------------

    void configToJson_defaultConfig_hasSevenKeys()
    {
        PipelineConfig cfg;
        const QJsonObject obj = JsonUtils::configToJson(cfg);
        QCOMPARE(obj.keys().size(), 7);
    }

    void configToJson_thresholdRoundTripsAsDouble()
    {
        PipelineConfig cfg;
        cfg.threshold = 0.75;
        const QJsonObject obj = JsonUtils::configToJson(cfg);
        QCOMPARE(obj["threshold"].toDouble(), 0.75);
    }

    void configToJson_smoothingEnabledIsJsonBool()
    {
        PipelineConfig cfg;
        cfg.smoothingEnabled = true;
        const QJsonObject obj = JsonUtils::configToJson(cfg);
        QVERIFY(obj["smoothingEnabled"].isBool());
        QVERIFY(obj["smoothingEnabled"].toBool());
    }

    void configToJson_saveIntermediatesIsJsonBool()
    {
        PipelineConfig cfg;
        cfg.saveIntermediates = false;
        const QJsonObject obj = JsonUtils::configToJson(cfg);
        QVERIFY(obj["saveIntermediates"].isBool());
        QVERIFY(!obj["saveIntermediates"].toBool());
    }

    // ----------------------------------------------------------------
    // configFromJson
    // ----------------------------------------------------------------

    void configFromJson_completeObject_allSevenFieldsMatch()
    {
        QJsonObject obj;
        obj["modelVersion"]      = QStringLiteral("SegModel_v2.0");
        obj["registrationMode"]  = QStringLiteral("Affine");
        obj["threshold"]         = 0.42;
        obj["smoothingEnabled"]  = false;
        obj["outputDir"]         = QStringLiteral("/tmp/out");
        obj["strictValidation"]  = false;
        obj["saveIntermediates"] = true;

        const PipelineConfig cfg = JsonUtils::configFromJson(obj);
        QCOMPARE(cfg.modelVersion,     QStringLiteral("SegModel_v2.0"));
        QCOMPARE(cfg.registrationMode, QStringLiteral("Affine"));
        QCOMPARE(cfg.threshold,        0.42);
        QVERIFY(!cfg.smoothingEnabled);
        QCOMPARE(cfg.outputDir,        QStringLiteral("/tmp/out"));
        QVERIFY(!cfg.strictValidation);
        QVERIFY(cfg.saveIntermediates);
    }

    void configFromJson_emptyObject_returnsDefaults()
    {
        const PipelineConfig defaults;
        const PipelineConfig cfg = JsonUtils::configFromJson(QJsonObject{});
        QCOMPARE(cfg.modelVersion,     defaults.modelVersion);
        QCOMPARE(cfg.registrationMode, defaults.registrationMode);
        QCOMPARE(cfg.threshold,        defaults.threshold);
        QCOMPARE(cfg.smoothingEnabled, defaults.smoothingEnabled);
        QCOMPARE(cfg.outputDir,        defaults.outputDir);
        QCOMPARE(cfg.strictValidation, defaults.strictValidation);
        QCOMPARE(cfg.saveIntermediates, defaults.saveIntermediates);
    }

    void configFromJson_partialObject_missingFieldsGetDefaults()
    {
        // Only modelVersion is provided; everything else should be default.
        QJsonObject obj;
        obj["modelVersion"] = QStringLiteral("SegModel_v1.0");

        const PipelineConfig defaults;
        const PipelineConfig cfg = JsonUtils::configFromJson(obj);
        QCOMPARE(cfg.modelVersion,     QStringLiteral("SegModel_v1.0"));
        QCOMPARE(cfg.registrationMode, defaults.registrationMode);
        QCOMPARE(cfg.threshold,        defaults.threshold);
    }

    void configToJson_thenFromJson_roundTrip()
    {
        PipelineConfig original;
        const QJsonObject json = JsonUtils::configToJson(original);
        const PipelineConfig restored = JsonUtils::configFromJson(json);

        QCOMPARE(restored.modelVersion,     original.modelVersion);
        QCOMPARE(restored.registrationMode, original.registrationMode);
        QCOMPARE(restored.threshold,        original.threshold);
        QCOMPARE(restored.smoothingEnabled, original.smoothingEnabled);
        QCOMPARE(restored.outputDir,        original.outputDir);
        QCOMPARE(restored.strictValidation, original.strictValidation);
        QCOMPARE(restored.saveIntermediates, original.saveIntermediates);
    }

    void configToJson_nonDefaultValues_roundTrip()
    {
        PipelineConfig original;
        original.modelVersion     = QStringLiteral("SegModel_v2.0");
        original.registrationMode = QStringLiteral("Deformable");
        original.threshold        = 0.33;
        original.smoothingEnabled = false;
        original.outputDir        = QStringLiteral("/custom/output");
        original.strictValidation = false;
        original.saveIntermediates = true;

        const QJsonObject json = JsonUtils::configToJson(original);
        const PipelineConfig restored = JsonUtils::configFromJson(json);

        QCOMPARE(restored.modelVersion,      original.modelVersion);
        QCOMPARE(restored.registrationMode,  original.registrationMode);
        QCOMPARE(restored.threshold,         original.threshold);
        QCOMPARE(restored.smoothingEnabled,  original.smoothingEnabled);
        QCOMPARE(restored.outputDir,         original.outputDir);
        QCOMPARE(restored.strictValidation,  original.strictValidation);
        QCOMPARE(restored.saveIntermediates, original.saveIntermediates);
    }

    void configFromJson_wrongTypeForThreshold_usesDefault()
    {
        // threshold is a string — toDouble() should fall back to default
        QJsonObject obj;
        obj["threshold"] = QStringLiteral("not-a-number");

        const PipelineConfig defaults;
        const PipelineConfig cfg = JsonUtils::configFromJson(obj);
        // QJsonValue::toDouble(default) returns the default when type is wrong
        QCOMPARE(cfg.threshold, defaults.threshold);
    }
};

QTEST_MAIN(tst_JsonUtils)
#include "tst_JsonUtils.moc"
