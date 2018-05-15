grapes: GRAPESConsole.o
	cd vflib2; make -B
	g++ -w -o grapes GRAPESConsole.o -L./vflib2/lib -lvf -lstdc++ -lm -O3 -pthread
GRAPESConsole.o: GRAPESConsole.cpp 
	g++ -w -c -O3 -std=c++0x -I./vflib2/include -I./RILib -I./GRAPESLib -I./GRAPESLib/data -I./GRAPESLib/pp GRAPESConsole.cpp

clean:
	rm grapes
	rm GRAPESConsole.o
	cd vflib2; make clean
