#!/bin/bash
source ./../vars.sh

cwd=$(pwd)

mkdir thumbnails
thumbnails=thumbnails


cd $images
magick mogrify -resize 200 -path $cwd/$thumbnails *.jpg

cd $cwd
