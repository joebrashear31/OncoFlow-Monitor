#pragma once

#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QTreeWidget>
#include <QElapsedTimer>
#include <QTimer>

class StatusPanel : public QWidget {
    Q_OBJECT
public:
    explicit StatusPanel(QWidget *parent = nullptr);

    void setStudyId(const QString &id);
    void setRunStatus(const QString &status);
    void setCurrentStage(const QString &stage);
    void setProgress(int percent);
    void setStageState(const QString &stage, const QString &state);
    void startTimer();
    void stopTimer();
    void reset();

private:
    static const QStringList &stageNames();
    QTreeWidgetItem *stageItem(const QString &stage) const;

    QLabel *m_studyIdLabel;
    QLabel *m_statusLabel;
    QLabel *m_stageLabel;
    QLabel *m_elapsedLabel;
    QProgressBar *m_progressBar;
    QTreeWidget *m_stageTree;
    QElapsedTimer m_elapsed;
    QTimer *m_tickTimer;
};
