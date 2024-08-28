#
# Source files
#
# Here is where you can add the name of your local common classes.
#

SET (MedSquareSrcs
  main.cxx
  MSQ2DRenderWidget.cxx
  MSQ3DRenderWidget.cxx
  MSQAxialRenderWidget.cxx
  MSQComponentWidget.cxx
  MSQCoronalRenderWidget.cxx  
  MSQExportSliceDialog.cxx
  MSQGeometryWidget.cxx
  MSQGeometryDialog.cxx
  MSQGeometryDifference.cxx
  MSQGeometryDifferenceItem.cxx
  MSQGeometryItem.cxx
  MSQImageIO.cxx
  MSQImageItem.cxx
  MSQImageManagerWidget.cxx
  MSQImportDICOMDialog.cxx
  MSQInspectorWidget.cxx
  MSQOpenRawDialog.cxx  
  MSQOrientationColors.cxx
  MSQOrientationWidget.cxx
  MSQOrthogonalViewer.cxx
  MSQProjectionMenu.cxx
  MSQRenderWidget.cxx
  MSQSagittalRenderWidget.cxx
  MSQSliceExporter.cxx
  MSQSliceNavigationWidget.cxx
  MSQWindowLevelWidget.cxx
  vtkmsq3DOrientationMarkerWidget.cxx
  vtkmsqDistancePolyDataFilter.cxx
  vtkmsqInteractorStyleImage.cxx
  vtkmsqInteractorStyleImage2D.cxx
  vtkmsqInteractorStyleImage3D.cxx
)

# Use the include path and library for Qt that is used by VTK.
INCLUDE_DIRECTORIES(
  ${QT_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

SET(MedSquareSrcs ${MedSquareSrcs} MedSquare.cxx)

# If you have .ui files put them in the next line
SET(UIS )
SET(MedSquareHeaders
  MedSquare.h
  MSQ2DRenderWidget.h
  MSQ3DRenderWidget.h
  MSQAxialRenderWidget.h
  MSQComponentWidget.h
  MSQCoronalRenderWidget.h
  MSQExportSliceDialog.h  
  MSQGeometryDialog.h
  MSQGeometryDifference.h
  MSQGeometryDifferenceItem.h
  MSQGeometryItem.h
  MSQGeometryWidget.h
  MSQImageIO.h
  MSQImageItem.h
  MSQImageManagerWidget.h
  MSQImportDICOMDialog.h
  MSQInspectorWidget.h
  MSQOpenRawDialog.h
  MSQOrientationWidget.h
  MSQOrthogonalViewer.h
  MSQProjectionMenu.h
  MSQRenderWidget.h
  MSQSagittalRenderWidget.h
  MSQSliceExporter.h
  MSQSliceNavigationWidget.h
  MSQViewer.h
  MSQWindowLevelWidget.h
)

SET(MedSquareResources MedSquare.qrc)

QT4_ADD_RESOURCES(RCCSrcs ${MedSquareResources})
#QT4_WRAP_UI(UIHeaders ${UIS})
#QT4_WRAP_CPP(MOCSrcs ${MedSquareHeaders} )
ADD_DEFINITIONS(-DQT_GUI_LIBS -DQT_CORE_LIB)
SET_SOURCE_FILES_PROPERTIES(${MedSquareSrcs} PROPERTIES
                            OBJECT_DEPENDS "${UIHeaders}")

# It's nice to have the ui in the windows project file...just double click on it
# and designer comes up on that ui file :)

IF (${CMAKE_BUILD_TOOL} MATCHES "msdev")
  SET (MedSquareSrcs ${MedSquareSrcs} ${UIS})
ENDIF (${CMAKE_BUILD_TOOL} MATCHES "msdev")
IF (${CMAKE_BUILD_TOOL} MATCHES "devenv")
  SET (MedSquareSrcs ${MedSquareSrcs} ${UIS})
ENDIF (${CMAKE_BUILD_TOOL} MATCHES "devenv")

#ADD_EXECUTABLE ( MedSquare WIN32 MACOSX_BUNDLE ${MedSquareSrcs} ${UISrcs} ${MOCSrcs} ${RCCSrcs})
ADD_EXECUTABLE ( MedSquare 
  ${MedSquareSrcs} 
  "${MedSquareHeaders}" 
  MedSquare.rc 
  ${UISrcs} ${MOCSrcs} ${RCCSrcs}
  ../IO/vtkmsqOBJWriter.cxx 
  ../IO/MSQPListParser.cxx 
  ../Imaging/MSQColormapFactory.cxx)
SET_TARGET_PROPERTIES(MedSquare PROPERTIES WIN32_EXECUTABLE $<CONFIG:Release>)

TARGET_LINK_LIBRARIES (MedSquare
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
)

INSTALL(TARGETS MedSquare
  RUNTIME DESTINATION bin
  BUNDLE DESTINATION bin
)
