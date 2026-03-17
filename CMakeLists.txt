cmake_minimum_required(VERSION 3.16)
project(OncoFlowMonitor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core)

set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/services/StudyService.cpp
    src/widgets/StudyListWidget.cpp
    src/widgets/ConfigPanel.cpp
    src/services/ValidationService.cpp
    src/widgets/StatusPanel.cpp
    src/widgets/LogPanel.cpp
    src/services/PipelineController.cpp
    src/services/PipelineWorker.cpp
)

set(HEADERS
    src/MainWindow.h
    src/models/Study.h
    src/models/PipelineConfig.h
    src/models/PipelineRun.h
    src/models/LogEntry.h
    src/services/StudyService.h
    src/widgets/StudyListWidget.h
    src/widgets/ConfigPanel.h
    src/services/ValidationService.h
    src/widgets/StatusPanel.h
    src/widgets/LogPanel.h
    src/services/PipelineController.h
    src/services/PipelineWorker.h
)

add_executable(OncoFlowMonitor ${SOURCES} ${HEADERS})

target_link_libraries(OncoFlowMonitor PRIVATE
    Qt6::Widgets
    Qt6::Core
)

target_include_directories(OncoFlowMonitor PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Copy assets to build directory
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
