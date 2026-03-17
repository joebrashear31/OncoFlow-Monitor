#pragma once

#include "models/PipelineConfig.h"
#include <QJsonObject>

namespace JsonUtils {

inline QJsonObject configToJson(const PipelineConfig &cfg)
{
    return {
        {"modelVersion", cfg.modelVersion},
        {"registrationMode", cfg.registrationMode},
        {"threshold", cfg.threshold},
        {"smoothingEnabled", cfg.smoothingEnabled},
        {"outputDir", cfg.outputDir},
        {"strictValidation", cfg.strictValidation},
        {"saveIntermediates", cfg.saveIntermediates}
    };
}

inline PipelineConfig configFromJson(const QJsonObject &obj)
{
    PipelineConfig cfg;
    cfg.modelVersion = obj["modelVersion"].toString(cfg.modelVersion);
    cfg.registrationMode = obj["registrationMode"].toString(cfg.registrationMode);
    cfg.threshold = obj["threshold"].toDouble(cfg.threshold);
    cfg.smoothingEnabled = obj["smoothingEnabled"].toBool(cfg.smoothingEnabled);
    cfg.outputDir = obj["outputDir"].toString(cfg.outputDir);
    cfg.strictValidation = obj["strictValidation"].toBool(cfg.strictValidation);
    cfg.saveIntermediates = obj["saveIntermediates"].toBool(cfg.saveIntermediates);
    return cfg;
}

} // namespace JsonUtils
