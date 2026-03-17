#include "StudyListWidget.h"

#include <QHeaderView>
#include <QVBoxLayout>

StudyListWidget::StudyListWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_searchBar = new QLineEdit;
    m_searchBar->setPlaceholderText(tr("Search studies..."));
    m_searchBar->setClearButtonEnabled(true);
    layout->addWidget(m_searchBar);

    m_tree = new QTreeWidget;
    m_tree->setHeaderLabels({tr("Study ID"), tr("Patient"), tr("Modality"), tr("Date"), tr("Status")});
    m_tree->setRootIsDecorated(false);
    m_tree->setAlternatingRowColors(true);
    m_tree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tree->header()->setStretchLastSection(true);
    m_tree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    layout->addWidget(m_tree);

    connect(m_searchBar, &QLineEdit::textChanged, this, &StudyListWidget::onSearchChanged);
    connect(m_tree, &QTreeWidget::itemSelectionChanged, this, &StudyListWidget::onSelectionChanged);
}

void StudyListWidget::setStudies(const QList<Study> &studies)
{
    m_studies = studies;
    populateTree();
}

Study StudyListWidget::selectedStudy() const
{
    auto items = m_tree->selectedItems();
    if (items.isEmpty()) return {};

    int row = items.first()->data(0, Qt::UserRole).toInt();
    if (row >= 0 && row < m_studies.size())
        return m_studies.at(row);
    return {};
}

bool StudyListWidget::hasSelection() const
{
    return !m_tree->selectedItems().isEmpty();
}

void StudyListWidget::onSearchChanged(const QString &text)
{
    populateTree(text);
}

void StudyListWidget::onSelectionChanged()
{
    if (hasSelection())
        emit studySelected(selectedStudy());
}

void StudyListWidget::populateTree(const QString &filter)
{
    m_tree->clear();
    for (int i = 0; i < m_studies.size(); ++i) {
        const auto &s = m_studies.at(i);
        if (!filter.isEmpty()) {
            bool match = s.studyId.contains(filter, Qt::CaseInsensitive)
                      || s.patientAlias.contains(filter, Qt::CaseInsensitive)
                      || s.modality.contains(filter, Qt::CaseInsensitive);
            if (!match) continue;
        }

        auto *item = new QTreeWidgetItem(m_tree);
        item->setText(0, s.studyId);
        item->setText(1, s.patientAlias);
        item->setText(2, s.modality);
        item->setText(3, s.acquisitionDate.toString(Qt::ISODate));
        item->setText(4, s.status);
        item->setData(0, Qt::UserRole, i);
    }
}
