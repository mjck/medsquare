#
# Configure output paths for libraries and executables.
#
SET(LIBRARY_OUTPUT_PATH ${MEDSQUARE_BINARY_DIR}/bin CACHE PATH
    "Single output directory for building all libraries.")
SET(EXECUTABLE_OUTPUT_PATH ${MEDSQUARE_BINARY_DIR}/bin CACHE PATH
    "Single output directory for building all executables.")
MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)

#
# Try to find VTK and include its settings (otherwise complain)
#
FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

#
# Try to find ITK and include its settings (otherwise complain as well)
#
FIND_PACKAGE(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})

#
# Try to find GDCM and include its settings
#
FIND_PACKAGE(GDCM REQUIRED)
INCLUDE(${GDCM_USE_FILE})

#
# Try to find GTest and include its settings
#
IF (MEDSQUARE_BUILD_TESTS)
   FIND_PACKAGE(GTest)
   INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIRS})
ENDIF (MEDSQUARE_BUILD_TESTS)

#
# Build shared libs ?
#
# Defaults to the same VTK setting.
#

# Standard CMake option for building libraries shared or static by default.
OPTION(BUILD_SHARED_LIBS
       "Build with shared libraries."
       ${VTK_BUILD_SHARED_LIBS})
# Copy the CMake option to a setting with MEDSQUARE_ prefix for use in
# our project.  This name is used in vtkmyConfigure.h.in.
SET(MEDSQUARE_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})

# If this is a build tree, provide an option for putting
# this project's executables and libraries in with VTK's.
IF (EXISTS ${VTK_DIR}/bin)
  OPTION(USE_VTK_OUTPUT_PATHS
         "Use VTK's output directory for this project's executables and libraries."
         OFF)
  MARK_AS_ADVANCED (USE_VTK_OUTPUT_PATHS)
  IF (USE_VTK_OUTPUT_PATHS)
    SET (LIBRARY_OUTPUT_PATH ${VTK_DIR}/bin)
    SET (EXECUTABLE_OUTPUT_PATH ${VTK_DIR}/bin)
  ENDIF (USE_VTK_OUTPUT_PATHS)
ENDIF (EXISTS ${VTK_DIR}/bin)


#
# Qt settings
#
IF (APPLE)
  FIND_PACKAGE(Qt5 COMPONENTS Core Gui OpenGL Xml REQUIRED)
ENDIF (APPLE)
IF (WIN32)
  FIND_PACKAGE(Qt4 REQUIRED QtCore QtGui QtOpenGL QtXml REQUIRED)
ENDIF (WIN32)
#INCLUDE(${QT_USE_FILE})

#
# Wrap Tcl, Java, Python
#
# Rational: even if your VTK was wrapped, it does not mean that you want to
# wrap your own local classes.
# Default value is OFF as the VTK cache might have set them to ON but
# the wrappers might not be present (or yet not found).
#

#
# Tcl
#

IF (VTK_WRAP_TCL)

  OPTION(MEDSQUARE_WRAP_TCL
         "Wrap classes into the TCL interpreted language."
         ON)

  IF(MEDSQUARE_WRAP_TCL)
    INCLUDE(${VTK_CMAKE_DIR}/vtkWrapTcl.cmake)
  ENDIF(MEDSQUARE_WRAP_TCL)

ELSE (VTK_WRAP_TCL)

  IF (MEDSQUARE_WRAP_TCL)
    MESSAGE("Warning. MEDSQUARE_WRAP_TCL is ON but the VTK version you have "
            "chosen has not support for Tcl (VTK_WRAP_TCL is OFF).  "
            "Please set MEDSQUARE_WRAP_TCL to OFF.")
    SET (MEDSQUARE_WRAP_TCL OFF)
  ENDIF (MEDSQUARE_WRAP_TCL)

ENDIF (VTK_WRAP_TCL)

#
# Python
#

IF (VTK_WRAP_PYTHON)

  OPTION(MEDSQUARE_WRAP_PYTHON
         "Wrap classes into the Python interpreted language."
         ON)

  IF (MEDSQUARE_WRAP_PYTHON)
    SET(VTK_WRAP_PYTHON_FIND_LIBS ON)
    INCLUDE(${VTK_CMAKE_DIR}/vtkWrapPython.cmake)
    IF (WIN32)
      IF (NOT BUILD_SHARED_LIBS)
        MESSAGE(FATAL_ERROR "Python support requires BUILD_SHARED_LIBS to be ON.")
        SET (MEDSQUARE_CAN_BUILD 0)
      ENDIF (NOT BUILD_SHARED_LIBS)
    ENDIF (WIN32)
  ENDIF (MEDSQUARE_WRAP_PYTHON)

ELSE (VTK_WRAP_PYTHON)

  IF (MEDSQUARE_WRAP_PYTHON)
    MESSAGE("Warning. MEDSQUARE_WRAP_PYTHON is ON but the VTK version you have "
            "chosen has not support for Python (VTK_WRAP_PYTHON is OFF).  "
            "Please set MEDSQUARE_WRAP_PYTHON to OFF.")
    SET (MEDSQUARE_WRAP_PYTHON OFF)
  ENDIF (MEDSQUARE_WRAP_PYTHON)

ENDIF (VTK_WRAP_PYTHON)

#
# Java
#
IF (VTK_WRAP_JAVA)

  OPTION(MEDSQUARE_WRAP_JAVA
         "Wrap classes into the Java interpreted language."
         ON)

  IF (MEDSQUARE_WRAP_JAVA)
    SET(VTK_WRAP_JAVA3_INIT_DIR "${MEDSQUARE_SOURCE_DIR}/Wrapping")
    INCLUDE(${VTK_CMAKE_DIR}/vtkWrapJava.cmake)
    IF (WIN32)
      IF (NOT BUILD_SHARED_LIBS)
        MESSAGE(FATAL_ERROR "Java support requires BUILD_SHARED_LIBS to be ON.")
        SET (MEDSQUARE_CAN_BUILD 0)
      ENDIF (NOT BUILD_SHARED_LIBS)
    ENDIF (WIN32)

    # Tell the java wrappers where to go.
    SET(VTK_JAVA_HOME ${MEDSQUARE_BINARY_DIR}/java/vtkmy)
    MAKE_DIRECTORY(${VTK_JAVA_HOME})
  ENDIF (MEDSQUARE_WRAP_JAVA)

ELSE (VTK_WRAP_JAVA)

  IF (MEDSQUARE_WRAP_JAVA)
    MESSAGE("Warning. MEDSQUARE_WRAP_JAVA is ON but the VTK version you have "
            "chosen has not support for Java (VTK_WRAP_JAVA is OFF).  "
            "Please set MEDSQUARE_WRAP_JAVA to OFF.")
    SET (MEDSQUARE_WRAP_JAVA OFF)
  ENDIF (MEDSQUARE_WRAP_JAVA)

ENDIF (VTK_WRAP_JAVA)

# Setup our local hints file in case wrappers need them.
SET(VTK_WRAP_HINTS ${MEDSQUARE_SOURCE_DIR}/Wrapping/hints)

# Tests configurations
OPTION(MEDSQUARE_BUILD_TESTS
	"Build tests"
	OFF)

OPTION(USE_BIOIMAGESUITE
  "Use BioimageSuite Module"
  OFF)

#
# Try to find BIS and include its settings
#
IF (USE_BIOIMAGESUITE)

  FIND_PACKAGE(BIS)
  
  INCLUDE(${BIS_USE_FILE})

ENDIF (USE_BIOIMAGESUITE)

ENABLE_TESTING()

INCLUDE(CPack)
