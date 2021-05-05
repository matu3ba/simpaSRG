#!/usr/bin/env python3
from tabulate import tabulate
import pandas as pd
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import sys
import os
import natsort
matplotlib.use("pgf")
matplotlib.rcParams.update({
    "pgf.texsystem": "lualatex",
    'font.family': 'serif',
    'text.usetex': True,
    'pgf.rcfonts': False,
    #'figure.constrained_layout.use': True,
})
plt.rcParams['figure.figsize'] = 3.347115, 3.5 #good enough? 4.5,4.
#ax.set_ylabel('Speed[written Voxel/time]')
#wnew = targ_whalf # targ_whalf
#w, h = fig.get_size_inches() #wnew = w * zoom <=> zoom = wnew / w
#zoom = wnew / w
#hnew = h * zoom
#fig.set_size_inches(w=wnew,h=hnew)
#ax.set_title(r'Voxel writing speed $[\frac{\texttt{Voxel}}{\mu s}]$ by example and threads')
#print(matplotlib.get_backend())

SMALL_SIZE = 6
MEDIUM_SIZE = 8
BIGGER_SIZE = 10
plt.rc('font', size=SMALL_SIZE)          # controls default text sizes
plt.rc('axes', titlesize=MEDIUM_SIZE)     # fontsize of the axes title
plt.rc('axes', labelsize=MEDIUM_SIZE)    # fontsize of the x and y labels
plt.rc('xtick', labelsize=SMALL_SIZE)    # fontsize of the tick labels
plt.rc('ytick', labelsize=SMALL_SIZE)    # fontsize of the tick labels
plt.rc('legend', fontsize=MEDIUM_SIZE)    # legend fontsize
plt.rc('figure', titlesize=MEDIUM_SIZE)  # fontsize of the figure title

#fig.set_size_inches(w=3.347115)
#fig.set_size_inches(w=6.69423)
#6.69423in => 3.347115
#8.49875cm and 16.9975
targ_whalf = 3.347115
targ_wfull = 6.69423
#compute h as it is needed by aspect ratio/zoom

#credit https://timodenk.com/blog/exporting-matplotlib-plots-to-latex/

def autolabel(rects, rot, roti, posround):
    """Attach a text label above each bar in *rects*, displaying its height."""
    self_rot = rot
    for i in range(0,len(rects)):
        height = round( rects[i].get_height(), posround )
        if i in roti:
            self_rot = 0
        ax.annotate('{}'.format(height),
                    xy=(rects[i].get_x() + rects[i].get_width() / 2, height),
                    xytext=(0, 2),  # 3 points vertical offset
                    textcoords="offset points",
                    ha='center', va='bottom', rotation=self_rot, fontsize=SMALL_SIZE)
        if i in roti:
            self_rot = rot

filein='avgs'
markedVoxels='../../build/markedVoxels.csv'
algos = ['simpleSRG', 'simpleSRGcib', 'simpaSRG_NAIVE', 'simpaSRG_NAIVEcib']
examples = [ 'BoneMaskHigh', 'Head_of_mouse', 'Maus_mit_Draht', 'Skeleton', 'Skull_of_mouse', 'CubeFull', 'SphereFull', 'Helix_D1', 'Helix_D2', 'Helix_D3', 'Helix_D4', 'Helix_D5' ]
real = [ 'BoneMaskHigh', 'Head_of_mouse', 'Maus_mit_Draht', 'Skeleton', 'Skull_of_mouse' ]
arti = ['CubeFull', 'SphereFull', 'Helix_D1', 'Helix_D2', 'Helix_D3', 'Helix_D4', 'Helix_D5' ]
artiCubeSphere = ['Cube8e6','Cube64e6','Cube512e6','Sphere8e6','Sphere64e6','Sphere512e6']
artiHelix = ['Helix_D1_8e6','Helix_D1_64e6','Helix_D1_512e6','Helix_D2_8e6','Helix_D2_64e6','Helix_D2_512e6','Helix_D3_8e6','Helix_D3_64e6','Helix_D3_512e6','Helix_D4_8e6','Helix_D4_64e6','Helix_D4_512e6','Helix_D5_8e6','Helix_D5_64e6','Helix_D5_512e6']
#voxspeedoffset = len(real)
voxspeedsimple =    [0] * (len(real)-1)
voxspeed1threads =  [0] * (len(real)-1)
voxspeed24threads = [0] * (len(real)-1)
timesimple =        [0] * (len(real)-1)
time1threads =      [0] * (len(real)-1)
time24threads =     [0] * (len(real)-1)
#-1 to ignore skull of mouse
x = np.arange(0,2*(len(real)-1),2)  # the label locations
sizes = [ 8000000, 64000000, 512000000 ]
sizecolors = [ 'b', 'g', 'r' ]
cols = ['algo', 'example', 'size', 'threads','avgtime']
colmvs = ['sizex', 'sizey', 'sizez', 'example', 'size', 'markedVoxels']
df = pd.read_csv(filein, header=None)
df.columns = cols
rows, columns = df.shape
mvs = pd.read_csv(markedVoxels, header=None)
mvsrows, mvscolumns = mvs.shape
mvs.columns = colmvs
#print(mvsrows, mvscolumns)

