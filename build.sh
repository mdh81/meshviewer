#!/bin/bash

srcDir=$(dirname `realpath $0`)
buildDir=""

while [[ !$# -eq 0 ]]; do
    case "$1" in
        -v|--verbose)
            verbose=1
            shift
            ;;
        -b|--buildDir)
            buildDir="$2"
            shift 2
            ;;
        -c|--clean)
            cleanFirst=1
            shift
            ;;
        *)
            echo "Ignoring unknown argument $1"
            shift
            ;;
    esac
done

if [[ "$verbose" -eq 1 ]]; then
    buildOpts="--verbose"
fi
if [ "$cleanFirst" -eq 1 ]; then
    buildOpts="$buildOpts --clean-first"
fi

if [[ -z "$buildDir" ]]; then
    buildDir="$srcDir/build"
fi

pushd "$srcDir"

mkdir -p "$buildDir"

git submodule update --init --recursive
cmake -S "$srcDir" -B "$buildDir"
cmake --build "$buildDir" $buildOpts --parallel

popd
