CXX = clang++

CXXFLAGS = -std=c++23 -Iinclude -I/opt/homebrew/include
LDLIBS   = -L/opt/homebrew/lib -lgtest -lgtest_main -lpthread

SRC_LAB1 = src/lab1/main_lab1.cpp
SRC_LAB2 = src/lab2/main_lab2.cpp

TEST1_SRCS = test/TEST_AVL.cpp
TEST2_SRCS = test/TEST_command.cpp

all: lab1

laba1: lab1
lab1:
	$(CXX) $(CXXFLAGS) $(SRC_LAB1) -o laba1
	./laba1

laba2: lab2
lab2:
	$(CXX) $(CXXFLAGS) $(SRC_LAB2) -o laba2
	./laba2

test1:
	$(CXX) $(CXXFLAGS) $(TEST1_SRCS) -o test1_bin $(LDLIBS)
	./test1_bin

test2:
	$(CXX) $(CXXFLAGS) $(TEST2_SRCS) -o test2_bin $(LDLIBS)
	./test2_bin

test: test1 test2

ui:
	@echo "Сборка интерфейса через CMake..."
	@mkdir -p ui/build
	cd ui/build && cmake .. && $(MAKE)
	@echo "Запуск интерфейса..."
	open ui/build/ui.app

clean:
	rm -f laba1 laba2 test1_bin test2_bin
	rm -rf ui/build
	find . -name ".DS_Store" -type f -delete

.PHONY: all laba1 lab1 laba2 lab2 test1 test2 test ui clean
