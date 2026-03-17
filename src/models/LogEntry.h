#pragma once

#include <QString>
#include <QDateTime>

struct LogEntry {
    QDateTime timestamp;
    QString severity;  // "Info", "Warning", "Error"
    QString message;
    QString runId;
};
