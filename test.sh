#!/bin/bash
set -e

if [[ -z "$1" ]]; then
    echo "Usage: $0 <build dir>"
    exit -1
fi

buildDir="$1"

testDirs=$(find "$buildDir" -name CTestTestfile.cmake -exec dirname {} \; | grep -E -v "_deps|tests")
for testDir in $testDirs; 
do 
    pushd "$testDir" &> /dev/null
    ctest . --stop-on-failure --output-on-failure
    popd &> /dev/null
done
