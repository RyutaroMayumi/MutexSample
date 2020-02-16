#!/bin/bash

rm -rf build
mkdir build
cd build

: ${configs:=Debug Release}

cmake -G"Visual Studio 14 Win64" ../
for config in ${configs}; do
	cmake --build . --config $config
done

cd ..

