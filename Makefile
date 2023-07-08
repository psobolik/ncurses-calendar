calendar : calendar.cpp
	g++ calendar.cpp -Wall -o calendar -lncurses

calendar.dbg : calendar.cpp
	g++ calendar.cpp -Wall -W -g3 -Og -lncurses -o calendar.dbg

run : calendar
	./calendar
