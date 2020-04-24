main : main.o trapezoidalmap.o
	g++ -o main main.o trapezoidalmap.o
main.o : main.cpp
	g++ -c main.cpp -o main.o
trapeziodalmap.o : trapezoidalmap.cpp
	g++ -c trapezoidalmap.cpp -o trapezoidalmap.o

clean:
	rm -rf *.o
