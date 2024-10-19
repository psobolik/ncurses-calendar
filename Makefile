CC=c++
CXXFLAGS=-Wall -std=c++11
CXXDBGFLAGS=-w -g3 -Og

calendar : calendar.cpp calendar.h
	$(CC) calendar.cpp $(CXXFLAGS) -lncurses -o calendar

calendar.dbg : calendar.cpp calendar.h
	$(CC) calendar.cpp $(CXXFLAGS) $(CXXDBFLAGS) -lncurses -o calendar.dbg

run : calendar
	./calendar
