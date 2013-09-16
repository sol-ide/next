set BOOST_ROOT=D:/prod/boost
set OLD_PATH=%PATH%
PATH=%PATH%;%CD%\external\win32\xsltproc\bin;%CD%\external\win32\doxygen\bin;%CD%\external\win32\unzip\bin
rmdir /S /Q build
mkdir build
cd build
rmdir /S /Q win32
mkdir win32
cd win32
set BOOST_LIBRARYDIR=D:/prod/boost/stage/win32
rem HACK: MSVC cannot build debug and release due to bug http://kwwidgets.org/Bug/view.php?id=6493 in cmake
cmake ../.. -DCMAKE_INSTALL_PREFIX=next_install_win32 -DBUILD_DOCUMENTATION=ON -DBUILD_DOCUMENTATION_HTML=ON -DDOCBOOK_AUTOCONFIG=ON -DBUILD_QUICKBOOK=ON -DENABLE_DEBUG=ON -DENABLE_RELEASE=OFF -DBUILD_TESTS=ALL -G "Visual Studio 12"
cd ..
rmdir /S /Q win64
mkdir win64
cd win64
set BOOST_LIBRARYDIR=D:/prod/boost/stage/win64
cmake ../.. -DCMAKE_INSTALL_PREFIX=next_install_win64 -DBUILD_DOCUMENTATION=ON -DBUILD_DOCUMENTATION_HTML=ON -DDOCBOOK_AUTOCONFIG=ON -DBUILD_QUICKBOOK=ON -DENABLE_DEBUG=ON -DENABLE_RELEASE=OFF -DBUILD_TESTS=ALL -G "Visual Studio 12 Win64"
cd ../..
PATH=%OLD_PATH%