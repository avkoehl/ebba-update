### Arthur Koehl
# PYTHON 2
# Program that updates the exisitng database tables
# The two tables that it updates are: bia_balladImpressions, bia_impressions
# These talbes need to be updated to include 
# impressions that were indexed, but don't already exist in the db

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
bdids = []
fulls = []
nums = []
for line in f:
    elements = line.split(',')
    full = elements[0]
    fulls.append(elements[0])
    split = full.split('.')
    fname = split[0]
    split2 = fname.split('-')
    bdids.append(split2[0])
    temp = split2[1]
    temp2 = temp.split('.')
    temp3 = temp2[0]
    nums.append(temp3[:-1])


####################################################################
### FIND THE MISSING IMPRESSIONS THAT ARE IN THE CSV BUT NOT IN DB
####################################################################
missing = []
for i in range(len(fulls)):
    s = str(fulls[i]).rstrip().lstrip()
    if cur.execute("SELECT * FROM bia_impressions where BIA_IMP_File = %s", (s, )):
        continue
    else:
        missing.append(s)

####################################################################
### ADD THE MISSING IMPRESSSIONS TO bia_impressions and bia_balladImpressions
####################################################################
#bdi_base_id = cur.execute("SELECT BIA_IMP_ID FROM bia_impressions ORDER BY BIA_IMP_ID DESC LIMIT 1") #11642 not working
bdi_base_id = 11759 #should be dynamic with commented line above...
bdi_bdid = ""
bdi_impid = ""
bdi_number = ""
imp_file = ""

with con:
    for i in range(len(missing)):
        fullname = missing[i]
        bdi_id = bdi_base_id + i
        bdi_bdid = fullname.split('-')[0]
        bdi_impid = bdi_id
        imp_file = fullname
        temp = fullname.split('-')
        temp2 = temp[1]
        temp3 = temp2.split('.')
        bdi_number = temp3[0][:-1]
        
        cur.execute("INSERT INTO bia_balladImpressions (BIA_BDI_ID, BIA_BDI_BDID, BIA_BDI_IMPID, BIA_BDI_Number) VALUES(%s,%s,%s,%s)", (bdi_id, bdi_bdid, bdi_impid, bdi_number))
        cur.execute("INSERT INTO bia_impressions (BIA_IMP_ID, BIA_IMP_File) VALUES (%s,%s)", (bdi_impid, imp_file))
