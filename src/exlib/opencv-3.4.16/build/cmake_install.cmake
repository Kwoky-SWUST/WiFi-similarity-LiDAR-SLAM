# Install script for directory: /home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16

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

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlicensesx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/licenses/opencv3" TYPE FILE RENAME "ippicv-readme.htm" FILES "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/build/3rdparty/ippicv/ippicv_lnx/icv/readme.htm")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlicensesx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/licenses/opencv3" TYPE FILE RENAME "ippicv-EULA.txt" FILES "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/build/3rdparty/ippicv/ippicv_lnx/EULA.txt")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlicensesx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/licenses/opencv3" TYPE FILE RENAME "ippicv-third-party-programs.txt" FILES "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/build/3rdparty/ippicv/ippicv_lnx/third-party-programs.txt")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlicensesx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/licenses/opencv3" TYPE FILE RENAME "ippiw-support.txt" FILES "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/build/3rdparty/ippicv/ippicv_lnx/icv/../iw/../support.txt")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlicensesx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/licenses/opencv3" TYPE FILE RENAME "ippiw-third-party-programs.txt" FILES "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/build/3rdparty/ippicv/ippicv_lnx/icv/../iw/../third-party-programs.txt")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlicensesx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/licenses/opencv3" TYPE FILE RENAME "ippiw-EULA.txt" FILES "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/build/3rdparty/ippicv/ippicv_lnx/icv/../iw/../EULA.txt")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlicensesx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/licenses/opencv3" TYPE FILE RENAME "opencl-headers-LICENSE.txt" FILES "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/3rdparty/include/opencl/LICENSE.txt")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2" TYPE FILE FILES "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/build/cvconfig.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2" TYPE FILE FILES "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/build/opencv2/opencv_modules.hpp")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/build/unix-install/opencv.pc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/OpenCV/OpenCVModules.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/OpenCV/OpenCVModules.cmake"
         "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/build/CMakeFiles/Export/share/OpenCV/OpenCVModules.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/OpenCV/OpenCVModules-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/OpenCV/OpenCVModules.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/OpenCV" TYPE FILE FILES "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/build/CMakeFiles/Export/share/OpenCV/OpenCVModules.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/OpenCV" TYPE FILE FILES "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/build/CMakeFiles/Export/share/OpenCV/OpenCVModules-release.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/OpenCV" TYPE FILE FILES
    "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/build/unix-install/OpenCVConfig-version.cmake"
    "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/build/unix-install/OpenCVConfig.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xscriptsx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/build/CMakeFiles/install/setup_vars_opencv3.sh")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/OpenCV" TYPE FILE FILES
    "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/platforms/scripts/valgrind.supp"
    "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/platforms/scripts/valgrind_3rdparty.supp"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/build/3rdparty/libjasper/cmake_install.cmake")
  include("/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/build/3rdparty/ippiw/cmake_install.cmake")
  include("/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/build/3rdparty/protobuf/cmake_install.cmake")
  include("/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/build/3rdparty/quirc/cmake_install.cmake")
  include("/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/build/3rdparty/ittnotify/cmake_install.cmake")
  include("/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/build/include/cmake_install.cmake")
  include("/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/build/modules/cmake_install.cmake")
  include("/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/build/doc/cmake_install.cmake")
  include("/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/build/data/cmake_install.cmake")
  include("/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/build/apps/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/gglin-test/wifi_SLAM/src/exlib/opencv-3.4.16/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
