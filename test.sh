if [[ -z "$1" ]]; then
    echo "Usage: $0 <build dir>"
    exit -1
fi

testDirs=$(find . -name CTestTestfile.cmake -exec dirname {} \; | grep -E -v "_deps|tests")
for testDir in $testDirs; 
do 
    pushd "$testDir"
    ctest --stop-on-failure .
    popd 
done
