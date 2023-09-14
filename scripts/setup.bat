echo off

cd ..
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DMUHLE_SHARED_LIBRARY=ON -DMUHLE_BUILD_GUI=ON -DMUHLE_BUILD_TESTS=ON -A x64
cd ..\scripts

pause
