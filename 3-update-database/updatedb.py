"""
     Program that updates database
     bia_balladImpressions, bia_impressions
     updates these 2 tables with new ballads 

     bia_impressions_index
     deletes the old matches table and adds new one with new results
 """

import pymysql as mdb

con = mdb.connect('127.0.0.1', 'root', '##565qwsxcv', 'EBBA')
cur = con.cursor()

def process_csv (fname):
    """ get a list of all the impressions, name, matches, cluster""" 
    impression_list = []
    f = open (fname, "r")
    for line in f:
        impression = {}
        elements = line.split(',')
        fullname = elements[0]
        if (fullname):
            impression["name"] = fullname
            impression["clusterid"] = elements[1]
            impression["matches"] = ",".join(elements[3:]).rstrip()
            impression_list.append(impression.copy())

    return impression_list

def get_missing (impressions):
    """ get a list of the impressions that aren't in the database """
    missing = []
    included = []
    names = []

    for imp in impressions:
        names.append(imp["name"])

    cur.execute("SELECT BIA_IMP_File FROM bia_impressions")
    result_set = cur.fetchall()
    for row in result_set:
        included.append(row[0])

    missing = [n for n in names if n not in included]
    return missing

def update_tables (missing):
    """ add the impressions that are missing to the database and give them ids 
        two tables to be updated: bia_impressions bia_balladImpressions 

        columns to add: 
            base_id      - starting point for ids,
            bdi_id       - id to be generated
            bdi_bdid     - ballad id (i.e 3000) 
            bdi_impid    - same as bdi_bdid, 
            bdi_number   - impression number (i.e 2), 
            imp_file     - fullname (i.e 3000-20.jpg)
    """

    cur.execute("SELECT BIA_IMP_ID FROM bia_impressions ORDER BY BIA_IMP_ID DESC LIMIT 1")
    result = cur.fetchall()
    bdi_base_id = result[0][0] + 1

    with con:
       for i,imp in enumerate(missing):
           bdi_id = bdi_base_id + i
           bdi_bdid = imp.split('-')[0]
           bdi_impid = bdi_id
           imp_file = imp
           
           temp = imp.split('-')
           temp2 = temp[1]
           temp3 = temp2.split('.')
           bdi_number = temp3[0][:-1]

           cur.execute("INSERT INTO bia_balladImpressions (BIA_BDI_ID, BIA_BDI_BDID, BIA_BDI_IMPID, BIA_BDI_Number) VALUES(%s,%s,%s,%s)", (bdi_id, bdi_bdid, bdi_impid, bdi_number))
           cur.execute("INSERT INTO bia_impressions (BIA_IMP_ID, BIA_IMP_File) VALUES (%s,%s)", (bdi_impid, imp_file))
    return

def get_impids (impressions):
    """ get a list of the impression ids """
    impids = []
    cur.execute("SELECT BIA_IMP_ID FROM bia_impressions")
    result = cur.fetchall()
    for row in result:
        impids.append(row[0])
    return impids



def add_tables (impressions):
    """ delete table then add new table with 5 columns:
        auto incrementing Id, BIA_IMP_ID, BIA_IMP_File, CLUSTER_ID, MATCHES
    """
    impids = get_impids(impressions)

    cur.execute("DROP TABLE IF EXISTS bia_impressions_index")
    cur.execute("CREATE TABLE bia_impressions_index(Id INT PRIMARY KEY AUTO_INCREMENT, BIA_IMP_ID BIGINT, BIA_IMP_FILE VARCHAR(25), CLUSTER_ID INT, MATCHES TEXT)")

    with con:
        for i,imp in enumerate(impressions):
            impId = impids[i]
            impFile = imp["name"]
            clusterid = imp["clusterid"] 
            matchlist = imp["matches"]
            cur.execute("INSERT INTO bia_impressions_index (BIA_IMP_ID, BIA_IMP_FILE, CLUSTER_ID, MATCHES) VALUES(%s,%s,%s,%s)", (impId,impFile,clusterid,matchlist))

def main():
    print ("reading in csv file")
    impressions = process_csv("combined.csv")

    ## update existing tables with new impressions
    # find the impressions that don't have row yet
    # add them to the table
    print ("finding new impressions that need to be added")
    missing = get_missing(impressions)
    print ("added", len(missing), "impressions to the database") 
    update_tables(missing)

    ## update the matches table
    # delete the existing table of matches
    # then add table of matches
    print ("deleting matches table and then filling it with new results")
    add_tables(impressions)

if __name__=="__main__":
    main()

