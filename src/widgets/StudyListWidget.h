#pragma once

#include "models/Study.h"

#include <QWidget>
#include <QLineEdit>
#include <QTreeWidget>
#include <QList>

class StudyListWidget : public QWidget {
    Q_OBJECT

public:
    explicit StudyListWidget(QWidget *parent = nullptr);

    void setStudies(const QList<Study> &studies);
    Study selectedStudy() const;
    bool hasSelection() const;

signals:
    void studySelected(const Study &study);

private slots:
    void onSearchChanged(const QString &text);
    void onSelectionChanged();

private:
    void populateTree(const QString &filter = {});

    QLineEdit *m_searchBar = nullptr;
    QTreeWidget *m_tree = nullptr;
    QList<Study> m_studies;
};
