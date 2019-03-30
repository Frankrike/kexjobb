OBJDIR = obj
SRCDIR = src
OUT = robotar.out
OUT_GEN = missiongen.out
CC = g++ -std=c++11
FLAGS = -Wall

_OBJS = main.o mission.o state.o situation.o algorithm.o pinkam1.o gui.o exhaustive.o
OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

_OBJS_GEN = mission.o missiongen.o
OBJS_GEN = $(patsubst %,$(OBJDIR)/%,$(_OBJS_GEN))

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(FLAGS) -MMD -c -o $@ $<

$(OUT): $(OBJS) $(OBJS_GEN)
	$(CC) $(OBJS) -o $@ -lsfml-graphics -lsfml-window -lsfml-system;
	$(CC) $(OBJS_GEN) -o $(OUT_GEN)



-include $(OBJDIR)/*.d
