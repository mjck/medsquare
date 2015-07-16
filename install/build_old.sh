#!/bin/bash

#=========================================================================
#
# Program:   MedSquare
# Module:    build.sh
#
# Copyright (c) Marcel P. Jackowski, Choukri Mekkaoui
# All rights reserved.
# See Copyright.txt or http://www.medsquare.org/copyright for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE.  See the above copyright notice for more information.
#
#=========================================================================
#
# \file build.sh
#
# \brief Consolidated installation script for MedSquare.
#
# Installation script that will install MedSquare and its direct dependencies.
# Default values will use known working versions of dependencies, will build and
# install all dependencies, and will erase build and source files.
#
#
#=========================================================================

# Default values:

# variables:
build_cmake="false"
build_gdcm="false"
build_gtest="false"
build_itk="false"
build_msq="true"
build_msq_post="true"
build_vtk="false"
install_prefix=$HOME/msq
interactive_installation="false"
keep_builds="false"
keep_sources="false"

# software versions:
cmake_version="2.8.4"
gdcm_version="2.0.18"
gtest_version="1.6.0"
itk_version="3.20.1"
medsquare_version="git"
vtk_version="5.10.0"


#################################
#     ***   WARNING   ***       #
# DO NOT EDIT beyond this line. #
#################################

# Variables
installed_version=""
versions_file=$install_prefix/VERSIONS
vtk_major=`echo $vtk_version | cut -d"." -f 1,2`

if [[ "$OSTYPE" == "linux-gnu" ]]; then lib_suffix='so'; else lib_suffix='dylib'; fi;


#####################
# generic functions #
#####################

# wait for stdin input and return 0 if "yes" was entered.
ask()
{
    echo -n "$1 [y] "
    read answer
    if [ "1$answer" == "1"    -o \
         "1$answer" == "1y"   -o \
         "1$answer" == "1Y"   -o \
         "1$answer" == "1yes" -o \
         "1$answer" == "1Yes" -o \
         "1$answer" == "1YES" ]; then
        return 0
    else
        return 1
    fi
}

# setup test environment
configure_environment()
{
    gtest_dir=`ls -1 $install_prefix | grep gtest`
    export DYLD_LIBRARY_PATH=$install_prefix/lib:$install_prefix/lib/vtk-$vtk_major:$install_prefix/lib/InsightToolkit:$install_prefix/bin:$install_prefix/$gtest_dir:${DYLD_LIBRARY_PATH}
    export LD_LIBRARY_PATH=$install_prefix/lib:$install_prefix/lib/vtk-$vtk_major:$install_prefix/lib/InsightToolkit:$install_prefix/bin:$install_prefix/$gtest_dir:${LD_LIBRARY_PATH}
}

# install 'package name' using 'function name'
do_install()
{
    echo "===      Installing $1       ==="
    $2
    echo "=== Finished $1 installation ==="
}

download_cmake()
{
    # cmake
    cmake_major=`echo $cmake_version | cut -d"." -f 1,2`
    echo "####   Downloading CMake sources   ####"
    curl -O -C - "http://www.cmake.org/files/v$cmake_major/cmake-$cmake_version.tar.gz"
}
download_gdcm()
{
    # GDCM
    echo "####   Downloading GDCM sources   ####"
    curl -O -C - "http://ufpr.dl.sourceforge.net/project/gdcm/gdcm%202.x/GDCM%20$gdcm_version/gdcm-$gdcm_version.tar.bz2"
}
download_gtest()
{
    # Gtest
    echo "####   Downloading GTest sources   ####"
    curl -O -C - "http://googletest.googlecode.com/files/gtest-$gtest_version.zip"
}
download_itk()
{
    # ITK
    itk_major=`echo $itk_version | cut -d"." -f 1,2`
    echo "####   Downloading ITK sources   ####"
    curl -O -C - http://ufpr.dl.sourceforge.net/project/itk/itk/$itk_major/InsightToolkit-$itk_version.tar.gz
}
download_vtk()
{
    # VTK
    echo "####   Downloading VTK sources   ####"
    curl -O -C - "http://www.vtk.org/files/release/$vtk_major/vtk-$vtk_version.tar.gz"
}

