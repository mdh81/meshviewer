#!/bin/bash
pushd ./build_web
emcmake cmake -S ../ B . -DEMSCRIPTEN=On -DCMAKE_BUILD_TYPE=Debug &&
make -j 8 VERBOSE=1
if [ $? -eq 0 ]; then
  emrun --port 5600 --browser chromium meshViewer.html /testfiles/cylinder.stl &
fi
popd

