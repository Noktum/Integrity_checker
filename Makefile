GCC = gcc -Wall -Wextra -Werror
INSTALL = sudo apt-get install libssl-dev 
all:
	$(GCC) main.c -o programm -lssl -lcrypto
	./programm

check:
	clang-format -i *.c
	cppcheck --enable=all --suppress=missingIncludeSystem main.c

valgrind:
	valgrind --tool=memcheck --leak-check=yes ./programm 