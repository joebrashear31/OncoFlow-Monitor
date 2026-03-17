#include "LogPanel.h"
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QTextStream>
#include <QVBoxLayout>

LogPanel::LogPanel(QWidget *parent) : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // Filter row
    auto *filterRow = new QHBoxLayout;
    m_searchBar = new QLineEdit;
    m_searchBar->setPlaceholderText(tr("Filter logs..."));
    m_searchBar->setClearButtonEnabled(true);
    filterRow->addWidget(m_searchBar);

    m_showInfo = new QCheckBox(tr("Info"));
    m_showInfo->setChecked(true);
    m_showWarning = new QCheckBox(tr("Warning"));
    m_showWarning->setChecked(true);
    m_showError = new QCheckBox(tr("Error"));
    m_showError->setChecked(true);
    filterRow->addWidget(m_showInfo);
    filterRow->addWidget(m_showWarning);
    filterRow->addWidget(m_showError);

    auto *clearBtn = new QPushButton(tr("Clear"));
    auto *exportBtn = new QPushButton(tr("Export..."));
    filterRow->addWidget(clearBtn);
    filterRow->addWidget(exportBtn);
    layout->addLayout(filterRow);

    m_tree = new QTreeWidget;
    m_tree->setHeaderLabels({tr("Time"), tr("Severity"), tr("Message")});
    m_tree->setRootIsDecorated(false);
    m_tree->setAlternatingRowColors(true);
    m_tree->header()->setStretchLastSection(true);
    m_tree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_tree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    layout->addWidget(m_tree);

    connect(m_searchBar, &QLineEdit::textChanged, this, &LogPanel::applyFilters);
    connect(m_showInfo, &QCheckBox::checkStateChanged, this, &LogPanel::applyFilters);
    connect(m_showWarning, &QCheckBox::checkStateChanged, this, &LogPanel::applyFilters);
    connect(m_showError, &QCheckBox::checkStateChanged, this, &LogPanel::applyFilters);
    connect(clearBtn, &QPushButton::clicked, this, &LogPanel::clearLogs);
    connect(exportBtn, &QPushButton::clicked, this, &LogPanel::exportLogs);
}

void LogPanel::appendLog(const LogEntry &entry)
{
    m_entries.append(entry);

    // Check if it passes current filters
    QString filter = m_searchBar->text();
    bool show = true;
    if (entry.severity == "Info" && !m_showInfo->isChecked()) show = false;
    if (entry.severity == "Warning" && !m_showWarning->isChecked()) show = false;
    if (entry.severity == "Error" && !m_showError->isChecked()) show = false;
    if (!filter.isEmpty() && !entry.message.contains(filter, Qt::CaseInsensitive)) show = false;

    if (show) {
        auto *item = new QTreeWidgetItem(m_tree);
        item->setText(0, entry.timestamp.toString("hh:mm:ss.zzz"));
        item->setText(1, entry.severity);
        item->setText(2, entry.message);
        if (entry.severity == "Warning") item->setForeground(1, QColor(200, 150, 0));
        else if (entry.severity == "Error") item->setForeground(1, Qt::red);
        m_tree->scrollToBottom();
    }
}

void LogPanel::clearLogs()
{
    m_entries.clear();
    m_tree->clear();
}

void LogPanel::applyFilters()
{
    m_tree->clear();
    QString filter = m_searchBar->text();
    for (const auto &e : m_entries) {
        if (e.severity == "Info" && !m_showInfo->isChecked()) continue;
        if (e.severity == "Warning" && !m_showWarning->isChecked()) continue;
        if (e.severity == "Error" && !m_showError->isChecked()) continue;
        if (!filter.isEmpty() && !e.message.contains(filter, Qt::CaseInsensitive)) continue;

        auto *item = new QTreeWidgetItem(m_tree);
        item->setText(0, e.timestamp.toString("hh:mm:ss.zzz"));
        item->setText(1, e.severity);
        item->setText(2, e.message);
        if (e.severity == "Warning") item->setForeground(1, QColor(200, 150, 0));
        else if (e.severity == "Error") item->setForeground(1, Qt::red);
    }
}

void LogPanel::exportLogs()
{
    QString path = QFileDialog::getSaveFileName(this, tr("Export Logs"), "logs.txt", tr("Text Files (*.txt)"));
    if (path.isEmpty()) return;
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&file);
    for (const auto &e : m_entries) {
        out << e.timestamp.toString("yyyy-MM-dd hh:mm:ss.zzz") << " [" << e.severity << "] " << e.message << "\n";
    }
}
