#!/bin/bash
source ./vars.sh
now=$(date)

echo "Start time : $now"
  parallel --jobs 100 ./src/process.exe ::: $images*.{jpg,tif}

nnow=$(date)
echo "Stop time : $nnow"

echo "zipping "
cd $keypoints
zip keypoints.zip *
mv keypoints.zip $output

