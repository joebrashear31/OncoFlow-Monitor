#include "FrameViewer.h"
#include "services/FrameSource.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMediaPlayer>
#include <QPixmap>
#include <QPushButton>
#include <QResizeEvent>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>
#include <QVideoFrame>
#include <QVideoSink>

static constexpr int kPlaybackIntervalMs = 100; // ~10 fps for image sequences

static QString formatMs(qint64 ms)
{
    qint64 s = ms / 1000;
    return QString("%1:%2").arg(s / 60, 2, 10, QChar('0')).arg(s % 60, 2, 10, QChar('0'));
}

FrameViewer::FrameViewer(QWidget *parent)
    : QWidget(parent)
    , m_imageLabel(new QLabel(this))
    , m_loadFolderBtn(new QPushButton(tr("Load Folder"), this))
    , m_loadVideoBtn(new QPushButton(tr("Load Video"), this))
    , m_playBtn(new QPushButton(tr("Play"), this))
    , m_frameInfo(new QLabel(tr("No frames"), this))
    , m_timer(new QTimer(this))
    , m_source(new FrameSource)
    , m_player(new QMediaPlayer(this))
    , m_videoSink(new QVideoSink(this))
{
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setMinimumSize(280, 200);
    m_imageLabel->setText("No frame loaded");
    m_imageLabel->setStyleSheet("background: #111; color: #666;");

    m_playBtn->setEnabled(false);
    m_frameInfo->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    auto *controls = new QHBoxLayout;
    controls->addWidget(m_loadFolderBtn);
    controls->addWidget(m_loadVideoBtn);
    controls->addWidget(m_playBtn);
    controls->addStretch();
    controls->addWidget(m_frameInfo);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_imageLabel, 1);
    layout->addLayout(controls);
    setLayout(layout);

    m_player->setVideoOutput(m_videoSink);

    connect(m_loadFolderBtn, &QPushButton::clicked, this, &FrameViewer::onLoadFolderClicked);
    connect(m_loadVideoBtn,  &QPushButton::clicked, this, &FrameViewer::onLoadVideoClicked);
    connect(m_playBtn,       &QPushButton::clicked, this, &FrameViewer::onPlayPauseClicked);
    connect(m_timer,         &QTimer::timeout,      this, &FrameViewer::onNextFrame);

    connect(m_videoSink, &QVideoSink::videoFrameChanged, this,
        [this](const QVideoFrame &frame) {
            QImage img = frame.toImage();
            if (!img.isNull())
                setFrame(img);
        }, Qt::QueuedConnection);

    connect(m_player, &QMediaPlayer::playbackStateChanged, this,
        [this](QMediaPlayer::PlaybackState) { updateControls(); });

    connect(m_player, &QMediaPlayer::durationChanged, this,
        [this](qint64) { updateControls(); });

    connect(m_player, &QMediaPlayer::positionChanged, this,
        [this](qint64) { updateControls(); });
}

FrameViewer::~FrameViewer()
{
    delete m_source;
}

void FrameViewer::setFrame(const QImage &image)
{
    m_currentImage = image;
    updatePixmap();
}

void FrameViewer::clearFrame()
{
    m_currentImage = QImage();
    m_imageLabel->setText("No frame loaded");
    m_imageLabel->setPixmap(QPixmap());
}

bool FrameViewer::loadFolder(const QString &folderPath)
{
    stop();
    m_videoMode    = false;
    m_currentIndex = 0;

    if (!m_source->loadFromFolder(folderPath))
        return false;

    setFrame(m_source->getFrame(0));
    m_playBtn->setEnabled(true);
    updateControls();
    return true;
}

bool FrameViewer::loadVideo(const QString &filePath)
{
    stop();
    m_videoMode = true;

    m_player->setSource(QUrl::fromLocalFile(filePath));
    m_playBtn->setEnabled(true);
    updateControls();
    return true;
}

void FrameViewer::play()
{
    if (m_videoMode)
        m_player->play();
    else if (m_source->isLoaded())
        m_timer->start(kPlaybackIntervalMs);

    updateControls();
}

void FrameViewer::stop()
{
    if (m_videoMode)
        m_player->pause();
    else
        m_timer->stop();

    updateControls();
}

bool FrameViewer::isPlaying() const
{
    if (m_videoMode)
        return m_player->playbackState() == QMediaPlayer::PlayingState;
    return m_timer->isActive();
}

void FrameViewer::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (!m_currentImage.isNull())
        updatePixmap();
}

void FrameViewer::onNextFrame()
{
    if (!m_source->isLoaded()) return;
    m_currentIndex = (m_currentIndex + 1) % m_source->frameCount();
    setFrame(m_source->getFrame(m_currentIndex));
    updateControls();
}

void FrameViewer::onLoadFolderClicked()
{
    const QString folder = QFileDialog::getExistingDirectory(
        this, tr("Select Frame Folder"), QString());
    if (folder.isEmpty()) return;

    if (!loadFolder(folder))
        m_imageLabel->setText("No supported images found in folder");
}

void FrameViewer::onLoadVideoClicked()
{
    const QString file = QFileDialog::getOpenFileName(
        this, tr("Open Video File"), QString(),
        tr("Video Files (*.mp4 *.mov *.avi *.mkv *.m4v);;All Files (*)"));
    if (file.isEmpty()) return;

    loadVideo(file);
}

void FrameViewer::onPlayPauseClicked()
{
    isPlaying() ? stop() : play();
}

void FrameViewer::updatePixmap()
{
    if (m_currentImage.isNull())
    {
        m_imageLabel->setText("No frame loaded");
        return;
    }
    m_imageLabel->setText("");
    QPixmap pixmap = QPixmap::fromImage(m_currentImage);
    m_imageLabel->setPixmap(
        pixmap.scaled(m_imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void FrameViewer::updateControls()
{
    m_playBtn->setText(isPlaying() ? tr("Pause") : tr("Play"));

    if (m_videoMode)
    {
        qint64 pos = m_player->position();
        qint64 dur = m_player->duration();
        m_frameInfo->setText(dur > 0
            ? QString("%1 / %2").arg(formatMs(pos), formatMs(dur))
            : tr("Loading..."));
    }
    else if (m_source->isLoaded())
    {
        m_frameInfo->setText(tr("%1 / %2").arg(m_currentIndex + 1).arg(m_source->frameCount()));
    }
    else
    {
        m_frameInfo->setText(tr("No frames"));
    }
}
