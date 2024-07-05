echo off

cd ..
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -A x64
cd ..\scripts

pause