threads = range(1,25)
speedup = [0] * 24 #list of zeros
timepar = [0] * 24 #list of times
markedVoxels_bytimepar = [0] * 24 #list of times
pspeed = []
ptime = []
pspeed_zoom = []
ptime_zoom = []
markVox_ptime = []
# -1 to ignore skull_of_mouse as too complex body
for i in range(0,len(real)-1):
    pspeed = []
    ptime = []
    pspeed_zoom = []
    ptime_zoom = []
    markVox_ptime = []
    realbase = df[ (df['algo'] == 'simpleSRG') & (df['example'] == real[i]) & (df['threads'] == 1) ]
    tmprows, tmpcols = realbase.shape
    assert (tmprows == 1)
    tmpsize = realbase['size'].iloc[0]
    realsa = df[ (df['algo'] == 'simpaSRG_NAIVE') & (df['example'] == real[i]) ]
    tmpstr = (real[i] + "_simpleSRG1")
    realvox = mvs[ (mvs['example'] == tmpstr) & (mvs['size'] == tmpsize) ]
    #print(realvox)
    lbl= "size:" + str( round(realbase.iloc[0,2] / 1048576, 2) ) + "MB" #converting from B to MB => divide by 1024**2=1048576
    #print(realbase)
    #print(realsa)
    for j in range(0,24):
        assert(realsa.iloc[j,3] == threads[j]), f"is {realsa.iloc[0,3]},should be {threads[j]}"
        speedup[j] = realbase.iloc[0,4] / realsa.iloc[j,4]
        timepar[j] = realsa.iloc[j,4] / 1e6
        markedVoxels_bytimepar[j] = realvox['markedVoxels'].iloc[0] / timepar[j]
    voxspeedsimple[i] = realvox['markedVoxels'].iloc[0] / realbase.iloc[0,4]
    voxspeed1threads[i] = realvox['markedVoxels'].iloc[0] / (timepar[0]*1e6)
    voxspeed24threads[i] = realvox['markedVoxels'].iloc[0] / (timepar[23]*1e6)
    timesimple[i] = realbase.iloc[0,4] / 1e6
    time1threads[i] = (timepar[0]*1e6) / 1e6
    time24threads[i] = (timepar[23]*1e6) / 1e6
    print('baseline time', realvox['example'].iloc[0],realvox['size'].iloc[0], realbase.iloc[0,4]/1e6)
    print('baseline writing speed', realvox['example'].iloc[0],realvox['size'].iloc[0], voxspeedsimple[i])
    #print(realvox['size'].iloc[0], realvox['markedVoxels'].iloc[0])
    #print('time8thread', realvox['example'].iloc[0],realvox['size'].iloc[0], timepar[7])
    #print('speedup8thread', realvox['example'].iloc[0],realvox['size'].iloc[0], speedup[7])
    print('time24thread', realvox['example'].iloc[0],realvox['size'].iloc[0], timepar[23])
    print('24threads writing speed', realvox['example'].iloc[0],realvox['size'].iloc[0], voxspeed24threads[i])
    #print('speedup24thread', realvox['example'].iloc[0],realvox['size'].iloc[0], speedup[23])
    #print (speedup)
    plt.figure(1)
    psp = plt.scatter(threads,speedup, label=lbl) #s=20,c='red',marker='^'
    pspeed.append(psp)
    plt.figure(2)
    pt = plt.scatter(threads,timepar, c='b', label=lbl) #s=20,c='red',marker='^'
    ptime.append(pt)
    plt.axhline(y=realbase.iloc[0,4] / 1e6, c='r', linestyle='-')
    plt.figure(1)
    plt.axhline(y=1, color='r', linestyle='-')
    plt.legend(handles=pspeed)
    plt.annotate('threads', xy=(0.82, -0.035), xycoords='axes fraction', fontsize=MEDIUM_SIZE)
    plt.annotate('speedup', xy=(0.0, 1.01), xycoords='axes fraction', fontsize=MEDIUM_SIZE)
    plt.savefig('speedup_' + str(real[i]) + '_' + str(realbase.iloc[0,2]) + '.pgf', bbox_inches='tight', pad_inches = 0)
    plt.figure(2)
    plt.legend(handles=ptime)
    plt.annotate('threads', xy=(0.82, -0.035), xycoords='axes fraction', fontsize=MEDIUM_SIZE)
    plt.annotate(r'time[$s$]', xy=(0.0, 1.01), xycoords='axes fraction', fontsize=MEDIUM_SIZE)
    plt.savefig('times_' + str(real[i]) + '_' + str(realbase.iloc[0,2]) + '.pgf', bbox_inches='tight', pad_inches = 0)
    plt.close(1)
    plt.close(2)
