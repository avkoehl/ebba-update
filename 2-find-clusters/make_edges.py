import yaml

def parse( line ):
    line = line.rstrip()
    elements = line.split(',')
    
    # get source
    source = elements[0]

    # get targets and weights
    targets = []
    for element in elements[2:]:
        targets.append(element)

    return (source, targets)

def main():
    params = yaml.load(open("../settings.yml"))

    ifilepath = params["outputdir"] + "results/" + "adjacency-list.csv"
    ofilepath = "edges.txt"
    f = open (ifilepath, "r")
    y = open (ofilepath, "w")

    for line in f:
        source,targets = parse(line)
        for t in targets:
            exploded = t.split(' ')
            distance = exploded[1]
            edge = exploded[0]
            print (source, edge, distance, file=y)

if __name__ == "__main__":
    main()



