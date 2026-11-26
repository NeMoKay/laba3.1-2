CXX = g++
CXXFLAGS = -std=c++17 -O2 -Iinclude -Itest -I/opt/homebrew/include -I/usr/local/include
LDFLAGS = -L/opt/homebrew/lib -L/usr/local/lib -lgtest -lgtest_main -pthread

ALL_TESTS = $(wildcard test/*.cpp)
LOAD_TESTS = $(wildcard test/*LOAD*.cpp test/*Load*.cpp)
UNIT_TESTS = $(filter-out $(LOAD_TESTS), $(ALL_TESTS))

.PHONY: main1 main2 test test_avl test_set test_vfs load_test load_test_avl load_test_vfs all_test plot_AVL plot_VFS clean

main1:
	$(CXX) $(CXXFLAGS) src/lab1/main_lab1.cpp -o main1_bin
	./main1_bin

main2:
	$(CXX) $(CXXFLAGS) src/lab2/main_lab2.cpp -o main2_bin
	./main2_bin

test:
	$(CXX) $(CXXFLAGS) $(UNIT_TESTS) -o test_bin $(LDFLAGS)
	./test_bin

test_avl:
	$(CXX) $(CXXFLAGS) test/TEST_AVL.cpp -o test_avl_bin $(LDFLAGS)
	./test_avl_bin

test_set:
	$(CXX) $(CXXFLAGS) test/TEST_Set.cpp -o test_set_bin $(LDFLAGS)
	./test_set_bin

test_vfs:
	$(CXX) $(CXXFLAGS) test/TEST_VirtualFS.cpp -o test_vfs_bin $(LDFLAGS)
	./test_vfs_bin

load_test:
	$(CXX) $(CXXFLAGS) $(LOAD_TESTS) -o load_test_bin $(LDFLAGS)
	./load_test_bin

load_test_avl:
	$(CXX) $(CXXFLAGS) test/TEST_LOAD_AVL.cpp -o load_test_avl_bin $(LDFLAGS)
	./load_test_avl_bin

load_test_vfs:
	$(CXX) $(CXXFLAGS) test/TEST_LOAD_VirtualFS.cpp -o load_test_vfs_bin $(LDFLAGS)
	./load_test_vfs_bin

all_test:
	$(CXX) $(CXXFLAGS) $(ALL_TESTS) -o all_test_bin $(LDFLAGS)
	./all_test_bin

plot_AVL: load_test_avl
	python3 scripts/plot1.py

plot_VFS: load_test_vfs
	python3 scripts/plot2.py

clean:
	rm -f main main1_bin main2_bin test_bin test_avl_bin test_set_bin test_vfs_bin load_test_bin load_test_avl_bin load_test_vfs_bin all_test_bin benchmark.csv benchmark_vfs.csv benchmark_plot.png
	find . -type f -name "*.o" -delete
	find . -type f -name "*.out" -delete
	find . -type f -name "*.gch" -delete
	find . -type d -name "*.dSYM" -exec rm -rf {} +