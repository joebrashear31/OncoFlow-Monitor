#include "ConfigPanel.h"
#include "utils/JsonUtils.h"

#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QJsonDocument>
#include <QMessageBox>
#include <QVBoxLayout>

ConfigPanel::ConfigPanel(QWidget *parent) : QWidget(parent)
{
    auto *form = new QFormLayout;

    m_modelVersion = new QComboBox;
    m_modelVersion->addItems({"SegModel_v1.0", "SegModel_v1.1", "SegModel_v1.2", "SegModel_v2.0"});
    form->addRow(tr("Model Version:"), m_modelVersion);

    m_registrationMode = new QComboBox;
    m_registrationMode->addItems({"Rigid", "Affine", "Deformable"});
    form->addRow(tr("Registration Mode:"), m_registrationMode);

    m_threshold = new QDoubleSpinBox;
    m_threshold->setRange(0.0, 1.0);
    m_threshold->setSingleStep(0.05);
    m_threshold->setDecimals(2);
    form->addRow(tr("Threshold:"), m_threshold);

    m_smoothing = new QCheckBox(tr("Enabled"));
    form->addRow(tr("Smoothing:"), m_smoothing);

    auto *dirRow = new QHBoxLayout;
    m_outputDir = new QLineEdit;
    auto *browseBtn = new QPushButton(tr("Browse..."));
    dirRow->addWidget(m_outputDir);
    dirRow->addWidget(browseBtn);
    form->addRow(tr("Output Directory:"), dirRow);

    m_strictValidation = new QCheckBox(tr("Enabled"));
    form->addRow(tr("Strict Validation:"), m_strictValidation);

    m_saveIntermediates = new QCheckBox(tr("Enabled"));
    form->addRow(tr("Save Intermediates:"), m_saveIntermediates);

    // Presets
    auto *presetRow = new QHBoxLayout;
    m_presetCombo = new QComboBox;
    m_presetCombo->addItem(tr("(Select Preset)"));
    m_savePresetBtn = new QPushButton(tr("Save Preset..."));
    presetRow->addWidget(m_presetCombo, 1);
    presetRow->addWidget(m_savePresetBtn);
    form->addRow(tr("Preset:"), presetRow);

    auto *btnRow = new QHBoxLayout;
    auto *defaultsBtn = new QPushButton(tr("Restore Defaults"));
    btnRow->addWidget(defaultsBtn);
    btnRow->addStretch();

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(form);
    mainLayout->addLayout(btnRow);
    mainLayout->addStretch();

    connect(browseBtn, &QPushButton::clicked, this, &ConfigPanel::browseOutputDir);
    connect(defaultsBtn, &QPushButton::clicked, this, &ConfigPanel::restoreDefaults);
    connect(m_savePresetBtn, &QPushButton::clicked, this, &ConfigPanel::savePreset);
    connect(m_presetCombo, &QComboBox::currentTextChanged, this, [this](const QString &name) {
        if (name != tr("(Select Preset)"))
            loadPreset(name);
    });

    connect(m_modelVersion, &QComboBox::currentIndexChanged, this, &ConfigPanel::configChanged);
    connect(m_registrationMode, &QComboBox::currentIndexChanged, this, &ConfigPanel::configChanged);
    connect(m_threshold, &QDoubleSpinBox::valueChanged, this, &ConfigPanel::configChanged);
    connect(m_smoothing, &QCheckBox::checkStateChanged, this, &ConfigPanel::configChanged);
    connect(m_outputDir, &QLineEdit::textChanged, this, &ConfigPanel::configChanged);
    connect(m_strictValidation, &QCheckBox::checkStateChanged, this, &ConfigPanel::configChanged);
    connect(m_saveIntermediates, &QCheckBox::checkStateChanged, this, &ConfigPanel::configChanged);

    restoreDefaults();
}

PipelineConfig ConfigPanel::currentConfig() const
{
    return {
        m_modelVersion->currentText(),
        m_registrationMode->currentText(),
        m_threshold->value(),
        m_smoothing->isChecked(),
        m_outputDir->text(),
        m_strictValidation->isChecked(),
        m_saveIntermediates->isChecked()
    };
}

void ConfigPanel::loadConfig(const PipelineConfig &cfg)
{
    m_modelVersion->setCurrentText(cfg.modelVersion);
    m_registrationMode->setCurrentText(cfg.registrationMode);
    m_threshold->setValue(cfg.threshold);
    m_smoothing->setChecked(cfg.smoothingEnabled);
    m_outputDir->setText(cfg.outputDir);
    m_strictValidation->setChecked(cfg.strictValidation);
    m_saveIntermediates->setChecked(cfg.saveIntermediates);
}

void ConfigPanel::setPresetsDir(const QString &dir)
{
    m_presetsDir = dir;
    QDir d(dir);
    if (!d.exists()) d.mkpath(".");
    refreshPresetList();
}

void ConfigPanel::restoreDefaults()
{
    loadConfig(PipelineConfig{});
}

void ConfigPanel::browseOutputDir()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Output Directory"), m_outputDir->text());
    if (!dir.isEmpty())
        m_outputDir->setText(dir);
}

void ConfigPanel::savePreset()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("Save Preset"), tr("Preset name:"),
                                          QLineEdit::Normal, {}, &ok);
    if (!ok || name.trimmed().isEmpty()) return;

    QDir dir(m_presetsDir);
    if (!dir.exists()) dir.mkpath(".");

    QFile file(dir.filePath(name + ".json"));
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("Error"), tr("Could not save preset."));
        return;
    }
    file.write(QJsonDocument(JsonUtils::configToJson(currentConfig())).toJson());
    refreshPresetList();
}

void ConfigPanel::loadPreset(const QString &name)
{
    QFile file(QDir(m_presetsDir).filePath(name + ".json"));
    if (!file.open(QIODevice::ReadOnly)) return;

    auto doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isObject())
        loadConfig(JsonUtils::configFromJson(doc.object()));
}

void ConfigPanel::refreshPresetList()
{
    m_presetCombo->blockSignals(true);
    m_presetCombo->clear();
    m_presetCombo->addItem(tr("(Select Preset)"));

    QDir dir(m_presetsDir);
    for (const auto &entry : dir.entryList({"*.json"}, QDir::Files)) {
        m_presetCombo->addItem(entry.chopped(5)); // remove .json
    }
    m_presetCombo->blockSignals(false);
}
