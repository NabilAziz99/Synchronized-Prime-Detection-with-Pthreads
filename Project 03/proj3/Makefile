
output: proj3.o buffer.o
	g++ -pthread -o proj3 proj3.cpp buffer.h
	rm *.o
	rm buffer.h.gch


proj3.o: proj3.cpp
	g++ -c proj3.cpp

buffer.o: buffer.h
	g++ -c buffer.h

clean:
	rm *.o 
