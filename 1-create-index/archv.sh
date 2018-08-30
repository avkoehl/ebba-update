#!/bin/bash
source ../vars.sh
now=$(date)

echo "Start time : $now"
  parallel --jobs 100 ./src/archv.exe ::: $images*.jpg

nnow=$(date)
echo "Stop time : $nnow"

echo "concatinating results into $output/results/adjacency-list.csv"

cd $matches
for f in *.txt; do (cat $f; echo '') >> $output/results/adjacency-list.csv; done

