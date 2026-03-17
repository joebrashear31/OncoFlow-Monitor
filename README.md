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
