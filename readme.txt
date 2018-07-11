Collection of all the code and scripts necessary to reindex the ebba database.

run pip install --user -r requirements.txt
assumes running on a computer cluster wiht bash, many cores, opencv built

The process has been segmented and ordered into 4 stages:
(0) prep
(1) process
(2) cluster
(3) updatedb

Then a final directory 'validation' will have the scripts that aid in the manual validation
(such as intercluster edge number finder...)


