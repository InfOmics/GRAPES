g++ -w -c -std=c++0x -I./include vf2.cpp
g++ -w -o vf2 vf2.o -L./lib -lvf -lstdc++ -lm -pthread