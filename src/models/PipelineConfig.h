#pragma once

#include <QString>

struct PipelineConfig {
    QString modelVersion = "SegModel_v1.2";
    QString registrationMode = "Rigid";
    double threshold = 0.85;
    bool smoothingEnabled = true;
    QString outputDir = "./output";
    bool strictValidation = true;
    bool saveIntermediates = false;
};
