#!/bin/bash
source vars.sh
now=$(date)

echo "Start time : $now"
  parallel --jobs 1 ./src/process.exe ::: $images*.jpg

nnow=$(date)
echo "Stop time : $nnow"

echo "zipping "
cd $keypoints
zip keypoints.zip *
mv keypoints.zip $output

