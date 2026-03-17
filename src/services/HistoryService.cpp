#include "HistoryService.h"
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

HistoryService::HistoryService(const QString &historyDir)
{
    QDir dir(historyDir);
    if (!dir.exists()) dir.mkpath(".");
    m_filePath = dir.filePath("runs.json");
}

QList<PipelineRun> HistoryService::loadHistory() const
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly)) return {};

    auto doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isArray()) return {};

    QList<PipelineRun> runs;
    for (const auto &val : doc.array()) {
        auto obj = val.toObject();
        PipelineRun r;
        r.runId = obj["runId"].toString();
        r.studyId = obj["studyId"].toString();
        r.startTime = QDateTime::fromString(obj["startTime"].toString(), Qt::ISODate);
        r.endTime = QDateTime::fromString(obj["endTime"].toString(), Qt::ISODate);
        r.status = obj["status"].toString();
        r.currentStage = obj["currentStage"].toString();
        r.outputPath = obj["outputPath"].toString();
        runs.append(r);
    }
    return runs;
}

void HistoryService::saveRun(const PipelineRun &run)
{
    auto runs = loadHistory();
    runs.append(run);

    QJsonArray arr;
    for (const auto &r : runs) {
        QJsonObject obj;
        obj["runId"] = r.runId;
        obj["studyId"] = r.studyId;
        obj["startTime"] = r.startTime.toString(Qt::ISODate);
        obj["endTime"] = r.endTime.toString(Qt::ISODate);
        obj["status"] = r.status;
        obj["currentStage"] = r.currentStage;
        obj["outputPath"] = r.outputPath;
        arr.append(obj);
    }

    QFile file(m_filePath);
    if (file.open(QIODevice::WriteOnly))
        file.write(QJsonDocument(arr).toJson());
}
