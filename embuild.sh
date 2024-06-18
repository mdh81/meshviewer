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
buildDir="./build_web"
buildDir+="_${buildType}"
mkdir -p "$buildDir"
pushd "$buildDir" || exit
emcmake cmake -S ../ -B . -DEMSCRIPTEN=On -DCMAKE_BUILD_TYPE="$buildType" &&

if make -j 10 VERBOSE=1
then
  cp meshViewer.html index.html
  kill -9 $(pgrep -fl python3 | grep emrun | cut -d ' ' -f 1)
  emrun --port 5600 --browser "$browser" index.html &
fi
popd

