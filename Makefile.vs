#
# Compile and run with the following command:
#   nmake run
# Or only compile with
#   nmake

CXXFLAGS = /EHsc
PROJECT  = project.exe
all : $(PROJECT)

run : $(PROJECT)
	$(PROJECT)

clean :
	del /f $(PROJECT) *.obj

$(PROJECT) : project.cpp
	$(CXX) $(CXXFLAGS) project.cpp -o $@