plt.figure(5)
width = 0.6  # the width of the bars 0.66 is exact border?
fig, ax = plt.subplots(constrained_layout=False)
rects1 = ax.bar(x - width, voxspeedsimple, width, label='SRG, 1 thr')
rects2 = ax.bar(x         ,voxspeed1threads,width, label='simpaSRG, 1 thr')
rects3 = ax.bar(x + width,voxspeed24threads,width, label='simpaSRG, 24 thr')
#ax.set_title(r'Voxel writing speed $[\frac{\texttt{Voxel}}{\mu s}]$ by example and threads')
ax.set_xticks(x)
#-1 to ignore skull of mouse
ax.set_xticklabels(real[:-1],rotation=45, fontsize=SMALL_SIZE)
ax.legend()
autolabel(rects1, 90, {}, 1)
autolabel(rects2, 90, {}, 1)
autolabel(rects3, 90, {0, 1}, 1)
plt.ylabel(r'$\frac{\texttt{Voxel}}{\mu s}$', rotation=0)
ax.yaxis.set_label_coords(-0.055,0.96)
#plt.tight_layout()
plt.savefig('markVoxbytimeREALexamples.pgf', bbox_inches='tight', pad_inches = 0)
plt.close(5)
plt.figure(5)
fig, ax = plt.subplots(constrained_layout=False)
#print(timesimple)
rects1 = ax.bar(x - width, timesimple, width, label='SRG, 1 thr')
rects2 = ax.bar(x         ,time1threads,width, label='simpaSRG, 1 thr')
rects3 = ax.bar(x + width,time24threads,width, label='simpaSRG, 24 thr')
ax.set_xticks(x)
#-1 to ignore skull of mouse
ax.set_xticklabels(real[:-1],rotation=45, fontsize=SMALL_SIZE)
ax.legend()
autolabel(rects1, 90, {}, 3)
autolabel(rects2, 90, {0,1}, 2)
autolabel(rects3, 90, {}, 2)
plt.annotate(r'time[$s$]', xy=(0.0, 1.01), xycoords='axes fraction', fontsize=MEDIUM_SIZE)
plt.savefig('timebysizeREALexamples.pgf', bbox_inches='tight', pad_inches = 0)
plt.close(5)

#print ('real speedsimple', voxspeedsimple)
lenvoxspeedarti = (3*len(arti))
voxspeedsimple = [0] * lenvoxspeedarti
voxspeed1threads = [0] * lenvoxspeedarti
voxspeed24threads = [0] * lenvoxspeedarti
x = np.arange(0,2*lenvoxspeedarti,2)
cnt = 0
    #plt.show()
