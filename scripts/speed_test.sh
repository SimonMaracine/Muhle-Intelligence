#! /bin/bash

clear
cd ../build
cmake --build . -j 8 --target test_move_generation_speed

if [ "$?" -ne 0 ]; then
    exit 1
fi

cd tests/test_move_generation_speed/
./test_move_generation_speed
