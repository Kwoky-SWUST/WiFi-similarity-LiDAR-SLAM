# Install script for directory: /home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/data

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibsx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/OpenCV/haarcascades" TYPE FILE FILES
    "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/data/haarcascades/haarcascade_eye.xml"
    "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/data/haarcascades/haarcascade_eye_tree_eyeglasses.xml"
    "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/data/haarcascades/haarcascade_frontalcatface.xml"
    "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/data/haarcascades/haarcascade_frontalcatface_extended.xml"
    "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/data/haarcascades/haarcascade_frontalface_alt.xml"
    "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/data/haarcascades/haarcascade_frontalface_alt2.xml"
    "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/data/haarcascades/haarcascade_frontalface_alt_tree.xml"
    "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/data/haarcascades/haarcascade_frontalface_default.xml"
    "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/data/haarcascades/haarcascade_fullbody.xml"
    "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/data/haarcascades/haarcascade_lefteye_2splits.xml"
    "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/data/haarcascades/haarcascade_licence_plate_rus_16stages.xml"
    "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/data/haarcascades/haarcascade_lowerbody.xml"
    "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/data/haarcascades/haarcascade_profileface.xml"
    "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/data/haarcascades/haarcascade_righteye_2splits.xml"
    "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/data/haarcascades/haarcascade_russian_plate_number.xml"
    "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/data/haarcascades/haarcascade_smile.xml"
    "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/data/haarcascades/haarcascade_upperbody.xml"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibsx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/OpenCV/lbpcascades" TYPE FILE FILES
    "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/data/lbpcascades/lbpcascade_frontalcatface.xml"
    "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/data/lbpcascades/lbpcascade_frontalface.xml"
    "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/data/lbpcascades/lbpcascade_frontalface_improved.xml"
    "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/data/lbpcascades/lbpcascade_profileface.xml"
    "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/data/lbpcascades/lbpcascade_silverware.xml"
    )
endif()

