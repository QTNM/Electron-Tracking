''' ROOT file converter for Cd-109 simulation outputs '''
import numpy as np
import argparse
from pathlib import Path
import ROOT as root

def convert(fname):
    ''' convert ROOT file into compressed text file, a .txt.gz'''
    filename = Path(fname)
    basetuple = filename.name.partition('.') # retrieve the file name partitions
    outfname = basetuple[0] # take only the name base

    # work on root file
    ff = root.TFile(fname, "READ")
    tree = ff.Get("Score")
    store = []
    for ev in tree:
        row = (ev.EventID,ev.TrackID,ev.PDG,ev.Kine,ev.Px,ev.Py,ev.Pz,ev.Posx,ev.Posy,ev.Posz)
        store.append(row) # store in memory
    arr = np.array(store) # list of tuples into array table, all as type float
    ff.Close()

    # now persist on disk
    np.savetxt(outfname+".csv.gz", arr, delimiter=',', header='evID, trackID, PDG, KE, Px, Py, Pz, posx, posy, posz')
    print('stored array with dim: ',arr.shape)


parser = argparse.ArgumentParser()
parser.add_argument("-f", "--file", help="input file name")
arg = parser.parse_args()
ffn = arg.file
if (ffn is not None):
    convert(ffn)
else:
    print('error: missing file name on command line')

