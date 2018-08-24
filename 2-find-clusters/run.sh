#!/bin/bash
source ../vars.sh

## 1. Make thumbnails if they don't exist
echo "creating thumbnails"
sh create-thumbnails.sh

## 2. run mcl
echo "running mcl"
sh mcl-cluster.sh

## 3. create webpages to view clusters
echo "creating html pages"
python3 view-clusters.py
cp *.html $output/results/

## 4. combine adjacency list with clusters
echo "update csv to include cluster info"
python3 combine.py

## 5. clean up
echo "cleaning up"
rm edges.txt clusters.tsv
