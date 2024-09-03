#Script file to set paths for medsquare base distribution
MSQROOT=${HOME}/medsquare/deps

# if [ -n $LD_LIBRARY_PATH ]; then
#     echo ""
# else
#     LD_LIBRARY_PATH=/usr/local/lib
# fi

# ------------------- Name libraries ---------------------
VTK_DIR=${MSQROOT}/vtk71/lib;   export VTK_DIR
ITK_DIR=${MSQROOT}/itk320/lib/InsightToolkit; export ITK_DIR
GDCM_DIR=${MSQROOT}/gdcm28/lib/gdcm-2.8; export GDCM_DIR
QT_DIR=${MSQROOT}/qt515/lib; export QT_DIR

PATH=${MSQROOT}/cmake372/bin:${PATH}
PATH=${MSQROOT}/vtk71/bin:${PATH}
PATH=${MSQROOT}/gdcm28/bin:${PATH}
PATH=${PATH}:${QT_DIR}:${MSQROOT}/qt515/bin
export PATH

LD_LIBRARY_PATH=${VTK_DIR}:${LD_LIBRARY_PATH}
LD_LIBRARY_PATH=${ITK_DIR}:${LD_LIBRARY_PATH}
LD_LIBRARY_PATH=${GDCM_DIR}:${LD_LIBRARY_PATH}
LD_LIBRARY_PATH=${QT_DIR}:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH

DYLD_LIBRARY_PATH=${MSQROOT}/vtk71/lib:${DYLD_LIBRARY_PATH}
DYLD_LIBRARY_PATH=${MSQROOT}/itk320/lib/InsightToolkit:${DYLD_LIBRARY_PATH}
DYLD_LIBRARY_PATH=${MSQROOT}/gdcm28/lib:${DYLD_LIBRARY_PATH}
DYLD_LIBRARY_PATH=${QT_DIR}:${DYLD_LIBRARY_PATH}
export DYLD_LIBRARY_PATH

# ----------------------------------------------------------
echo "MedSquare is now in your path"
echo "Key Components"

which cmake
#which vtk
which gdcminfo
which qmake

echo "VTK in ${VTK_DIR}"
echo "ITK in ${ITK_DIR}"
echo "GDCM in ${GDCM_DIR}"
echo "QT in ${QT_DIR}"