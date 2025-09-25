CXXFLAGS=-std=c++17 -pthread
CXX=g++
all:
	$(CXX) $(CXXFLAGS) project_part1.cpp -o project_part1

check:
	chmod +x project_part1
	./project_part1
clean:
	$(RM) project_part1
