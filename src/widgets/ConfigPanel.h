#pragma once

#include "models/PipelineConfig.h"
#include <QWidget>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>

class ConfigPanel : public QWidget {
    Q_OBJECT
public:
    explicit ConfigPanel(QWidget *parent = nullptr);
    PipelineConfig currentConfig() const;
    void loadConfig(const PipelineConfig &cfg);

signals:
    void configChanged();

private slots:
    void restoreDefaults();
    void browseOutputDir();

private:
    QComboBox *m_modelVersion;
    QComboBox *m_registrationMode;
    QDoubleSpinBox *m_threshold;
    QCheckBox *m_smoothing;
    QLineEdit *m_outputDir;
    QCheckBox *m_strictValidation;
    QCheckBox *m_saveIntermediates;
};
