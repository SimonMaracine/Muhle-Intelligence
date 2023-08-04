echo off

cd ..
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DMUHLE_SHARED_LIBRARY=ON -A x64
cd ..\scripts

pause
