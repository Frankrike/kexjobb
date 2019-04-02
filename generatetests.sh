make
./missiongen.out seed=0 w=10 h=10 mode=random density=0.2 robots=2 stations=2 orders=3 ordersize=3 items=2 > missions/rand0.txt
./missiongen.out seed=0 w=20 h=20 mode=random density=0.05 robots=5 stations=5 orders=30 ordersize=5 items=10 > missions/rand1.txt
./missiongen.out seed=0 mode=shelves shelvesX=3 shelvesY=2 margin=2 marginStations=3 robots=5 stations=5 orders=30 ordersize=5 > missions/shelves0.txt
./missiongen.out seed=0 mode=shelves2 shelvesX=3 shelvesY=2 margin=2 marginStations=3 robots=8 stations=8 orders=30 ordersize=5 > missions/shelves1.txt