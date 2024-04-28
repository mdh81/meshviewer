#!/bin/bash
buildType="Debug"
browser="chromium"
while [ "$#" -gt 0 ]; do
    case "$1" in
        -t|--buildType) buildType="$2"; shift 2;;
        -b|--browser) browser="$2"; shift 2;;
        --) shift; break;;
        -*) echo "Unknown option: $1" >&2; exit 1;;
        *) break;;
    esac
done
pushd ./build_web
emcmake cmake -S ../ B . -DEMSCRIPTEN=On -DCMAKE_BUILD_TYPE="$buildType" &&
make -j 10 VERBOSE=1
if [ $? -eq 0 ]; then
  emrun --port 5600 --browser "$browser" meshViewer.html /testfiles/suzanne_subdivided.stl &
fi
popd

