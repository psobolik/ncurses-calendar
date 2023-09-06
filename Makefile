CXXFLAGS=-Wall -std=c++11
CXXDBGFLAGS=-w -g3 -Og

calendar : calendar.cpp
	g++ calendar.cpp $(CXXFLAGS) -lncurses -o calendar

calendar.dbg : calendar.cpp
	g++ calendar.cpp $(CXXFLAGS) $(CXXDBFLAGS) -lncurses -o calendar.dbg

run : calendar
	./calendar
