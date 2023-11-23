all:UniquePtr.cc Array.cpp
	g++ $^ -o $@ -ggdb -std=c++23 -Wall -Wextra
.PHONY:clean
clean:
	rm all -fs