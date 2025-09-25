CXXFLAGS=-std=c++17 -pthread
CXX=g++
all:
	$(CXX) $(CXXFLAGS) device.cpp -o project_part1

check:
	chmod +x device
	./project_part1
clean:
	$(RM) device
