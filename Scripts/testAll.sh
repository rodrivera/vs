#!/bin/bash
AUX=""
#AUX="../"

qFile="$1"
queries="$AUX""data/queries/""$qFile"".txt"

if [ ! -f "$queries" ]; then
    echo "Queries file not found!"
    echo "Usage: testAll.sh search_window_SIZE"
    echo "with size between 0 and 100"
    exit
fi

declare -a SUFFIXES=("0" "10" "25" "50")
for N in "${SUFFIXES[@]}"
do
	intervals="$AUX""data/intervals/""$N"".txt"
	outR="$AUX""data/output/R/""$qFile""-""$N"".txt"
	outI="$AUX""data/output/I/""$qFile""-""$N"".txt"
	outbI="$AUX""data/output/bI/""$qFile""-""$N"".txt"
	echo "----- Intervals size = ""$N"" and QueryWindow size = ""$qFile"".. -----"
	#echo "Executing.. FNR with ""$N"" objects.."
	./"$AUX"rtest.out $intervals $queries $outR
	#echo "Executing..   X with ""$N"" objects.."
	./"$AUX"itest.out $intervals $queries $outI

	./"$AUX"bitest.out $intervals $queries $outI

	echo ""; echo "Checking differences between Output files.."
	if diff $outR $outI >/dev/null; then
		printf "Passed\n"
	else
		printf "¡¡ Failed !!\n"
		#sdiff $outFNR $outX
		exit
	fi
	echo "-------------------------------------------"; echo ""
done
