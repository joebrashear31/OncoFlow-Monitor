OncoFlow Monitor is a Qt/C++ desktop application for configuring and monitoring a simulated medical imaging processing pipeline. It provides an operator-facing UI for study selection, parameter validation, real-time job tracking, logging, and run history review. The project is designed to demonstrate desktop systems engineering patterns commonly used in medical imaging, clinical software, and scientific workflow applications.

## Build

```bash
mkdir build && cd build
cmake ..
make
./OncoFlowMonitor
```

## Requirements

- Qt 6
- CMake 3.16+
- C++17 compiler

## Frame Viewer

The Frame Viewer panel (top-right) displays image sequences and is the foundation for live hardware video.

### Loading images for a test

1. Collect a folder of `.png` or `.jpg` / `.jpeg` images (`.bmp` also works).
   Each file is treated as one frame; files are sorted by name, so zero-pad numbering works well:
   ```
   frames/
     frame_001.jpg
     frame_002.jpg
     frame_003.jpg
     ...
   ```
   A quick way to generate test frames from any video on macOS/Linux:
   ```bash
   mkdir frames
   ffmpeg -i input.mp4 -vf fps=10 frames/frame_%04d.jpg
   ```

2. Run the app, then click **Load Folder** in the Frame Viewer panel and select the folder.

3. Click **Play** to cycle through frames (~10 fps). The counter in the bottom-right of the panel shows the current position.

### Supported formats

**Image sequences (Load Folder)**

| Format | Extensions |
|--------|-----------|
| JPEG   | `.jpg`, `.jpeg` |
| PNG    | `.png` |
| BMP    | `.bmp` |

**Video files (Load Video)**

| Format | Extensions |
|--------|-----------|
| MP4    | `.mp4` |
| QuickTime | `.mov` |
| AVI    | `.avi` |
| Matroska | `.mkv` |
| M4V    | `.m4v` |

Video decoding uses Qt6::Multimedia (AVFoundation on macOS). Playback controls show `mm:ss / mm:ss` position.

### Path to live hardware video

The viewer exposes a `setFrame(const QImage &)` slot. To connect a live camera or stream, route decoded frames into that slot — for example using `Qt6::Multimedia` (`QCamera` + `QVideoSink`), OpenCV `VideoCapture`, or any other source that can produce a `QImage` per frame.
