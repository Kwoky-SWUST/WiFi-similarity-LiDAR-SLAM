# CMake generated Testfile for 
# Source directory: /home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/modules/videostab
# Build directory: /home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/build/modules/videostab
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(opencv_test_videostab "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/build/bin/opencv_test_videostab" "--gtest_output=xml:opencv_test_videostab.xml")
set_tests_properties(opencv_test_videostab PROPERTIES  LABELS "Main;opencv_videostab;Accuracy" WORKING_DIRECTORY "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/build/test-reports/accuracy")
