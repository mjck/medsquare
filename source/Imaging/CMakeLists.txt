#
# Source files
#
# Here is where you can add the name of your local I/O classes.
#

SET (IMAGING_SRCS
  MSQColormapFactory.cxx
  vtkmsqImageAverage.cxx
  vtkmsqLookupTable.cxx
)

# Use the include path and library for Qt that is used by VTK.
INCLUDE_DIRECTORIES(
  ${QT_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# If you have .ui files put them in the next line
SET(UIS )

SET(IMAGING_HEADERS
  MSQColormapFactory.h
  vtkmsqImageAverage.h
  vtkmsqLookupTable.h
)

#QT4_WRAP_CPP(MOCSrcs ${IMAGING_HEADERS} )
ADD_DEFINITIONS(-DQT_GUI_LIBS -DQT_CORE_LIB -DQT_XML_LIB)

# --------------------------------------------------------------------------
# You probably do not need to modify anything below this line

# Create the vtkmsqImaging C++ library.
ADD_LIBRARY (vtkmsqImaging ${IMAGING_SRCS} ../IO/MSQPListParser.cxx)
TARGET_LINK_LIBRARIES (vtkmsqImaging 
  ##vtkImaging 
  vtkzlib 
  vtkmsqIO
  ${VTK_LIBRARIES})

# Create the vtkmsqImaging Tcl library.
IF (VTK_WRAP_TCL AND MEDSQUARE_WRAP_TCL)
  VTK_WRAP_TCL3 (vtkmsqImagingTCL ImagingTCL_SRCS "${IMAGING_SRCS}" "")
  ADD_LIBRARY (vtkmsqImagingTCL ${ImagingTCL_SRCS})
  TARGET_LINK_LIBRARIES (vtkmsqImagingTCL vtkmsqImaging vtkIOTCL)
ENDIF (VTK_WRAP_TCL AND MEDSQUARE_WRAP_TCL)

# Create the vtkmsqImaging Python library.
IF (VTK_WRAP_PYTHON AND MEDSQUARE_WRAP_PYTHON)
  INCLUDE_DIRECTORIES("${PYTHON_INCLUDE_PATH}")
  VTK_WRAP_PYTHON3 (vtkmsqImagingPython ImagingPython_SRCS "${IMAGING_SRCS}")
  ADD_LIBRARY (vtkmsqImagingPythonD ${ImagingPython_SRCS})
  ADD_LIBRARY (vtkmsqImagingPython MODULE vtkmsqImagingPythonInit.cxx)
  TARGET_LINK_LIBRARIES(vtkmsqImagingPythonD vtkmsqImaging vtkImaging vtkImagingPythonD)
  TARGET_LINK_LIBRARIES(vtkmsqImagingPython vtkmsqImagingPythonD)
ENDIF (VTK_WRAP_PYTHON AND MEDSQUARE_WRAP_PYTHON)

# Create the vtkmsqImaging Java library.
IF (VTK_WRAP_JAVA AND MEDSQUARE_WRAP_JAVA)
  VTK_WRAP_JAVA3 (vtkmsqImagingJava ImagingJava_SRCS "${IMAGING_SRCS}")
  ADD_LIBRARY (vtkmsqImagingJava SHARED ${ImagingJava_SRCS})
  TARGET_LINK_LIBRARIES (vtkmsqImagingJava vtkmsqImaging vtkImagingJava)
ENDIF (VTK_WRAP_JAVA AND MEDSQUARE_WRAP_JAVA)

INSTALL(TARGETS vtkmsqImaging
  DESTINATION bin
)
