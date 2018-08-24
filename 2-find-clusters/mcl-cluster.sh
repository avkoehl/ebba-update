#!/bin/bash
source ../vars.sh

mcl ./edges.txt --abc -o clusters.tsv
cp clusters.tsv $output/results/

