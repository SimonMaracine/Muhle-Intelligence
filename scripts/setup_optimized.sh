#! /bin/bash

cd ..
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DMUHLE_SHARED_LIBRARY=ON -DMUHLE_BUILD_GUI=ON -DMUHLE_BUILD_TESTS=ON
