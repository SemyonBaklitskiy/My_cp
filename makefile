all: main.o functions.o
	g++ -Wall -Wextra main.o functions.o -o cp

functions.o: functions.cpp functions.h
	g++ -Wall -Wextra -c functions.cpp -o functions.o

main.o: main.cpp
	g++ -Wall -Wextra -c main.cpp -o main.o

clean:
	rm main.o functions.o main
