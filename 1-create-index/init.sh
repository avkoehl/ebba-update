#!/bin/bash
source ./vars.sh
now=$(date)

declare -a IMAGES=$images*{jpg,tif}

#echo $images*{jpg,tif}
ext=".jpg"
cnt=0

for i in $images*.tif
do
    base=${i%.*}
    if [ -a $base$ext ]; then
    	echo $i
        mv $i /data2/dsl/ballads/backup-tifs/
        let "cnt++"
    fi
done
echo $cnt

#parallel --jobs 100 ./src/process.exe ::: $images*.{jpg,tif}


