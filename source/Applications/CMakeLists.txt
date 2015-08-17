#
# Source files
#
# Here is where you can add the name of your local common classes.
#

SET (MSQDicomExplorerSrcs
  MSQDicomImageViewerButton.cxx
  MSQAspectRatioPixmapLabel.cxx
  MSQTagSortItem.cxx
  MSQDicomTagModel.cxx
  MSQDicomTagSortingModel.cxx
  MSQSortOrderDelegate.cxx
  MSQSearchLineEdit.cxx
  MSQDicomSearchLineEdit.cxx
  MSQDicomHeaderViewer.cxx
  MSQDicomImageViewer.cxx
  MSQDicomImageSorter.cxx
  MSQDicomQualityControl.cxx
  MSQDicomExplorer.cxx
  main.cxx
)

IF (USE_BIOIMAGESUITE)
  MESSAGE(STATUS "Adding BioimageSuite libraries")
  ADD_DEFINITIONS(-DUSE_BIOIMAGESUITE)
ENDIF (USE_BIOIMAGESUITE)

#MESSAGE(STATUS "BIS_LIBRARY_DIRS: " ${BIS_LIBRARY_DIRS} )
#MESSAGE(STATUS "BIS_INCLUDE_DIRS: " ${BIS_INCLUDE_DIRS} )

# Use the include path and library for Qt that is used by VTK.
INCLUDE_DIRECTORIES(
  ${QT_INCLUDE_DIR}
  ${BIS_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

SET(DicomExplorerSrcs ${DicomExplorerSrcs} DicomExplorer.cxx)

# If you have .ui files put them in the next line
SET(UIS )
SET(MSQDicomExplorerHeaders
  MSQDicomImageViewerButton.h
  MSQAspectRatioPixmapLabel.h
  MSQTagSortItem.h
  MSQDicomTagModel.h
  MSQDicomTagSortingModel.h
  MSQSortOrderDelegate.h
  MSQSearchLineEdit.h
  MSQDicomSearchLineEdit.h
  MSQDicomHeaderViewer.h
  MSQDicomImageViewer.h
  MSQDicomImageSorter.h
  MSQDicomQualityControl.h
  MSQDicomExplorer.h
)

SET(MSQDicomExplorerResources MSQDicomExplorer.qrc)

QT4_ADD_RESOURCES(RCCSrcs ${MSQDicomExplorerResources})
QT4_WRAP_UI(UIHeaders ${UIS})
QT4_WRAP_CPP(MOCSrcs ${MSQDicomExplorerHeaders} )
ADD_DEFINITIONS(-DQT_GUI_LIBS -DQT_CORE_LIB)
SET_SOURCE_FILES_PROPERTIES(${MSQDicomExplorerSrcs} PROPERTIES
                            OBJECT_DEPENDS "${UIHeaders}")

# It's nice to have the ui in the windows project file...just double click on it
# and designer comes up on that ui file :)

IF (${CMAKE_BUILD_TOOL} MATCHES "msdev")
  SET (MSQDicomExplorerSrcs ${MSQDicomExplorerSrcs} ${UIS})
ENDIF (${CMAKE_BUILD_TOOL} MATCHES "msdev")
IF (${CMAKE_BUILD_TOOL} MATCHES "devenv")
  SET (MSQDicomExplorerSrcs ${MSQDicomExplorerSrcs} ${UIS})
ENDIF (${CMAKE_BUILD_TOOL} MATCHES "devenv")

ADD_EXECUTABLE ( DicomExplorer WIN32 MACOSX_BUNDLE ${MSQDicomExplorerSrcs} ${UISrcs} ${MOCSrcs} ${RCCSrcs})

TARGET_LINK_LIBRARIES (DicomExplorer
  QVTK
  ${QT_LIBRARIES}
  gdcmCommon
  gdcmMSFF
  vtkgdcm
  vtkmsqImaging
  vtkmsqGraphics
  vtkmsqIO
  vtkCommon
  vtkRendering
  vtkGraphics
  vtkImaging
  vtkIO
  vtkWidgets
  ${BIS_LIBRARIES}
)

INSTALL(TARGETS DicomExplorer
  RUNTIME DESTINATION bin
  BUNDLE DESTINATION bin
)
