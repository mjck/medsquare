#
# Source files
#
# Here is where you can add the name of your local graphics classes.
#

SET (GRAPHICS_SRCS
  vtkmsqFrameSource.cxx
  vtkmsqImageItem.cxx
  vtkmsqImagePlane.cxx
  vtkmsqAxialImagePlane.cxx
  vtkmsqSagittalImagePlane.cxx
  vtkmsqCoronalImagePlane.cxx
  vtkmsqRectangleActor2D.cxx
)

# --------------------------------------------------------------------------
# You probably do not need to modify anything below this line

# Create the vtkmsqGraphics C++ library.
ADD_LIBRARY (vtkmsqGraphics ${GRAPHICS_SRCS})
TARGET_LINK_LIBRARIES (vtkmsqGraphics vtkmsqImaging vtkmsqIO vtkzlib ${VTK_LIBRARIES})

# Create the vtkmsqGraphics Tcl library.
IF (VTK_WRAP_TCL AND MEDSQUARE_WRAP_TCL)
  VTK_WRAP_TCL3 (vtkmsqGraphicsTCL GraphicsTCL_SRCS "${GRAPHICS_SRCS}" "")
  ADD_LIBRARY (vtkmsqGraphicsTCL ${GraphicsTCL_SRCS})
  TARGET_LINK_LIBRARIES (vtkmsqGraphicsTCL vtkmsqGraphics vtkIOTCL)
ENDIF (VTK_WRAP_TCL AND MEDSQUARE_WRAP_TCL)

# Create the vtkmsqGraphics Python library.
IF (VTK_WRAP_PYTHON AND MEDSQUARE_WRAP_PYTHON)
  INCLUDE_DIRECTORIES("${PYTHON_INCLUDE_PATH}")	
  VTK_WRAP_PYTHON3 (vtkmsqGraphicsPython GraphicsPython_SRCS "${GRAPHICS_SRCS}")
  ADD_LIBRARY (vtkmsqGraphicsPythonD ${GraphicsPython_SRCS})
  ADD_LIBRARY (vtkmsqGraphicsPython MODULE vtkmsqGraphicsPythonInit.cxx)
  TARGET_LINK_LIBRARIES(vtkmsqGraphicsPythonD vtkmsqGraphics
  vtkGraphics vtkRendering vtkRenderingPythonD vtkGraphicsPythonD)
  TARGET_LINK_LIBRARIES(vtkmsqGraphicsPython vtkmsqGraphicsPythonD)
ENDIF (VTK_WRAP_PYTHON AND MEDSQUARE_WRAP_PYTHON)

# Create the vtkmsqGraphics Java library.
IF (VTK_WRAP_JAVA AND MEDSQUARE_WRAP_JAVA)
  VTK_WRAP_JAVA3 (vtkmsqGraphicsJava GraphicsJava_SRCS "${GRAPHICS_SRCS}")
  ADD_LIBRARY (vtkmsqGraphicsJava SHARED ${GraphicsJava_SRCS})
  TARGET_LINK_LIBRARIES (vtkmsqGraphicsJava vtkmsqGraphics vtkGraphicsJava)
ENDIF (VTK_WRAP_JAVA AND MEDSQUARE_WRAP_JAVA)

INSTALL(TARGETS vtkmsqGraphics
  DESTINATION bin
)
