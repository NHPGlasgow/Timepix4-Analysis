#!/bin/bash

dates=( "250203" "250204" "250205" "250206" )
#for dir in /w/work5/eic/Timepix/MainzTests/data.2502*;
for date in ${dates[*]}
do
    dir=/w/work5/eic/Timepix/MainzTests/data.$date
    outfile=$date.txt
    #echo $dir
    rm -f $outfile
    touch $outfile
    for dir2 in $dir/run-*;
    do
	#echo $dir2
	if [ -z "$(ls -A $dir2/ )" ];then
	    continue;
	fi
	
	files=($dir2/*)
	file=${files[0]##"$dir2/"}
	det=`echo $file | cut -d'-' -f 1`
	dethalf=`echo $file | cut -d'-' -f 2`
	date2=`echo $file | cut -d'-' -f 3`
	time=`echo $file | cut -d'-' -f 4`
	size=`du -kh $dir2 | cut -f1`
	echo $det, $time, $size >> $outfile
	
    done
done
