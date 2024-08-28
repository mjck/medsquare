CMAKE_MINIMUM_REQUIRED(VERSION 3.2.1 FATAL_ERROR)

#
# Set the project name.
#

PROJECT (MEDSQUARE)

SET(CMAKE_CXX_STANDARD 11)
# Process moc automatically
set(CMAKE_AUTOMOC ON)
# Process .ui files automatically
set(CMAKE_AUTOUIC ON)
# Process .qrc files automatically
set(CMAKE_AUTORCC ON)

#
# Compiler Specific Things
# -------------------------------------------------------------------------------------
IF(WIN32)
  ADD_DEFINITIONS( -DWIN32_LEAN_AND_MEAN -D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)
  SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:msvcrt140.lib ")
ENDIF(WIN32)

IF (UNIX)
IF( CMAKE_COMPILER_IS_GNUCXX )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall" )
  set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC" )
ENDIF( CMAKE_COMPILER_IS_GNUCXX )
ENDIF (UNIX)
# ------------------------------------------------------------------------------------


#
# Load CMake commands that you probably should not modify.
#

INCLUDE (${MEDSQUARE_SOURCE_DIR}/CMakeOptions.cmake)

#IF (MEDSQUARE_WRAP_PYTHON)
#   INCLUDE(${VTK_CMAKE_DIR}/vtkWrapPython.cmake)
#ENDIF (MEDSQUARE_WRAP_PYTHON)

#
# You can put your include path(s) here
#

INCLUDE_DIRECTORIES (${MEDSQUARE_SOURCE_DIR}/Imaging)
INCLUDE_DIRECTORIES (${MEDSQUARE_SOURCE_DIR}/Graphics)
INCLUDE_DIRECTORIES (${MEDSQUARE_SOURCE_DIR}/IO)
INCLUDE_DIRECTORIES (${MEDSQUARE_SOURCE_DIR}/Main)
INCLUDE_DIRECTORIES (${MEDSQUARE_SOURCE_DIR}/Applications)
INCLUDE_DIRECTORIES (${MEDSQUARE_SOURCE_DIR}/Testing)
INCLUDE_DIRECTORIES(${MEDSQUARE_BINARY_DIR})

#
# Common configuration settings
#
# Do not worry about this one.
#

CONFIGURE_FILE(
  ${MEDSQUARE_SOURCE_DIR}/vtkmsqConfigure.h.in
  ${MEDSQUARE_BINARY_DIR}/vtkmsqConfigure.h
)

#
# Here is where you can list the sub-directories holding your local
# classes. Sorting classes by 'package' type like VTK does (Common,
# Rendering, Filtering, Imaging, IO, etc.) is a good thing and prevents
# numerous dependencies problems.
#

ADD_SUBDIRECTORY(Imaging)
ADD_SUBDIRECTORY(Graphics)
ADD_SUBDIRECTORY(IO)
ADD_SUBDIRECTORY(Main)
ADD_SUBDIRECTORY(Applications)
ADD_SUBDIRECTORY(Testing)

INSTALL(FILES MedSquare.sh
  DESTINATION ${CMAKE_INSTALL_PREFIX})
