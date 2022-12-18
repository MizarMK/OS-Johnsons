#!/bin/bash

set -e
make -f Makefile

file1="./testout.txt"
file2="./actualout.txt"

echo "2360 295.0" > "$file1"
echo `java PageSim "input1"` > "$file2"

if cmp -s "$file1" "$file2"; then
	echo "pass input 1"
else
	echo "Failed expected output for input1"
	exit 1
fi

echo "1660 207.5" > "$file1"
echo `java PageSim "input2"` > "$file2"

if cmp -s "$file1" "$file2"; then
        echo "pass input 2"
else
        echo "Failed expected output for input2"
        exit 1
fi

echo "ERROR: TLB address details must be positive; exiting..." > "$file1"
echo `java PageSim "input3"` > "$file2"

if [$? -ne 2]; then
        echo "Failed expected output for input3"
        exit 1
fi

echo "ERROR: Memory Access cost must be non-negative" > "$file1"
echo `java PageSim "input4"` > "$file2"

if [$($?) -ne 4]; then
        echo "Failed expected output for input4"
        exit 1
fi

echo "ERROR: TLB cost or size details invalid" > "$file1"
echo `java PageSim "input5"` > "$file2"

if [$? -ne 5]; then
        echo "Failed expected output for input5"
        exit 1
fi

echo "ERROR: TLB cost or size details invalid" > "$file1"
echo `java PageSim "input6"` > "$file2"

if [$? -ne 5]; then
        echo "Failed expected output for input6"
        exit 1
fi

echo "ERROR: this address cannot be represented using the address args passed; exiting..." > "$file1"
echo `java PageSim "input7"` > "$file2"

if [$? -ne 11]; then
        echo "Failed expected output for input7"
        exit 1
fi

echo "ERROR: address input may not be negative" > "$file1"
echo `java PageSim "input8"` > "$file2"

if [$? -ne 6]; then
        echo "Failed expected output for input8"
        exit 1
fi
