#pragma once

#include "models/Study.h"
#include "models/PipelineConfig.h"
#include <QStringList>

class ValidationService {
public:
    QStringList validate(const Study &study, const PipelineConfig &config) const;
};
