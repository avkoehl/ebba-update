import os

imgpath = "../../thumbnails/"

def get_clusters (fname):
    clusters = []
    f = open (fname, "r")
    for line in f:
        clusters.append(line)
    return clusters

def make_html (fname):
    imgs = os.listdir(imgpath)
    o = open (fname.split('.')[0] + ".html", "w")
    path = imgpath 
    print ("<doctype HTML><head><title>visualize clusters</title></head><body>", file= o)
    print ("<style> img { width: 75px; } </style>", file=o)
    print ("<p> download clusters file: <a href=\"" + fname + "\">"+ fname + " </a></p>", file=o)
    clusters = get_clusters(fname)
    nimgs = 0
    for i in clusters:
        images = i.split("\t") 
        nimgs = nimgs + len(images)
    print ("<p> Number of images: " ,nimgs , "</p>", file=o)

    count = 0 
    for i in clusters:
        images = i.split("\t") 
        if count >= 0: 
            print ("<p> Cluster id: ", count, file=o)
            print ("  Size ", len(images) , "</p>", file=o)

            for image in images:
                if image in imgs:
                    print ("<img title=\"", image, "\" src=\"", path + image.split('.')[0] + ".jpg", "\"/>", file=o)
                else:
                    print ("<img title=\"",  image, "not found", "\" src=\"", path + image.split('.')[0] + ".jpg", "\"/>", file=o)


        print ("<hr>", file=o)
        count = count + 1

    print ("</body></html>", file=o)
    o.close()
    return

def missing_html (fname):
    imgs = os.listdir(imgpath)
    o = open (fname.split('.')[0] + "_missing.html", "w")
    path = imgpath
    print ("<doctype HTML><head><title>visualize clusters</title></head><body>", file= o)
    print ("<style> img { width: 75px; max-height: 100px } </style>", file=o)
    clusters = get_clusters(fname)
    nimgs = 0
    for i in clusters:
        images = i.split("\t") 
        nimgs = nimgs + len(images)
    print ("<p> Number of images: " ,len(imgs) - nimgs , "</p>", file=o)

    count = 0 
    included = []
    for i in clusters:
        images = i.split("\t")
        for i in images:
            included.append(i.rstrip().split('.')[0] + ".jpg")
    missing = [img for img in imgs if img not in included]

    for image in missing:
        print ("<img title=\"",  image, "not found", "\" src=\"", path + image.split('.')[0] + ".jpg", "\"/>", file=o)

    print ("<hr>", file=o)
    print ("</body></html>", file=o)
    o.close()
    return


def main():
    fname = "clusters.tsv"
    make_html(fname)
    missing_html(fname)

if __name__=="__main__":
    main()



