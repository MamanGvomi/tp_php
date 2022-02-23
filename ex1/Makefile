MPICXX = g++
SRC= programme_hote.cpp
# SRC= $(wildcard *.cpp)
OBJ= $(SRC:.cpp=.o)
EXEC= programme_hote

%.o: %.cpp
	$(MPICXX) -O3 -lOpenCL -o $@ -c $< 

main: $(OBJ) 
	$(MPICXX) -o $@ $^

clean:
	@rm -rf *.o

cleanall: clean
	@rm -rf $(EXEC)

