#!/bin/bash

rm -rf build
mkdir build
cd build

: ${configs:=Debug Release}
for config in ${configs}; do
	cmake -H../ -DCMAKE_BUILD_TYPE=$config -B./$config -G"Eclipse CDT4 - Unix Makefiles"
	cmake --build $config --target clean
	cmake --build $config --target all
done

cd ..

