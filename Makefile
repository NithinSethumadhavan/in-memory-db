#
# Compile and run with the following command:
#   make run
# Or only compile with
#   make

CXXFLAGS = -g -Wall -Wextra -std=c++11
PROJECT = project
all : $(PROJECT)

run : $(PROJECT)
	./$(PROJECT)

clean :
	rm -f $(PROJECT) *.o

project.o : project.cpp
	$(CXX) $(CXXFLAGS) -c project.cpp

$(PROJECT) : project.o
	$(CXX) $^ -o $@
