CXXFLAGS=-std=c++17 -pthread
CXX=g++
all:
	$(CXX) $(CXXFLAGS) device.cpp -o device

check:
	chmod +x device
	./device
clean:
	$(RM) device
