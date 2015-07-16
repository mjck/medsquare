#
# Source files
#
# Here is where you can add the name of your local common classes.
#

# Use the include path and library for Qt that is used by VTK.
INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

SET(TESTS
    vtkmsqPhilipsReaderTest
    vtkmsqRawReaderTest
    vtkmsqAnalyzeWriterTest
    vtkmsqAnalyzeReaderTest
  )

IF (MEDSQUARE_BUILD_TESTS)
  FIND_PACKAGE(GTest REQUIRED)

  FOREACH(TEST IN LISTS TESTS)
    ADD_EXECUTABLE ( ${TEST} ${TEST}.cxx )
    TARGET_LINK_LIBRARIES ( ${TEST}
      ${GTEST_BOTH_LIBRARIES}
      QVTK
      ${QT_LIBRARIES}
      gdcmCommon
      gdcmMSFF
      vtkgdcm
      vtkmsqImaging
      vtkmsqIO
      vtkRendering
      vtkGraphics
      vtkIO
      vtkCommon )
    GTEST_ADD_TESTS (${CMAKE_BINARY_DIR}/bin/${TEST} "" ${TEST}.cxx)
  ENDFOREACH(TEST IN LISTS TESTS)

  FILE(COPY Data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
ENDIF (MEDSQUARE_BUILD_TESTS)