# extract version of one of the installed package from installed versions file
get_version()
{
    installed_version=""
    if [ -e $versions_file ]; then
        installed_version=`grep $1 $versions_file | sed 's/.*=//'`
    fi
}

# ask user for instructions
interactive_variables()
{
    ask "Should I continue?"
    if [ ! $? -eq 0 ]; then
        exit
    fi
    echo "Software sources will be downloaded in the current directory:"
    pwd
    ask "Do you want to keep them after compilation?"
    if [ $? -eq 0 ]; then
        keep_sources="true"
    else
        keep_sources="false"
    fi
    echo "Compilation will create object files in directories under the current directory:"
    pwd
    ask "Do you want to keep the object files after compilation?"
    if [ $? -eq 0 ]; then
        keep_builds="true"
    else
        keep_builds="false"
    fi
    echo "Which directory you want to install to? [Default=$install_prefix]"
    read directory
    if [ ! "$directory" == "" ]; then
        install_prefix=$directory
    fi
}

# run automated tests
msq_test()
{
    cd msq_build
    make test
    passed=$?
    cd ..
    if [ $passed != 0 ]; then
        exit 4
    fi
}

# set variables for dependencies that must be recompiled
recompile()
{
    get_version cmake
    if [ "$installed_version" != "$cmake_version" ]; then
        build_cmake="true"
        build_gdcm="true"
        build_gtest="true"
        build_itk="true"
        build_msq="true"
        build_msq_post="true"
        build_vtk="true"
    fi
    get_version vtk
    if [ "$installed_version" != "$vtk_version" ]; then
        #build_cmake="true"
        build_gdcm="true"
        #build_gtest="true"
        #build_itk="true"
        build_msq="true"
        build_msq_post="true"
        build_vtk="true"
    fi
    get_version gdcm
    if [ "$installed_version" != "$gdcm_version" ]; then
        #build_cmake="true"
        build_gdcm="true"
        #build_gtest="true"
        #build_itk="true"
        build_msq="true"
        build_msq_post="true"
        #build_vtk="true"
    fi
    get_version itk
    if [ "$installed_version" != "$itk_version" ]; then
        #build_cmake="true"
        #build_gdcm="true"
        #build_gtest="true"
        build_itk="true"
        build_msq="true"
        build_msq_post="true"
        #build_vtk="true"
    fi
    get_version gtest
    if [ "$installed_version" != "$gtest_version" ]; then
        #build_cmake="true"
        #build_gdcm="true"
        build_gtest="true"
        #build_itk="true"
        build_msq="true"
        build_msq_post="true"
        #build_vtk="true"
    fi
    get_version medsquare
    if [ "$installed_version" != "$medsquare_version" ]; then
        #build_cmake="true"
        #build_gdcm="true"
        #build_gtest="true"
        #build_itk="true"
        build_msq="true"
        build_msq_post="true"
        #build_vtk="true"
    fi
}

# remove files only if they actually exist
rem()
{
    if [ -e $1 ]; then
        rm $1
    fi
}

# forcefully files/dirs remove only if they exist
remd()
{
    if [ -e $1 ]; then
        echo "Removing $1"
        rm -rf $1
    fi
}

run_and_test()
{
    $1
    if [ ! $? -eq 0 ]; then
        echo "ERROR in command: \"$1\""
        exit 1
    fi
}

# checks for some prerequisites
sanity_tests()
{
    echo -n "Checking prerequisites for installation..."
    # cmake compilation requires g++
    gppTest=`which g++`
    if [ -z $gppTest ]; then
        echo "g++ compiler not found. Make sure it exists and is on the PATH."
        exit 1
    fi
    # curl is needed to download several files. Break if not present.
    which curl > /dev/null
    if [ ! $? -eq 0 ]; then
        echo "This script uses curl to download several files. Please install this program, and re-run this script."
        exit 1
    fi
    echo " Ok."
}

