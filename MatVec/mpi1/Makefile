MPICXX = mpic++
SRC= $(wildcard *.cpp)
OBJ= $(SRC:.cpp=.o)
EXEC= main

%.o :%.cpp
	$(MPICXX) -o $@ -c $< -fopenmp

main : $(OBJ) 
	$(MPICXX) -o $@ $^ -fopenmp

.PHONY: clean mrproper

clean:
	@rm -rf *.o

mrproper: clean
	@rm -rf $(EXEC)

run:
	mpirun -np 3 ./main 2 4 0 test.txt