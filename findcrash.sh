#!/bin/bash
# This script can be used to find heisenbugs. It runs the program with different seeds until the return is not 0

if [ -z "$1" ] || [ -z "$2" ]; then
    echo "Usage: $0 <start_value> <end_value>"
    exit 1
fi

start_value=$1
end_value=$2

if [ $start_value -gt $end_value ]; then
    echo "Start value must be less than or equal to end value"
    exit 1
fi

s=$start_value

while [ $s -le $end_value ]; do
    cmake-build-debug/pcu icon.png test.png -s $s
    if [ $? -ne 0 ]; then
        break
    fi
    ((s++))
done