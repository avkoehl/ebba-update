Note that about 3000 images that are in the images directory have both a .tif and .jpg version. Currently, the keypoints file only has a keypoint file for whichever one came second as it will have overwrote the first. This means in our matches we have some strange results that need to be cleaned.

First figure out how to clean this.
   find all the duplicates, see in what order they were processed (i think tiff second)
   if thats the case, simply delete all the lines for the .jpg version in adjacency list
   then rerun the find edges

Second figure out how to solve this problem in the future!



Also, make cluster.sh that creates the edges.txt, places that in the update-date dir
then it finds the clusters and updates the adjacency in update-dir

Make sure the process images and the archv shell scripts are timed, also have it find the number of jobs dynamically based on number of processors/threads that are open. 
