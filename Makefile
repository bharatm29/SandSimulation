CFLAGS = -Wall -Wextra -Iraylib  -lwayland-client -lwayland-cursor -lwayland-egl -lxkbcommon -lraylib -lGL -lm -lpthread -ldl -lrt
SRCDIR = src

all: main

main: $(SRCDIR)/main.cpp $(SRCDIR)/particle.cpp
	g++ -o main $(SRCDIR)/main.cpp $(CFLAGS)
	./main
