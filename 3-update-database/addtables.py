### Arthur Koehl
# Program that reads in the adjacency list (with cluster ids)
# adds a new table to the csv containing matches for each image
# as well as the cluster id

import MySQLdb as mdb

####################################################################
### CONNECT TO THE MYSQL DATABASE
####################################################################
con = mdb.connect('127.0.0.1', 'root', '', 'ebbasite')
cur = con.cursor()

####################################################################
### PROCESS THE CSV FILE
####################################################################
f = open ("matches_and_cluster_1-25_2018.csv", "r")
fnames = [] 
clusterids = []
matches = []

for line in f:
    allmatches = ""
    elements = line.split(',')
    allmatches = ",".join(elements[3:])

    fnames.append(elements[0])
    clusterids.append(elements[1])
    matches.append(allmatches)


####################################################################
### FROM TABLE BIA_IMPRESSIONS GET ALL THE IMPRESSION IDS 
####################################################################
imp_id = []
for i in range(len(fnames)):
    s = fnames[i].rstrip()  
    cur.execute("SELECT BIA_IMP_ID FROM bia_impressions where BIA_IMP_File = %s", (s, ))
    row = cur.fetchone()
    impid = row[0]
    imp_id.append(impid)


####################################################################
### WRITE NEW TABLE WITH 5 COLUMNS: 
# Id, BIA_IMP_ID, BIA_IMP_File, CLUSTER_ID, MATCHES
####################################################################

cur.execute("DROP TABLE IF EXISTS bia_impressions_index")
cur.execute("CREATE TABLE bia_impressions_index(Id INT PRIMARY KEY AUTO_INCREMENT, BIA_IMP_ID BIGINT, BIA_IMP_FILE VARCHAR(25), CLUSTER_ID INT, MATCHES TEXT)")

with con:
    for i in range(len(fnames)):
        impId = imp_id[i]
        impFile = str(fnames[i]).rstrip()
        clusterid = clusterids[i].rstrip()
        if ("none" in clusterid):
            clusterid = -1
        matchlist = matches[i]
        cur.execute("INSERT INTO bia_impressions_index (BIA_IMP_ID, BIA_IMP_FILE, CLUSTER_ID, MATCHES) VALUES(%s,%s,%s,%s)", (impId,impFile,clusterid,matchlist))
