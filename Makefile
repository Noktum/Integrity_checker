GCC = gcc -Wall -Wextra -Werror

all:
	$(GCC) main.c -o programm
	./programm

check:
	clang-format -i *.c
	cppcheck --enable=all --suppress=missingIncludeSystem main.c