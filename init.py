import os
import yaml
from shutil import copyfile


def main():
    params = yaml.load(open("./settings.yml"))

    print ("creating all directories that are needed but don't exist yet")
    # keypoints and matches directories
    if not os.path.exists(params["keypoints"]):
        print ("creating : " + params["keypoints"])
        os.makedirs(params["keypoints"])

    # output of update /update-date/meta-data and /update-date/results/
    if not os.path.exists(params["matches"]):
        print ("creating : " + params["matches"])
        os.makedirs(params["matches"])

    if not os.path.exists(params["outputdir"]):
        print ("creating : " + params["outputdir"])
        os.makedirs(params["outputdir"]) 

    if not os.path.exists(params["outputdir"] + "/meta-data/"):
        print ("creating : " + params["outputdir"] + "/meta-data/")
        os.makedirs(params["outputdir"] + "/meta-data/")

    if not os.path.exists(params["outputdir"] + "/results/"):
        print ("creating : " + params["outputdir"] + "/results/")
        os.makedirs(params["outputdir"] + "/results/")

    ofilepath = params["outputdir"] + "meta-data/" + "new-images.txt"


    print ("writing imgdir, keyptdir, matches, outputdir to bash variable")
    with open ("./1-create-index/vars.sh", "w") as f:
        print ("images=" + params["images"], file=f)
        print ("keypoints=" + params["keypoints"], file=f)
        print ("matches=" + params["matches"], file=f)
        print ("output=" + params["outputdir"], file=f)

    print ("copying settings.yml to outputdir/meta-data")
    copyfile("./settings.yml", params["outputdir"] + "/meta-data/settings.yml")

    print ("writing readme")
    with open (params["outputdir"] + "readme.txt", "w") as o:
        print ("Update performed on ", params["date"], file=o)

if __name__=="__main__":
    main()
