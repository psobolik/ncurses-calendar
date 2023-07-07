calendar : calendar.cpp
	g++ calendar.cpp -Wall -o calendar -lncurses

run : calendar
	./calendar
