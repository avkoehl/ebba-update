#!/bin/bash
source ../vars.sh

cwd=$(pwd)

mkdir ./thumbnails
thumbnails=./thumbnails

cd $images
magick mogrify -resize 75 -path $cwd$thumbnails *.jpg
magick mogrify -resize 75 -path $cwd$thumbnails *.tif

cd $cwd
