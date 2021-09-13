CXX=g++
CXXFLAGS=-std=c++17 -O2 -Wall -Wfatal-errors -Wpedantic
LIBS=-lncurses
BIN=chomp
OBJS=chomp.o

chomp: $(OBJS)
	$(CXX) -o $(BIN) $(OBJS) $(LIBS)

.PHONY: clean objclean

clean:
	rm -f $(OBJS) $(BIN)

objclean:
	rm -f $(OBJS)

