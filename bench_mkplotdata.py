#!/usr/bin/env python3
from tabulate import tabulate
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import sys
import os
import natsort 

def startingwith(fname, real, arti, app_real, app_arti):
    l_real = len(real)
    l_arti = len(arti)
    for i in range(0,len(real)):
        if fname.startswith(real[i]):
            #app_real.append(fname[len(real[i]):])
            print(fname[len(real[i]):])
            return
    for i in range(0,len(arti)):
        if fname.startswith(arti[i]):
            # + "_"
            #app_arti.append(fname[len(arti[i]):])
            print(fname[len(arti[i]):])
            return


datain='bench_results'
dataout='plots'
os.chdir(datain)
#col=['time']
#thr=['threads']
files = [f for f in os.listdir('.') if os.path.isfile(f)]
sortedfiles = natsort.natsorted(files)
algos = ['simpleSRG_', 'simpleSRGcib_', 'simpaSRG_NAIVE_', 'simpaSRG_NAIVEcib_']
examples = [ 'BoneMaskHigh', 'Head_of_mouse', 'Maus_mit_Draht', 'Skeleton', 'Skull_of_mouse', 'CubeFull', 'SphereFull', 'Helix_D1', 'Helix_D2', 'Helix_D3', 'Helix_D4', 'Helix_D5' ]
cols = ['algo', 'example', 'size', 'threads','avgtime']
indices=range(0,len(sortedfiles))
newDataframe = pd.DataFrame(index=indices,columns=cols) # result csv
for fileindex in indices:
    tmp = ""
    algo = ""
    example = ""
    for algo in algos:
        if sortedfiles[fileindex].startswith(algo):
            newDataframe.iloc[fileindex,0] = sortedfiles[fileindex][:len(algo)-1]
            tmp = sortedfiles[fileindex][len(algo):]
            break
    for example in examples:
        if tmp.startswith(example):
            newDataframe.iloc[fileindex,1] = tmp[:len(example)]
            tmp = tmp[len(example)+1:]
            break
    #print(tmp)
    #sys.exit()
    res = tmp.split("_")
    #print(res)
    assert(len(res) == 2), f"tmp: {tmp}, len = {len(res)}, file: {sortedfiles[fileindex]}"
    newDataframe.iloc[fileindex,2] = res[0]
    newDataframe.iloc[fileindex,3] = res[1]
    #print(newDataframe)
    #sys.exit()
    df = pd.read_csv(sortedfiles[fileindex], header=None) #data for calculating mean
    rows, columns = df.shape
    assert(rows >= 1), f"rows = {rows},cols={columns},file = {sortedfiles[fileindex]}"
    mean = 0
    for row in range(0,rows):
        assert(df.iloc[row,0] == algo[:len(algo)-1]), f"val[row,0] = {df.iloc[row,0]}, algo={algo},file = {sortedfiles[fileindex]}"
        assert(df.iloc[row,1] == example), f"val[row,1] = {df.iloc[row,1]}, example={example},file = {sortedfiles[fileindex]}"
        assert(df.iloc[row,2] == np.int64(res[0])), f"val[row,2] = {df.iloc[row,2]},size={res[0]},file = {sortedfiles[fileindex]}"
        #print(type(df.iloc[row,3]))
        #print(type(res[1]))
        assert(df.iloc[row,3] == np.int64(res[1])), f"val[row,3] = {df.iloc[row,3]},threads={res[1]},file = {sortedfiles[fileindex]}"
        mean = mean + np.int64(df.iloc[row,4])
        #print(df.iloc[row,4])
    #floatmean = mean / rows
    intmean = mean //rows
    print(intmean)
    #assert(floatmean > 0)
    assert(intmean > 0)
    newDataframe.iloc[fileindex,4] = intmean
    print(newDataframe.iloc[fileindex,:])
#sys.exit()
newDataframe.to_csv("../" + dataout + "/avgs", header=None, index=False)
