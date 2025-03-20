#!/bin/bash

srcDir=$(dirname "$(realpath "$0")")
buildType="debug"
buildDir="cmake-build-$buildType"

while [[ ! $# -eq 0 ]]; do
    case "$1" in
        -h|--help)
            printf "Usage: %s <options>\n" "$0"
            printf "Options are:
                    -v|--verbose   Turn on verbose output
                    -b|--buildDir  Set build directory where the object files will to placed
                    -c|--clean     Create a clean build
                    -t|--buildType Build type. Must be debug or releasee (Defaults to debug)
                    -h|--help      Print this message and exit\n"
            exit 0
            ;;
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
        -t|--type)
            buildType="$2"
            shift 2
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

if [[ "$cleanFirst" -eq 1 ]]; then
    buildOpts="$buildOpts --clean-first"
fi

if [[ -z "$buildDir" ]]; then
    buildDir="$srcDir/build"
fi

pushd "$srcDir" || exit

mkdir -p "$buildDir"

git submodule update --init --recursive
cmake -S "$srcDir" -B "$buildDir" -DCMAKE_BUILD_TYPE="$buildType"
cmake --build "$buildDir" $buildOpts --parallel

popd || exit
