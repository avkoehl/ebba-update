#!/bin/bash
source ../vars.sh

cwd=$(pwd)

cd $images
magick mogrify -format jpg *.tif
rm *.tif

cd $cwd