# update version of installed package in installed versions file
set_version()
{
    if [ -e $versions_file ]; then
        sed "/$1/d" -i $versions_file
    fi
    echo "$1=$2" >> $versions_file
}

###################################
# software installation functions #
###################################

cmake_install()
{
    if [ ! "$build_cmake" == "true" ]; then
        return
    fi
    if [ ! -e cmake-$cmake_version.tar.gz ];then
        download_cmake
    fi
    # unpacking
    if [ ! -e cmake-$cmake_version ]; then
        tar -xzvf cmake-$cmake_version.tar.gz
        if [ ! $? -eq 0 ]; then
            echo "ERROR"
            exit 1
        fi
    fi
    #installing
    cd cmake-$cmake_version
    run_and_test "./configure --prefix=$install_prefix --verbose"
    run_and_test "make"
    run_and_test "make install"
    set_version cmake $cmake_version
    cd ..
}

cpack_generate_packages()
{
    cd msq_build
    make package
}

gdcm_install()
{
    if [ ! "$build_gdcm" == "true" ]; then
        return
    fi
    if [ ! -e gdcm-$gdcm_version.tar.bz2 ];then
        download_gdcm
    fi
    tar -xjvf gdcm-$gdcm_version.tar.bz2
    if [ ! -e gdcm_build ]; then
        mkdir gdcm_build
    fi
    cd gdcm_build
    run_and_test "cmake -D CMAKE_INSTALL_PREFIX=$install_prefix -D VTK_DIR:STRING=$install_prefix/lib/vtk-$vtk_major -D GDCM_BUILD_APPLICATIONS:STRING=OFF -D GDCM_BUILD_SHARED_LIBS:STRING=ON -D GDCM_USE_VTK:STRING=ON -D GDCM_WRAP_PYTHON:STRING=OFF ../gdcm-$gdcm_version"
    run_and_test "make"
    run_and_test "make install"
    set_version gdcm $gdcm_version
    cd ..
}

gtest_install()
{
    if [ ! "$build_gtest" == "true" ]; then
        return
    fi
    if [ ! -e gtest-$gtest_version.zip ];then
        download_gtest
    fi
    remd $install_prefix/gtest-$gtest_version
    run_and_test "unzip gtest-$gtest_version.zip -d $install_prefix"
    here=`pwd`
    cd $install_prefix/gtest-$gtest_version
    run_and_test "cmake -D BUILD_SHARED_LIBS:STRING=ON ."
    run_and_test "make"
    set_version gtest $gtest_version
    cd $here
}

itk_install()
{
    if [ ! "$build_itk" == "true" ]; then
        return
    fi
    if [ ! -e InsightToolkit-$itk_version.tar.gz ];then
        download_itk
    fi
    if [ ! -e InsightToolkit-$itk_version ]; then
        tar -xzvf InsightToolkit-$itk_version.tar.gz
    fi
    if [ ! -e build_itk ]; then
        mkdir build_itk
    fi
    cd build_itk
    run_and_test "cmake -D CMAKE_INSTALL_PREFIX:STRING=$install_prefix -D BUILD_TESTING:STRING=OFF -D BUILD_EXAMPLES:STRING=OFF -D BUILD_SHARED_LIBS:STRING=ON ../InsightToolkit-$itk_version"
    run_and_test "make"
    run_and_test "make install"
    set_version itk $itk_version
    cd ..
}

