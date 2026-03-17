#include "ValidationService.h"

QStringList ValidationService::validate(const Study &study, const PipelineConfig &config) const
{
    QStringList errors;
    if (study.studyId.isEmpty())
        errors << "No study selected.";
    if (config.modelVersion.isEmpty())
        errors << "Model version must not be empty.";
    if (config.registrationMode.isEmpty())
        errors << "Registration mode must not be empty.";
    if (config.threshold < 0.0 || config.threshold > 1.0)
        errors << "Threshold must be between 0.0 and 1.0.";
    if (config.outputDir.trimmed().isEmpty())
        errors << "Output directory must not be empty.";
    return errors;
}