for i in range(0,len(arti)):
    #print('size: ' + str(size))
    pspeed = []
    ptime = []
    pspeed_zoom = []
    ptime_zoom = []
    markVox_ptime = []
    basetime = 0
    for isize in range(0,len(sizes)):
        lbl = "size:" + str( round(sizes[isize] / 1048576, 2) ) + "MB" #scaling from B to MB => divide by 1024**2 = 1048576
        artibase = df[ (df['algo'] == 'simpleSRG') & (df['example'] == arti[i]) & (df['threads'] == 1) & (df['size'] == sizes[isize]) ]
        artisa = df[ (df['algo'] == 'simpaSRG_NAIVE') & (df['example'] == arti[i]) & (df['size'] == sizes[isize]) ]
        tmpstr = (arti[i] + "_simpleSRG1")
        artivox = mvs[ (mvs['example'] == tmpstr) & (mvs['size'] == sizes[isize]) ]
        #print('artivox :', artivox)
        #print(artibase)
        #print(artisa)
        #tmpartibase = artibase[artibase('size')==size]
        #tmpartisa = artisa[artisa('size')==size]
        for j in range(0,24):
            assert(artisa.iloc[j,3] == threads[j]), f"is {tmpartisa.iloc[0,3]},should be {threads[j]}"
            speedup[j] = artibase.iloc[0,4] / artisa.iloc[j,4]
            timepar[j] = artisa.iloc[j,4] / 1e6
            markedVoxels_bytimepar[j] = artivox['markedVoxels'].iloc[0] / timepar[j]
        #print('base:', artibase.iloc[0,4])
        #print('1th:', (timepar[0]*1e6) )
        #print('24th:', (timepar[23]*1e6) )
        #print('3*i+isize:', 3*i+isize)
        #print(sizes[isize])
        voxspeedsimple[3*i+isize] = artivox['markedVoxels'].iloc[0] / artibase.iloc[0,4]
        voxspeed1threads[3*i+isize] = artivox['markedVoxels'].iloc[0] / (timepar[0]*1e6)
        voxspeed24threads[3*i+isize] = artivox['markedVoxels'].iloc[0] / (timepar[23]*1e6)
        print('baseline time', artivox['example'].iloc[0],artivox['size'].iloc[0], artibase.iloc[0,4]/1e6)
        print('baseline writing speed', artivox['example'].iloc[0],artivox['size'].iloc[0], voxspeedsimple[3*i+isize])
        #print(artivox['size'].iloc[0], artivox['markedVoxels'].iloc[0])
        #print('time8thread', artivox['example'].iloc[0],artivox['size'].iloc[0], timepar[7])
        #print('speedup8thread', artivox['example'].iloc[0],artivox['size'].iloc[0], speedup[7])
        print('time24thread', artivox['example'].iloc[0],artivox['size'].iloc[0], timepar[23])
        print('24threads writing speed', artivox['example'].iloc[0],artivox['size'].iloc[0], voxspeed24threads[3*i+isize])
        #print('speedup24thread', artivox['example'].iloc[0],artivox['size'].iloc[0], speedup[23])

        plt.figure(1) #relative speedup
        psp = plt.scatter(threads,speedup, c=sizecolors[isize], label=lbl) #s=20,c='red',marker='^'
        pspeed.append(psp)
        #plt.title('Example ' + arti[i] + ': Speedup per Thread')
        #plt.ylabel(r'$\frac{\texttt{Voxel}}{\mu s}$', rotation=0)
        #ax.yaxis.set_label_coords(-0.055,0.96)
        plt.figure(2) #absolute time
        pt = plt.scatter(threads,timepar, c=sizecolors[isize], label=lbl) #s=20,c='red',marker='^'
        ptime.append(pt)
        plt.axhline(y=artibase.iloc[0,4] / 1e6, c=sizecolors[isize], linestyle='-')
        #plt.title('Example ' + arti[i] + ': Parallel Time per Thread')
        #plt.ylabel(r'$\frac{\texttt{Voxel}}{\mu s}$', rotation=0)
        #ax.yaxis.set_label_coords(-0.055,0.96)
    cnt = cnt + len(sizes)
    plt.figure(1)
    plt.axhline(y=1, color='r', linestyle='-')
    plt.legend(handles=pspeed)
    #plt.xlabel('threads', x=0.90, y=-0.05)
    #plt.ylabel('speedup', x=1.20, y=1.02, rotation=0)
    plt.annotate('threads', xy=(0.82, -0.035), xycoords='axes fraction', fontsize=MEDIUM_SIZE)
    plt.annotate('speedup', xy=(0.0, 1.01), xycoords='axes fraction', fontsize=MEDIUM_SIZE)
    #plt.tight_layout()
    plt.savefig('speedup_' + str(arti[i]) + '_' + str(artibase.iloc[0,2]) + '.pgf' , bbox_inches='tight', pad_inches = 0)
    plt.figure(2)
    plt.legend(handles=ptime)
    #plt.xlabel('threads', x=0.90, y=-0.05)
    #plt.ylabel(r'time[$s$]', x=1.20, y=1.02, rotation=0)
    plt.annotate('threads', xy=(0.82, -0.035), xycoords='axes fraction', fontsize=MEDIUM_SIZE)
    plt.annotate(r'time[$s$]', xy=(0.0, 1.01), xycoords='axes fraction', fontsize=MEDIUM_SIZE)
    plt.savefig('times_' + str(arti[i]) + '_' + str(artibase.iloc[0,2]) + '.pgf' , bbox_inches='tight', pad_inches = 0)
    plt.close(1)
    plt.close(2)
    #plt.show()
