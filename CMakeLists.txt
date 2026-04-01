cmake_minimum_required(VERSION 3.16)
project(OncoFlowMonitor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Multimedia Test)

enable_testing()

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
    src/services/HistoryService.cpp
    src/widgets/HistoryPanel.cpp
    src/widgets/RunSummaryDialog.cpp
    src/widgets/FrameViewer.cpp
    src/services/FrameSource.cpp
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
    src/services/HistoryService.h
    src/widgets/HistoryPanel.h
    src/widgets/RunSummaryDialog.h
    src/widgets/FrameViewer.h
    src/services/FrameSource.h
)

add_executable(OncoFlowMonitor ${SOURCES} ${HEADERS})

target_link_libraries(OncoFlowMonitor PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Multimedia
)

target_include_directories(OncoFlowMonitor PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Copy assets to build directory
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

# -----------------------------------------------------------------------
# Test targets
# -----------------------------------------------------------------------

# Shared library of all testable service/widget code (no main.cpp).
add_library(OncoFlowTestLib STATIC
    src/services/FrameSource.cpp
    src/services/FrameSource.h
    src/widgets/FrameViewer.cpp
    src/widgets/FrameViewer.h
    src/services/StudyService.cpp
    src/services/StudyService.h
    src/services/ValidationService.cpp
    src/services/ValidationService.h
    src/services/PipelineController.cpp
    src/services/PipelineController.h
    src/services/PipelineWorker.cpp
    src/services/PipelineWorker.h
    src/services/HistoryService.cpp
    src/services/HistoryService.h
    src/widgets/ConfigPanel.cpp
    src/widgets/ConfigPanel.h
    src/widgets/StatusPanel.cpp
    src/widgets/StatusPanel.h
    src/widgets/LogPanel.cpp
    src/widgets/LogPanel.h
    src/widgets/HistoryPanel.cpp
    src/widgets/HistoryPanel.h
    src/widgets/StudyListWidget.cpp
    src/widgets/StudyListWidget.h
    src/widgets/RunSummaryDialog.cpp
    src/widgets/RunSummaryDialog.h
)
target_include_directories(OncoFlowTestLib PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(OncoFlowTestLib PUBLIC
    Qt6::Widgets
    Qt6::Core
    Qt6::Multimedia
)

# Helper macro to reduce boilerplate for each test target
macro(add_oncoflow_test TARGET SOURCE)
    qt_add_executable(${TARGET} ${SOURCE})
    target_include_directories(${TARGET} PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/tests
    )
    target_link_libraries(${TARGET} PRIVATE
        OncoFlowTestLib
        Qt6::Test
        Qt6::Widgets
        Qt6::Core
        Qt6::Multimedia
    )
    add_test(NAME ${TARGET} COMMAND ${TARGET})
    set_tests_properties(${TARGET} PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
    )
endmacro()

# --- tst_FrameSource ---------------------------------------------------
qt_add_executable(tst_FrameSource tests/tst_FrameSource.cpp)
target_include_directories(tst_FrameSource PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/tests
)
target_link_libraries(tst_FrameSource PRIVATE
    OncoFlowTestLib
    Qt6::Test
    Qt6::Core
)
add_test(NAME FrameSource COMMAND tst_FrameSource)
set_tests_properties(FrameSource PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# --- tst_FrameViewer ---------------------------------------------------
qt_add_executable(tst_FrameViewer tests/tst_FrameViewer.cpp)
target_include_directories(tst_FrameViewer PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/tests
)
target_link_libraries(tst_FrameViewer PRIVATE
    OncoFlowTestLib
    Qt6::Test
    Qt6::Widgets
    Qt6::Core
    Qt6::Multimedia
)
add_test(NAME FrameViewer COMMAND tst_FrameViewer)
set_tests_properties(FrameViewer PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# --- Phase 1: Pure logic services -------------------------------------
add_oncoflow_test(tst_ValidationService tests/tst_ValidationService.cpp)
add_oncoflow_test(tst_StudyService      tests/tst_StudyService.cpp)
add_oncoflow_test(tst_JsonUtils         tests/tst_JsonUtils.cpp)
add_oncoflow_test(tst_HistoryService    tests/tst_HistoryService.cpp)

# --- Phase 2: Threaded services ---------------------------------------
add_oncoflow_test(tst_PipelineWorker     tests/tst_PipelineWorker.cpp)
add_oncoflow_test(tst_PipelineController tests/tst_PipelineController.cpp)

# --- Phase 3: Widget tests --------------------------------------------
add_oncoflow_test(tst_ConfigPanel      tests/tst_ConfigPanel.cpp)
add_oncoflow_test(tst_StatusPanel      tests/tst_StatusPanel.cpp)
add_oncoflow_test(tst_LogPanel         tests/tst_LogPanel.cpp)
add_oncoflow_test(tst_StudyListWidget  tests/tst_StudyListWidget.cpp)
add_oncoflow_test(tst_HistoryPanel     tests/tst_HistoryPanel.cpp)
add_oncoflow_test(tst_RunSummaryDialog tests/tst_RunSummaryDialog.cpp)
