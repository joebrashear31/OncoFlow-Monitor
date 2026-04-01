#pragma once

#include <QWidget>
#include <QImage>

class QLabel;
class QPushButton;
class QTimer;
class QMediaPlayer;
class QVideoSink;
class FrameSource;

class FrameViewer : public QWidget
{
    Q_OBJECT
public:
    explicit FrameViewer(QWidget *parent = nullptr);
    ~FrameViewer() override;

    // Feed a single frame directly (for live hardware use)
    void setFrame(const QImage &image);
    void clearFrame();

    // Image-sequence playback
    bool loadFolder(const QString &folderPath);

    // Video file playback (.mp4, .mov, .avi, .mkv, .m4v)
    bool loadVideo(const QString &filePath);

    void play();
    void stop();
    bool isPlaying() const;

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onNextFrame();
    void onLoadFolderClicked();
    void onLoadVideoClicked();
    void onPlayPauseClicked();

private:
    void updatePixmap();
    void updateControls();

    QImage        m_currentImage;
    QLabel       *m_imageLabel      = nullptr;
    QPushButton  *m_loadFolderBtn   = nullptr;
    QPushButton  *m_loadVideoBtn    = nullptr;
    QPushButton  *m_playBtn         = nullptr;
    QLabel       *m_frameInfo       = nullptr;

    // Image-sequence mode
    QTimer       *m_timer           = nullptr;
    FrameSource  *m_source          = nullptr;
    int           m_currentIndex    = 0;

    // Video mode
    QMediaPlayer *m_player          = nullptr;
    QVideoSink   *m_videoSink       = nullptr;
    bool          m_videoMode       = false;
};
