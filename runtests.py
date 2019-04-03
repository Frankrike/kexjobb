import subprocess
import os

DIR = "genmissions"
NUMCASES = 10
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
    print("Running " + self.name + " on " + testcase.name)
    return testcase.run(self.command)

def experiment(name, algos, cases):
  global output
  output += name + "\n"
  for case in cases:
    case.generate()
  for algo in algos:
    output += "{}\t".format(algo.name)
  output += "\n"
  for algo in algos:
    tot = 0
    for case in cases:
      tot += algo.run(case)
    output += "{:.2f}\t".format(tot/len(cases))
  output += "\n"



algorithms = [
  Algorithm("pinkam1", "./robotar.out showgui=false algorithm=pinkam1"),
  Algorithm("pinkam2", "./robotar.out showgui=false algorithm=pinkam2"),
  Algorithm("vhs", "./robotar.out showgui=false algorithm=vhs")
]

shelfCases = [
  Testcase("shelves{}".format(i), './missiongen.out seed={} mode=shelves shelvesX=3 shelvesY=2 margin=2 marginStations=3 robots=8 stations=8 orders=30'.format(i))
  for i in range(NUMCASES)
]

shelfRingCases = [
  Testcase("shelves_ring{}".format(i), './missiongen.out seed={} mode=shelves2 shelvesX=3 shelvesY=2 margin=2 marginStations=3 robots=8 stations=8 orders=30'.format(i))
  for i in range(NUMCASES)
]

experiment("normal shelves", algorithms, shelfCases)
experiment("normal shelves, ring of stations", algorithms, shelfRingCases)

print(output)