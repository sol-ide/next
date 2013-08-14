set BOOST_ROOT=D:/prod/boost
set BOOST_LIBRARYDIR=D:/prod/boost/stage/lib
mkdir stage
cd stage
rmdir /S /Q build_win32
mkdir build_win32
cd build_win32
cmake ../..
