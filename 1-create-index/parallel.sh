#!/bin/bash

now=$(date)

echo "Start time : $now"
  parallel --jobs 100 ./makeAdjacency.exe ::: *.jpg

nnow=$(date)
echo "Stop time : $nnow"

#find *.txt | xargs -I[] sh -c "cat {}; echo ''" > results.csv
