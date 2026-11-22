CXX = g++
CXXFLAGS = -std=c++17 -O2 -Iinclude -Itest -I/opt/homebrew/include -I/usr/local/include
LDFLAGS = -L/opt/homebrew/lib -L/usr/local/lib -lgtest -lgtest_main -pthread

ALL_TESTS = $(wildcard test/*.cpp)
LOAD_TESTS = $(filter %LOAD_AVL.cpp %LOAD%.cpp %Load%.cpp, $(ALL_TESTS))
UNIT_TESTS = $(filter-out $(LOAD_TESTS), $(ALL_TESTS))

.PHONY: main1 main2 test load_test all_test plot clean

main1:
	$(CXX) $(CXXFLAGS) src/lab1/main_lab1.cpp -o main1_bin
	./main1_bin

main2:
	$(CXX) $(CXXFLAGS) src/lab2/main_lab2.cpp -o main2_bin
	./main2_bin

test:
	$(CXX) $(CXXFLAGS) $(UNIT_TESTS) -o test_bin $(LDFLAGS)
	./test_bin

load_test:
	$(CXX) $(CXXFLAGS) $(LOAD_TESTS) -o load_test_bin $(LDFLAGS)
	./load_test_bin

all_test:
	$(CXX) $(CXXFLAGS) $(ALL_TESTS) -o all_test_bin $(LDFLAGS)
	./all_test_bin

benchmark.csv:
	$(MAKE) load_test

plot: benchmark.csv
	python3 scripts/plot.py

clean:
	rm -f main main1_bin main2_bin test_bin load_test_bin all_test_bin benchmark.csv benchmark_plot.png
	find . -type f -name "*.o" -delete
	find . -type f -name "*.out" -delete
	find . -type f -name "*.gch" -delete
	find . -type d -name "*.dSYM" -exec rm -rf{} +