
./build/output: ./build/objects/main.o 
	gcc ./build/objects/main.o -o ./build/output 

./build/objects/main.o: ./src/main.c
	gcc -c ./src/main.c -o ./build/objects/main.o

clean: 
	rm ./build/objects/*.o ./build/output