cnt = 0
#for i in range(0,len(voxspeedsimple)):
#    if i % 2 == 0:
#        print('i:', i , voxspeedsimple[i])
#        print('i:', i , voxspeed1threads[i])
#        print('i:', i , voxspeed24threads[i])
#print('arti speedsimple', voxspeedsimple)

# create separate plot for Cube and Sphere
voxspeedsimpleSMALLER = voxspeedsimple[0:6]
voxspeed1threadsSMALLER = voxspeed1threads[0:6]
voxspeed24thradsSMALLER = voxspeed24threads[0:6]
#x = np.arange(len(voxspeedsimpleSMALLER))
x = np.arange(0,2*len(voxspeedsimpleSMALLER),2)

plt.figure(5)
#width = 0.5  # the width of the bars
fig, ax = plt.subplots(constrained_layout=False)
rects1 = ax.bar(x - width, voxspeedsimpleSMALLER, width, label='SRG, 1 thr')
rects2 = ax.bar(x         ,voxspeed1threadsSMALLER,width, label='simpaSRG, 1 thr')
rects3 = ax.bar(x + width,voxspeed24thradsSMALLER,width, label='simpaSRG, 24 thr')
wnew = targ_whalf # targ_whalf/full
w, h = fig.get_size_inches() #wnew = w * zoom <=> zoom = wnew / w
zoom = wnew / w
hnew = h * zoom
fig.set_size_inches(w=wnew,h=hnew)
#ax.set_title(r'Voxel writing speed by example and threads')
ax.set_xticks(x)
ax.set_xticklabels(artiCubeSphere,rotation=45, fontsize=SMALL_SIZE)
ax.legend()
autolabel(rects1, 90, {0,3}, 1) #Cube,Sphere
autolabel(rects2, 90, {}, 1)
autolabel(rects3, 90, {1,2,4,5}, 1)
plt.ylabel(r'$\frac{\texttt{Voxel}}{\mu s}$', rotation=0)
ax.yaxis.set_label_coords(-0.055,0.96)
#plt.show()
#plt.tight_layout()
plt.savefig('markVoxbytimeCubeSphere.pgf', bbox_inches='tight', pad_inches = 0)
plt.close(5)

# create separate plot for Helix examples
voxspeedsimpleSMALLER = voxspeedsimple[6:]
voxspeed1threadsSMALLER = voxspeed1threads[6:]
voxspeed24thradsSMALLER = voxspeed24threads[6:]
x = np.arange(0,2*len(voxspeedsimpleSMALLER),2)

plt.figure(5)
#width = 0.5  # the width of the bars
fig, ax = plt.subplots(constrained_layout=False)
rects1 = ax.bar(x - width, voxspeedsimpleSMALLER, width, label='SRG, 1 thr')
rects2 = ax.bar(x         ,voxspeed1threadsSMALLER,width, label='simpaSRG, 1 thr')
rects3 = ax.bar(x + width,voxspeed24thradsSMALLER,width, label='simpaSRG, 24 thr')
ax.set_xticks(x)
ax.set_xticklabels(artiHelix,rotation=90, fontsize=SMALL_SIZE) #fontsize,labelsize,titlesize
ax.legend()
autolabel(rects1, 90, {12,13}, 1) #Helix
autolabel(rects2, 90, {}, 3)
autolabel(rects3, 90, {}, 3)
plt.ylabel(r'$\frac{\texttt{Voxel}}{\mu s}$', rotation=0)
ax.yaxis.set_label_coords(-0.055,0.96)
#plt.tight_layout()
plt.savefig('markVoxbytimeHelix.pgf', bbox_inches='tight', pad_inches = 0)
plt.close(5)

#print (voxspeedsimple)
#print (voxspeed1threads)
#print (voxspeed24threads)
