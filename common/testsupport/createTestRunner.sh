#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Error: Binary directory is a required argument"
    echo "Usage: "$0" <binary_directory_with_all_tests>"
    exit 1
fi

allTests=$(find "$1" -iname '*_test' -type f -perm +111)

if [ -f "$1"/testRunner.sh ]
then
  rm "$1"/testRunner.sh
fi

oldIFS="$IFS"
IFS=$'\n'

for test in $allTests
do
  echo "echo Running test $test" >> "$1"/testRunner.sh
  echo "$test" >> "$1"/testRunner.sh
done

chmod a+x "$1"/testRunner.sh

IFS="$oldIFS"