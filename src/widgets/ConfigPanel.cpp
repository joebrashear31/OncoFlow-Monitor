#include "ConfigPanel.h"
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
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

    auto *btnRow = new QHBoxLayout;
    auto *defaultsBtn = new QPushButton(tr("Restore Defaults"));
    auto *savePresetBtn = new QPushButton(tr("Save Preset..."));
    savePresetBtn->setEnabled(false); // placeholder
    btnRow->addWidget(defaultsBtn);
    btnRow->addWidget(savePresetBtn);
    btnRow->addStretch();

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(form);
    mainLayout->addLayout(btnRow);
    mainLayout->addStretch();

    connect(browseBtn, &QPushButton::clicked, this, &ConfigPanel::browseOutputDir);
    connect(defaultsBtn, &QPushButton::clicked, this, &ConfigPanel::restoreDefaults);

    // Emit configChanged on any edit
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
