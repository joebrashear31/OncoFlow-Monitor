// TDD tests for ConfigPanel — written RED first, then greened.

#include <QtTest/QtTest>
#include <QApplication>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSignalSpy>
#include <QTemporaryDir>
#include "widgets/ConfigPanel.h"
#include "models/PipelineConfig.h"

class tst_ConfigPanel : public QObject
{
    Q_OBJECT

private slots:

    // ----------------------------------------------------------------
    // Default state
    // ----------------------------------------------------------------

    void defaultConfig_matchesPipelineConfigDefaults()
    {
        ConfigPanel panel;
        PipelineConfig cfg = panel.currentConfig();
        QCOMPARE(cfg.modelVersion,      QString("SegModel_v1.2"));
        QCOMPARE(cfg.registrationMode,  QString("Rigid"));
        QCOMPARE(cfg.threshold,         0.85);
        QVERIFY(cfg.smoothingEnabled);
        QCOMPARE(cfg.outputDir,         QString("./output"));
        QVERIFY(cfg.strictValidation);
        QVERIFY(!cfg.saveIntermediates);
    }

    // ----------------------------------------------------------------
    // loadConfig / currentConfig roundtrip
    // ----------------------------------------------------------------

    void loadConfig_roundtrip_modelVersion()
    {
        ConfigPanel panel;
        PipelineConfig in;
        in.modelVersion = "SegModel_v2.0";
        panel.loadConfig(in);
        QCOMPARE(panel.currentConfig().modelVersion, in.modelVersion);
    }

    void loadConfig_roundtrip_registrationMode()
    {
        ConfigPanel panel;
        PipelineConfig in;
        in.registrationMode = "Deformable";
        panel.loadConfig(in);
        QCOMPARE(panel.currentConfig().registrationMode, in.registrationMode);
    }

    void loadConfig_roundtrip_threshold()
    {
        ConfigPanel panel;
        PipelineConfig in;
        in.threshold = 0.42;
        panel.loadConfig(in);
        QCOMPARE(panel.currentConfig().threshold, in.threshold);
    }

    void loadConfig_roundtrip_smoothingEnabled()
    {
        ConfigPanel panel;
        PipelineConfig in;
        in.smoothingEnabled = false;
        panel.loadConfig(in);
        QCOMPARE(panel.currentConfig().smoothingEnabled, false);
    }

    void loadConfig_roundtrip_outputDir()
    {
        ConfigPanel panel;
        PipelineConfig in;
        in.outputDir = "/tmp/test_output";
        panel.loadConfig(in);
        QCOMPARE(panel.currentConfig().outputDir, in.outputDir);
    }

    void loadConfig_roundtrip_strictValidation()
    {
        ConfigPanel panel;
        PipelineConfig in;
        in.strictValidation = false;
        panel.loadConfig(in);
        QCOMPARE(panel.currentConfig().strictValidation, false);
    }

    void loadConfig_roundtrip_saveIntermediates()
    {
        ConfigPanel panel;
        PipelineConfig in;
        in.saveIntermediates = true;
        panel.loadConfig(in);
        QCOMPARE(panel.currentConfig().saveIntermediates, true);
    }

    void loadConfig_allFields_roundtrip()
    {
        ConfigPanel panel;
        PipelineConfig in;
        in.modelVersion      = "SegModel_v2.0";
        in.registrationMode  = "Affine";
        in.threshold         = 0.55;
        in.smoothingEnabled  = false;
        in.outputDir         = "/tmp/full_roundtrip";
        in.strictValidation  = false;
        in.saveIntermediates = true;
        panel.loadConfig(in);
        PipelineConfig out = panel.currentConfig();
        QCOMPARE(out.modelVersion,      in.modelVersion);
        QCOMPARE(out.registrationMode,  in.registrationMode);
        QCOMPARE(out.threshold,         in.threshold);
        QCOMPARE(out.smoothingEnabled,  in.smoothingEnabled);
        QCOMPARE(out.outputDir,         in.outputDir);
        QCOMPARE(out.strictValidation,  in.strictValidation);
        QCOMPARE(out.saveIntermediates, in.saveIntermediates);
    }

