#!/bin/bash

# Check if the correct number of arguments are provided
if [ "$#" -ne 4 ]; then
    echo "Usage: $0 <min_value_row> <max_value_row> <min_value_col> <max_value_col>"
    exit 1
fi

# Get the max values from the arguments
min_value_row=$1
max_value_row=$2
min_value_col=$3
max_value_col=$4

# Loop through the ranges
for i in $(seq -w $min_value_row $max_value_row); do
    for j in $(seq -w $min_value_col $max_value_col); do
        echo -n "Pixel $i $j: "
        ./cmake-build-debug/pngpixel $i $j icon.png
    done
done
