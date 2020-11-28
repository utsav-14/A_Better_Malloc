rm mem.o libmem.so main 
g++ -c -fpic mem.cpp
g++ -shared -o libmem.so mem.o 
g++ main.cpp -lmem -L. -o main
./main