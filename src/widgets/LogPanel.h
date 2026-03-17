#pragma once

#include "models/LogEntry.h"
#include <QWidget>
#include <QTreeWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QList>

class LogPanel : public QWidget {
    Q_OBJECT
public:
    explicit LogPanel(QWidget *parent = nullptr);
    void appendLog(const LogEntry &entry);
    void clearLogs();

private slots:
    void applyFilters();
    void exportLogs();

private:
    QLineEdit *m_searchBar;
    QCheckBox *m_showInfo;
    QCheckBox *m_showWarning;
    QCheckBox *m_showError;
    QTreeWidget *m_tree;
    QList<LogEntry> m_entries;
};
