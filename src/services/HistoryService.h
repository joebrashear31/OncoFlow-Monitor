#pragma once

#include "models/PipelineRun.h"
#include <QList>
#include <QString>

class HistoryService {
public:
    explicit HistoryService(const QString &historyDir);
    QList<PipelineRun> loadHistory() const;
    void saveRun(const PipelineRun &run);

private:
    QString m_filePath;
};
