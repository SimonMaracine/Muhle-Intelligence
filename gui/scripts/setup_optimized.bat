echo off

cd ..
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -A x64
cd ..\scripts

pause
