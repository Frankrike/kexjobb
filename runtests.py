import subprocess
import os
import time
import numpy as np
import matplotlib.pyplot as plt

DIR = "genmissions"
PLOTDIR = "plots"
NUMCASES = 10
NUMSMALLCASES = 5
FNULL = open(os.devnull, 'w')
subprocess.call("make", shell=True)
subprocess.call("mkdir {}".format(DIR), shell=True)
subprocess.call("mkdir {}".format(PLOTDIR), shell=True)
output = ""

class Testcase:
  def __init__(self, name, command):
    self.name = name
    self.command = command

  def generate(self):
    print("Generating case " + self.name)
    directory = "{}/{}.txt".format(DIR, self.name)
    subprocess.call(self.command + " > {}".format(directory), shell=True)

  def run(self, command):
    directory = "{}/{}.txt".format(DIR, self.name)
    cmd = command + " < {} > tmp".format(directory)
    subprocess.call(cmd, stderr=FNULL, shell=True)
    self.result = int(subprocess.getoutput("cat tmp"))
    subprocess.call("rm tmp", shell=True)
    return self.result

class Algorithm:
  def __init__(self, name, command):
    self.name = name
    self.command = command

  def run(self, testcase):
    print("Running " + self.name + " on " + testcase.name + "... ", end='', flush=True)
    start = time.time()
    result = testcase.run(self.command)
    print("done ({:.2f}s)".format(time.time()-start))
    return result

def experiment(name, algos, cases):
  global output
  percentiles = (0, 25, 50, 75, 100)
  for case in cases:
    case.generate()

  results = []
  labels = []
  for algo in algos:
    results.append([algo.run(case) for case in cases])
    labels.append(algo.name)

  outputHere = ""
  outputHere += name + "\n%-tile\t"
  for algo in algos:
    outputHere += "{}\t".format(algo.name)
  outputHere += "\n"

  for p in percentiles:
    outputHere += "{}\t".format(p)
    for result in results:
      outputHere += "{:.2f}\t".format(np.percentile(np.array(result), p))
    outputHere += "\n"
  print(outputHere)
  output += outputHere

  makePlot(name, results, labels)

def makePlot(name, data, labels):
  fileName = "_".join([s.strip(",") for s in name.split()]) + ".png"
  fig, ax = plt.subplots()
  ax.set_title(name)
  ax.boxplot(data, labels=labels)
  #ax.set_ylim(bottom=0)
  plt.savefig(PLOTDIR + "/" + fileName)

algorithms = [
  Algorithm("pinkam1", "./robotar.out showgui=false algorithm=pinkam1"),
  Algorithm("pinkam2", "./robotar.out showgui=false algorithm=pinkam2"),
  Algorithm("vhs", "./robotar.out showgui=false algorithm=vhs")
]
algorithmsAllowCol = [
  Algorithm("pinkam1", "./robotar.out showgui=false allowcollisions=true algorithm=pinkam1"),
  Algorithm("pinkam2", "./robotar.out showgui=false allowcollisions=true algorithm=pinkam2"),
  Algorithm("vhs", "./robotar.out showgui=false allowcollisions=true algorithm=vhs")
]
algorithmsMoveTowards2 = [
  Algorithm("pinkam1", "./robotar.out showgui=false moveversion=2 algorithm=pinkam1"),
  Algorithm("pinkam2", "./robotar.out showgui=false moveversion=2 algorithm=pinkam2"),
  Algorithm("vhs", "./robotar.out showgui=false moveversion=2 algorithm=vhs")
]
exhaustive = Algorithm("exhaustive", "./robotar.out showgui=false algorithm=exhaustive")

shelfCases = [
  Testcase("shelves{}".format(i), './missiongen.out seed={} mode=shelves shelvesX=3 shelvesY=2 shelfLength=4 margin=2 marginStations=3 robots=8 stations=8 orders=100 ordersize=5'.format(i))
  for i in range(NUMCASES)
]

shelfRingCases = [
  Testcase("shelves_ring{}".format(i), './missiongen.out seed={} mode=shelves2 shelvesX=3 shelvesY=2 shelfLength=4 margin=2 marginStations=3 robots=8 stations=8 orders=100 ordersize=5'.format(i))
  for i in range(NUMCASES)
]

smallShelfCases = [
  Testcase("shelves_small{}".format(i), './missiongen.out seed={} mode=shelves shelvesX=1 shelvesY=1 shelfLength=2 margin=1 marginStations=1 robots=2 stations=2 orders=3 ordersize=3'.format(i))
  for i in range(NUMSMALLCASES)
]

smallShelfRingCases = [
  Testcase("shelves_small_ring{}".format(i), './missiongen.out seed={} mode=shelves2 shelvesX=1 shelvesY=1 shelfLength=2 margin=1 marginStations=1 robots=2 stations=2 orders=3 ordersize=3'.format(i))
  for i in range(NUMSMALLCASES)
]

experiment("shelves", algorithms, shelfCases)
experiment("shelves, ring of stations", algorithms, shelfRingCases)

experiment("shelves, allow collisions", algorithmsAllowCol, shelfCases)
experiment("shelves, ring of stations, allow collisions", algorithmsAllowCol, shelfRingCases)

experiment("shelves, moveTowards2", algorithmsMoveTowards2, shelfCases)
experiment("shelves, ring of stations, moveTowards2", algorithmsMoveTowards2, shelfRingCases)

experiment("small shelves", algorithms + [exhaustive], smallShelfCases)
experiment("small shelves, ring of stations", algorithms + [exhaustive], smallShelfRingCases)

experiment("small shelves, allow collisions", algorithmsAllowCol + [exhaustive], smallShelfCases)
experiment("small shelves, ring of stations, allow collisions", algorithmsAllowCol + [exhaustive], smallShelfRingCases)

experiment("small shelves, moveTowards2", algorithmsMoveTowards2 + [exhaustive], smallShelfCases)
experiment("small shelves, ring of stations, moveTowards2", algorithmsMoveTowards2 + [exhaustive], smallShelfRingCases)

print("FINAL OUTPUT")
print(output)