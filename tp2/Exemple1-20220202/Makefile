MPICXX = mpic++
SRC= sum_tab.cpp
# SRC= $(wildcard *.cpp)
OBJ= $(SRC:.cpp=.o)
EXEC= main

%.o: %.cpp
	$(MPICXX) -o $@ -c $< 

main: $(OBJ) 
	$(MPICXX) -o $@ $^

clean:
	@rm -rf *.o

cleanall: clean
	@rm -rf $(EXEC)

