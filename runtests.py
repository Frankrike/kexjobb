import subprocess
import os
import time

DIR = "genmissions"
NUMCASES = 10
NUMSMALLCASES = 3
FNULL = open(os.devnull, 'w')
subprocess.call("make", shell=True)
subprocess.call("mkdir {}".format(DIR), shell=True)
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
  outputHere = ""
  outputHere += name + "\n"
  for case in cases:
    case.generate()
  for algo in algos:
    outputHere += "{}\t".format(algo.name)
  outputHere += "\n"
  for algo in algos:
    tot = 0
    for case in cases:
      tot += algo.run(case)
    outputHere += "{:.2f}\t".format(tot/len(cases))
  outputHere += "\n"
  print(outputHere)
  output += outputHere

algorithms = [
  Algorithm("pinkam1", "./robotar.out showgui=false algorithm=pinkam1"),
  Algorithm("pinkam2", "./robotar.out showgui=false algorithm=pinkam2"),
  Algorithm("vhs", "./robotar.out showgui=false algorithm=vhs")
]
exhaustive = Algorithm("exhaustive", "./robotar.out showgui=false algorithm=exhaustive")

shelfCases = [
  Testcase("shelves{}".format(i), './missiongen.out seed={} mode=shelves shelvesX=3 shelvesY=2 shelfLength=4 margin=2 marginStations=3 robots=8 stations=8 orders=30 ordersize=5'.format(i))
  for i in range(NUMCASES)
]

shelfRingCases = [
  Testcase("shelves_ring{}".format(i), './missiongen.out seed={} mode=shelves2 shelvesX=3 shelvesY=2 shelfLength=4 margin=2 marginStations=3 robots=8 stations=8 orders=30 ordersize=5'.format(i))
  for i in range(NUMCASES)
]

smallShelfCases = [
  Testcase("shelves_small{}".format(i), './missiongen.out seed={} mode=shelves shelvesX=1 shelvesY=1 shelfLength=2 margin=1 marginStations=1 robots=2 stations=2 orders=3 ordersize=3'.format(i))
  for i in range(NUMSMALLCASES)
]

smallShelfRingCases = [
  Testcase("shelves_small{}".format(i), './missiongen.out seed={} mode=shelves2 shelvesX=1 shelvesY=1 shelfLength=2 margin=1 marginStations=1 robots=2 stations=2 orders=3 ordersize=3'.format(i))
  for i in range(NUMSMALLCASES)
]

experiment("shelves", algorithms, shelfCases)
experiment("shelves, ring of stations", algorithms, shelfRingCases)
experiment("small shelves", algorithms + [exhaustive], smallShelfCases)
experiment("small shelves, ring of stations", algorithms + [exhaustive], smallShelfRingCases)

print("FINAL OUTPUT")
print(output)