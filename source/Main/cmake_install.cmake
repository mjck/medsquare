# Install script for directory: /Users/marcel/medsquare/source/Main

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/Users/marcel/medsquare/build")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
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

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/Users/marcel/medsquare/source/bin/MedSquare")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/MedSquare" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/MedSquare")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/marcel/medsquare/deps/itk320/lib/InsightToolkit"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/MedSquare")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/marcel/medsquare/deps/gdcm28/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/MedSquare")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/marcel/medsquare/source/bin"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/MedSquare")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/MedSquare")
    endif()
  endif()
endif()

