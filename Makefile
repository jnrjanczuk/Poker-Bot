CXX = g++
CXXFLAGS = -std=c++17 -Wall -g

main: poker.cpp
	@$(CXX) $(CXXFLAGS) poker.cpp -o poker
	@$(CXX) $(CXXFLAGS) tests.cpp -o tests

clean:
	@rm -f poker
	@rm -rf poker.dSYM