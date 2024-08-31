#
# Source files
#
# Here is where you can add the name of your local I/O classes.
#

SET (IO_SRCS
  MSQXMLParser.cxx
  MSQPListParser.cxx
  MSQPListSerializer.cxx
  vtkmsqMedicalImageProperties.cxx
  vtkmsqPhilipsPAR.cxx
  vtkmsqPhilipsRECReader.cxx
  vtkmsqAnalyzeReader.cxx
  vtkmsqBruker2DSEQReader.cxx
  vtkmsqAnalyzeWriter.cxx
  vtkmsqNiftiReader.cxx
  vtkmsqRawHeader.cxx
  vtkmsqRawReader.cxx
  vtkmsqGDCMImageReader.cxx
  vtkmsqGDCMMoisacImageReader.cxx
  vtkmsqOBJWriter.cxx
  vtkmsqImageInterleaving.cxx
)

# Use the include path and library for Qt that is used by VTK.
INCLUDE_DIRECTORIES(
  ${Qt5Core_INCLUDE_DIR}
  ${Qt5Widgets_INCLUDE_DIR}
  ${Qt5Xml_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)
# If you have .ui files put them in the next line
SET(UIS )

SET(IO_HEADERS
  MSQPListParser.h
)

#QT4_WRAP_CPP(MOCSrcs ${IO_HEADERS} )
ADD_DEFINITIONS(-DQT_GUI_LIBS -DQT_CORE_LIB -DQT_XML_LIB)

# --------------------------------------------------------------------------
# You probably do not need to modify anything below this line

# Create the vtkmsqIO C++ library.
ADD_LIBRARY (vtkmsqIO ${IO_SRCS} )
TARGET_LINK_LIBRARIES (vtkmsqIO 
  gdcmCommon
  gdcmMSFF
  vtkgdcm
  ##vtkIO 
  vtkzlib
  ITKniftiio
  Qt5::Core
  Qt5::Xml
  ${VTK_LIBRARIES})

# Create the vtkmsqIO Tcl library.
IF (VTK_WRAP_TCL AND MEDSQUARE_WRAP_TCL)
  VTK_WRAP_TCL3 (vtkmsqIOTCL IOTCL_SRCS "${IO_SRCS}" "")
  ADD_LIBRARY (vtkmsqIOTCL ${IOTCL_SRCS})
  TARGET_LINK_LIBRARIES (vtkmsqIOTCL vtkmsqIO vtkIOTCL)
ENDIF (VTK_WRAP_TCL AND MEDSQUARE_WRAP_TCL)

# Create the vtkmsqIO Python library.
IF (VTK_WRAP_PYTHON AND MEDSQUARE_WRAP_PYTHON)
  INCLUDE_DIRECTORIES("${PYTHON_INCLUDE_PATH}")	
  VTK_WRAP_PYTHON3 (vtkmsqIOPython IOPython_SRCS "${IO_SRCS}")
  ADD_LIBRARY (vtkmsqIOPythonD ${IOPython_SRCS})
  ADD_LIBRARY (vtkmsqIOPython MODULE vtkmsqIOPythonInit.cxx)
  TARGET_LINK_LIBRARIES(vtkmsqIOPythonD vtkmsqIO vtkIO vtkIOPythonD)
  TARGET_LINK_LIBRARIES(vtkmsqIOPython vtkmsqIOPythonD)
ENDIF (VTK_WRAP_PYTHON AND MEDSQUARE_WRAP_PYTHON)

# Create the vtkmsqIO Java library.
IF (VTK_WRAP_JAVA AND MEDSQUARE_WRAP_JAVA)
  VTK_WRAP_JAVA3 (vtkmsqIOJava IOJava_SRCS "${IO_SRCS}")
  ADD_LIBRARY (vtkmsqIOJava SHARED ${IOJava_SRCS})
  TARGET_LINK_LIBRARIES (vtkmsqIOJava vtkmsqIO vtkIOJava)
ENDIF (VTK_WRAP_JAVA AND MEDSQUARE_WRAP_JAVA)

INSTALL(TARGETS vtkmsqIO
  DESTINATION bin
)
