CXX = g++
CXXFLAGS = -std=c++17 -Wall -g

main: poker.cpp
	@$(CXX) $(CXXFLAGS) poker.cpp -o poker

clean:
	@rm -f poker
	@rm -rf poker.dSYM