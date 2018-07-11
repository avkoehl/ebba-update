import yaml

def get_clusters (clustersfile):
    clusters = []
    with open (clustersfile) as f:
    	for line in f:
            line = line.rstrip()
            clusters.append(line)
    return clusters

def get_clusterid (name, clusters):
    idx = -1
    for i,c in enumerate(clusters):
        if name in c:
            idx = c
            break
     return idx

def main():
    params = yaml.load(open("../settings.yml"))
    original_list = params["outputdir"] + "results/" + "adjacency-list.csv"
    clustersfile = params["outputdir"] + "results/" + "clusters.txt"
    output = params["outputdir"] + "results/" + "updated-adjacency-list.csv"
    clusters = get_clusters (clustersfile)
    
    f = open (original_list, "r")
    o = open (output, "w")

    for line in f:
        line = line.rstrip()
        elements = line.split(",")
        name = elements[0]

        # for cluster in clusters, see if name is in that list
        idx = get_clusterid (name, clusters)

        # splice in cluster 
        cid = str(idx)
        elements.insert(1, cid)

        # print new elements as a csv into new csv o
        result= (",").join(elements)
        print (result, file=o)

    f.close()
    o.close()

if __name__ == "__main__":
    main()




    

