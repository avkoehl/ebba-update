# PYTHON 3



# ============================================================ #
### Read in the Clusters
# ============================================================ #
clusters = []
f = open ("clusters_1-25_2018.txt")
for line in f:
    clusters.append(line)

# ============================================================ #
### Generate an HTML file and image style
# ============================================================ #
o = open ("results_all.html", "w")
path = "\"/home/arthur/imagesets/Small/"
print ("<doctype HTML><head><title>visualize clusters</title></head><body>", file= o)
print ("<style> img { width: 150px; } </style>", file=o)



# ============================================================ #
### Load the Thumbnails into the html page to visualize 
# ============================================================ #
    # WANT TO OUTPUT BELOW EACH IMAGE - 
    # THE NUMBER OF MATCHES IT HAS, 
    # THE INTRACLUSTER VERTEX DEGREE, 
    # INTERCLUSTER VERTEX DEGREE
    # for now just getting the matches, until I figure out a way to get those other values (should be simple)

count = 0 
for i in clusters:
    images = i.split("\t")
    if count > 120 and count < 150:
        print ("<p> Cluster id: ", count, file=o)
        print ("  Size ", len(images) , "</p>", file=o)

        for image in images:
           print ("<img title=\"", image, "\"src=", path + image, "\"/>", file=o)

    print ("<hr>", file=o)
    count = count + 1



print ("</body></html>", file=o)
o.close()



    
