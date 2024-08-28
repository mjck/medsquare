@REM Script file to set paths for medsquare base distribution
@set MSQROOT=%USERPROFILE%\medsquare\deps

@REM ------------------- Name libraries ---------------------
@set VTK_DIR=%MSQROOT%\vtk510\lib\vtk-5.10
@set ITK_DIR=%MSQROOT%\itk320\lib\InsightToolkit
@set GDCM_DIR=%MSQROOT%\gdcm28\lib\gdcm-2.8
@set QT4_DIR=%MSQROOT%\qt4\lib

@set PATH=%MSQROOT%\cmake323\bin;%PATH%
@set PATH=%PATH%;%MSQROOT%\vtk510\bin
@set PATH=%PATH%;%MSQROOT%\itk320\bin
@set PATH=%PATH%;%MSQROOT%\gdcm28\bin
@set PATH=%PATH%;%QT4_DIR%;%MSQROOT%\qt4\bin

@set LIB=%LIB%;%VTK_DIR%
@set LIB=%LIB%;%ITK_DIR%
@set LIB=%LIB%;%GDCM_DIR%
@set LIB=%LIB%;%QT4_DIR%

@REM ----------------------------------------------------------
@echo "Medsquare is now in your path"
@echo "Key Components"

@where cmake
@where vtk
@where gdcminfo
@where qmake

@echo VTK in %VTK_DIR%
@echo ITK in %ITK_DIR%
@echo GDCM in %GDCM_DIR%
@echo QT in %QT4_DIR%