msq_install()
{
    if [ ! "$build_msq" == "true" ]; then
        return
    fi
    if [ ! -e medsquare ]; then
        git clone git://medsquare.git.sourceforge.net/gitroot/medsquare/medsquare
    else
        cd medsquare
        git checkout master
        git pull
        cd ..
    fi
    gtest_dir=`ls -1 $install_prefix | grep gtest | grep -v zip`
    gdcm_major=`echo $gdcm_version | cut -d"." -f 1,2`
    if [ -z "$gtest_dir" ]; then
        echo "No gtest detected. Aborting."
        exit 1
    fi
    if [ ! -e msq_build ]; then
        mkdir msq_build
        cd msq_build
        run_and_test "cmake -D VTK_DIR:STRING=$install_prefix/lib/vtk-$vtk_major -D GDCM_DIR:STRING=$install_prefix/lib/gdcm-$gdcm_major -D CMAKE_INSTALL_PREFIX:STRING=$install_prefix -D ITK_DIR:STRING=$install_prefix/lib/InsightToolkit -D GTEST_LIBRARY:STRING=$install_prefix/$gtest_dir/libgtest.$lib_suffix -D GTEST_INCLUDE_DIR:PATH=$install_prefix/$gtest_dir/include -D GTEST_MAIN_LIBRARY:STRING=$install_prefix/$gtest_dir/libgtest_main.$lib_suffix ../medsquare/source"
        make
        cd ..
    fi
    cd msq_build
    run_and_test "cmake -D VTK_DIR:STRING=$install_prefix/lib/vtk-$vtk_major -D GDCM_DIR:STRING=$install_prefix/lib/gdcm-$gdcm_major -D CMAKE_INSTALL_PREFIX:STRING=$install_prefix -D ITK_DIR:STRING=$install_prefix/lib/InsightToolkit -D GTEST_LIBRARY:STRING=$install_prefix/$gtest_dir/libgtest.$lib_suffix -D GTEST_INCLUDE_DIR:PATH=$install_prefix/$gtest_dir/include -D GTEST_MAIN_LIBRARY:STRING=$install_prefix/$gtest_dir/libgtest_main.$lib_suffix ../medsquare/source"
    run_and_test "make"
    run_and_test "make install"
    set_version medsquare $medsquare_version
    cd ..
}

msq_post_install()
{
    if [ ! "$build_msq_post" == "true" ]; then
        return
    fi
    echo "export LD_LIBRARY_PATH=$install_prefix/lib:$install_prefix/lib/vtk-$vtk_major:$install_prefix/bin:$install_prefix/lib/InsightToolkit:${LD_LIBRARY_PATH}" > $install_prefix/MedSquare.sh
    echo "export DYLD_LIBRARY_PATH=$install_prefix/lib/vtk-$vtk_major:$install_prefix/bin:${DYLD_LIBRARY_PATH}" >> $install_prefix/MedSquare.sh
    echo "export PYTHONPATH=$install_prefix/lib/python2.6/site-packages" >> $install_prefix/MedSquare.sh
    echo "export PATH=$install_prefix/bin:$PATH" >> $install_prefix/MedSquare.sh
    echo "$install_prefix/bin/MedSquare" >> $install_prefix/MedSquare.sh
    chmod u+x,og-rwx $install_prefix/MedSquare.sh
    echo "Current MedSquare.sh script:"
    cat $install_prefix/MedSquare.sh
}

