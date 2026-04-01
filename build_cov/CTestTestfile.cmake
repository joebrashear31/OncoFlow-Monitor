# CMake generated Testfile for 
# Source directory: /Users/joebrashear/Desktop/OncoFlow-Monitor
# Build directory: /Users/joebrashear/Desktop/OncoFlow-Monitor/build_cov
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(FrameSource "/Users/joebrashear/Desktop/OncoFlow-Monitor/build_cov/tst_FrameSource")
set_tests_properties(FrameSource PROPERTIES  ENVIRONMENT "QT_QPA_PLATFORM=offscreen" _BACKTRACE_TRIPLES "/Users/joebrashear/Desktop/OncoFlow-Monitor/CMakeLists.txt;94;add_test;/Users/joebrashear/Desktop/OncoFlow-Monitor/CMakeLists.txt;0;")
add_test(FrameViewer "/Users/joebrashear/Desktop/OncoFlow-Monitor/build_cov/tst_FrameViewer")
set_tests_properties(FrameViewer PROPERTIES  ENVIRONMENT "QT_QPA_PLATFORM=offscreen" _BACKTRACE_TRIPLES "/Users/joebrashear/Desktop/OncoFlow-Monitor/CMakeLists.txt;109;add_test;/Users/joebrashear/Desktop/OncoFlow-Monitor/CMakeLists.txt;0;")
