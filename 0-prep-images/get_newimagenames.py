import os
import yaml
from glob import glob

def main():
    params = yaml.load(open("../settings.yml"))

    ofilepath = params["outputdir"] + "meta-data/" + "new-images.txt"
    PATH = params["newimages"]
    OPATH = params["images"]
    ofile = open (ofilepath, "w")

    tifs = [y for x in os.walk(PATH) for y in glob(os.path.join(x[0], '*.tif'))]
    jpgs = [y for x in os.walk(PATH) for y in glob(os.path.join(x[0], '*.jpg'))]
    result = tifs + jpgs 

    for r in result:
        print (r, file=ofile)
        fname = r.split('/')[-1]
        os.rename(r, OPATH + fname)

if __name__=="__main__":
    main()