vtk_install()
{
    if [ ! "$build_vtk" == "true" ]; then
        return
    fi

    if [ ! -e vtk-$vtk_version.tar.gz ]; then
        download_vtk
    fi
    if [ ! -e VTK ]; then
        tar -xzvf vtk-$vtk_version.tar.gz
    fi

    # there is a bug in a file that prevents vtktiff to be proeperly compiled, and this is needed for gdcm.
    # the block below takes care of that, if the bug is detected.
    echo -n "Checking for vtktiff bug... "
    TIFFBUG=`cat VTK/Utilities/vtktiff/CMakeLists.txt | grep tiffconf.h`
    if [ "$TIFFBUG" == "" ]; then
        echo "VTK/Utilities/vtktiff/CMakeLists.txt has a bug, correcting it as per http://vtk.1045678.n5.nabble.com/missing-install-file-tiffconf-h-td2651846.html"
        cat VTK/Utilities/vtktiff/CMakeLists.txt | sed -n '1h;1!H;${;g;s/\${VTKTIFF_SOURCE_DIR}\/tconf\.h/\${VTKTIFF_SOURCE_DIR}\/tconf.h\n    \${VTKTIFF_SOURCE_DIR}\/tiffconf.h/g;p;}' > tempfile
        mv -f tempfile VTK/Utilities/vtktiff/CMakeLists.txt
    fi
    echo "Done."

    if [ ! -e vtk_build ]; then
        mkdir vtk_build
    fi
    cd vtk_build

    # find the path for glx.h
    glx_path=`locate glx.h | head -n 1 | sed 's/\(^.*\)\/GL\/glx.h/\1/'`

    run_and_test "cmake -D CCMAKE_CXX_FLAGS:STRING='-U VTK_IMPLEMENT_MESA_CXX' -D VTK_USE_X:STRING=OFF -D BUILD_SHARED_LIBS:STRING=ON -D BUILD_TESTING:STRING=OFF -D CMAKE_INSTALL_PREFIX:STRING=$install_prefix -D VTK_USE_QT:STRING=ON -D VTK_WRAP_PYTHON:STRING=ON -D VTK_USE_GUISUPPORT:STRING=ON -D CMAKE_BUILD_TYPE:STRING=DEBUG -D OPENGL_INCLUDE_DIR:STRING=$glx_path -D VTK_USE_QVTK_QTOPENGL:STRING=ON -D VTK_USE_TK:STRING=OFF ../VTK"

    run_and_test "make"
    run_and_test "make install"
    set_version vtk $vtk_version

    cd ..
}

##################
# "main" program #
##################

echo "################################################"
echo "####                                        ####"
echo "####           MedSquare install            ####"
echo "####                                        ####"
echo "################################################"
echo
echo "This script is for developers to install MedSquare"
echo "and all its dependencies from scratch, on a home"
echo "directory, and not use any system libraries. It is"
echo "a failsafe install. Use at your own risk."
echo

if [ $# -ge 1 ]; then
    first_parameter="$1"
fi

if [ "$interactive_installation" == "true" ]; then
    interactive_variables
fi

sanity_tests

if [ ! -e $install_prefix ]; then
    echo "$install_prefix does not exist. Creating."
    mkdir -p $install_prefix
fi

# update PATH environment variable so that we use new tools as soon as they become available
export PATH=$install_prefix/bin:$PATH

# update PYTHONPATH environment variable so that VTK can install and use Python wrappings
export PYTHONPATH=$install_prefix/lib/python2.6/site-packages:$PYTHONPATH

if [ "$first_parameter" == "cpack" ]; then
    rm $versions_file
    keep_builds="true"
fi

recompile

do_install cmake cmake_install
do_install vtk vtk_install
do_install itk itk_install
do_install gdcm gdcm_install
do_install "Google Test" gtest_install
do_install MedSquare msq_install
do_install "post-install MedSquare.sh update" msq_post_install

if [ "$first_parameter" == "cpack" ]; then
    do_install CPack cpack_generate_packages
fi

# Automated tests
if [ "$first_parameter" == "test" ]; then
    configure_environment
    msq_test
fi

if [ "$keep_sources" == "false" ]; then
    echo "####   Removing source files   ####"
    rem cmake-$cmake_version.tar.gz
    rem vtk-$vtk_version.tar.gz
    rem InsightToolkit-$itk_version.tar.gz
    rem gdcm-$gdcm_version.tar.gz
    rem gtest-$gtest_version.zip
fi

if [ "$keep_builds" == "false" ]; then
    echo "####   Removing object files   ####"
    remd cmake-$cmake_version
    remd vtk_build
    remd vtk.build
    remd VTK
    remd InsightToolkit-$itk_version
    remd build_itk
    remd gdcm-$gdcm_version
    remd gdcm-$gdcm_version.tar.gz
    remd gdcm_build
    remd msq_build
    remd build_msq
    remd medsquare
fi
