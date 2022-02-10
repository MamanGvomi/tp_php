MPICXX = mpic++
SRC= $(wildcard *.cpp)
OBJ= $(SRC:.cpp=.o)


all: maitre esclave

%.o: %.cpp
	$(MPICXX) -o $@ -c $< 

maitre: maitre.o
	$(MPICXX) -o maitre maitre.o

esclave: esclave.o
	$(MPICXX) -o esclave esclave.o

clean:
	@rm -rf *.o

cleanall: clean
	@rm -rf maitre esclave

run: 
	mpirun --oversubscribe -np 1 ./maitre 18 4 0