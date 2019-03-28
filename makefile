OBJDIR = obj
SRCDIR = src
OUT = robotar.out
CC = g++ -std=c++11
FLAGS = -Wall

_OBJS = main.o mission.o state.o situation.o algorithm.o pinkam1.o gui.o exhaustive.o
OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(FLAGS) -MMD -c -o $@ $<

$(OUT): $(OBJS)
	$(CC) $(OBJS) -o $@ -lsfml-graphics -lsfml-window -lsfml-system

-include $(OBJDIR)/*.d