    // ----------------------------------------------------------------
    // configChanged signal
    // ----------------------------------------------------------------

    void configChanged_emittedOnLoadConfigWithDifferentModelVersion()
    {
        ConfigPanel panel;
        QSignalSpy spy(&panel, &ConfigPanel::configChanged);
        PipelineConfig cfg;
        cfg.modelVersion = "SegModel_v1.0"; // differs from default v1.2
        panel.loadConfig(cfg);
        QVERIFY(spy.count() > 0);
    }

    void configChanged_emittedOnThresholdChange()
    {
        ConfigPanel panel;
        QSignalSpy spy(&panel, &ConfigPanel::configChanged);
        auto *spinbox = panel.findChild<QDoubleSpinBox*>();
        QVERIFY(spinbox != nullptr);
        spinbox->setValue(0.10);
        QVERIFY(spy.count() > 0);
    }

    void configChanged_emittedOnSmoothingToggle()
    {
        ConfigPanel panel;
        QSignalSpy spy(&panel, &ConfigPanel::configChanged);
        auto checkboxes = panel.findChildren<QCheckBox*>();
        QVERIFY(!checkboxes.isEmpty());
        // Toggle the first checkbox (smoothing)
        checkboxes.first()->setChecked(!checkboxes.first()->isChecked());
        QVERIFY(spy.count() > 0);
    }

    // ----------------------------------------------------------------
    // Preset: setPresetsDir populates combo with JSON files
    // ----------------------------------------------------------------

    void setPresetsDir_withJsonFile_presetAppearsInCombo()
    {
        QTemporaryDir tmpDir;
        QVERIFY(tmpDir.isValid());

        // Write a minimal preset file
        QJsonObject obj;
        obj["modelVersion"]      = "SegModel_v1.0";
        obj["registrationMode"]  = "Rigid";
        obj["threshold"]         = 0.5;
        obj["smoothingEnabled"]  = false;
        obj["outputDir"]         = "./out";
        obj["strictValidation"]  = false;
        obj["saveIntermediates"] = false;
        QFile f(tmpDir.filePath("mypreset.json"));
        QVERIFY(f.open(QIODevice::WriteOnly));
        f.write(QJsonDocument(obj).toJson());
        f.close();

        ConfigPanel panel;
        panel.setPresetsDir(tmpDir.path());

        // The preset combo should now contain "mypreset"
        QComboBox *presetCombo = nullptr;
        for (auto *cb : panel.findChildren<QComboBox*>()) {
            if (cb->findText("mypreset") >= 0) {
                presetCombo = cb;
                break;
            }
        }
        QVERIFY(presetCombo != nullptr);
    }

    void setPresetsDir_selectingPreset_restoresConfig()
    {
        QTemporaryDir tmpDir;
        QVERIFY(tmpDir.isValid());

        QJsonObject obj;
        obj["modelVersion"]      = "SegModel_v2.0";
        obj["registrationMode"]  = "Deformable";
        obj["threshold"]         = 0.33;
        obj["smoothingEnabled"]  = false;
        obj["outputDir"]         = "/tmp/presetout";
        obj["strictValidation"]  = false;
        obj["saveIntermediates"] = true;
        QFile f(tmpDir.filePath("testpreset.json"));
        QVERIFY(f.open(QIODevice::WriteOnly));
        f.write(QJsonDocument(obj).toJson());
        f.close();

        ConfigPanel panel;
        panel.setPresetsDir(tmpDir.path());

        // Trigger loading by selecting the preset in the combo
        for (auto *cb : panel.findChildren<QComboBox*>()) {
            int idx = cb->findText("testpreset");
            if (idx >= 0) {
                cb->setCurrentIndex(idx);
                break;
            }
        }
        QApplication::processEvents();

        PipelineConfig cfg = panel.currentConfig();
        QCOMPARE(cfg.modelVersion, QString("SegModel_v2.0"));
        QCOMPARE(cfg.registrationMode, QString("Deformable"));
        QCOMPARE(cfg.saveIntermediates, true);
    }
};

QTEST_MAIN(tst_ConfigPanel)
#include "tst_ConfigPanel.moc"
