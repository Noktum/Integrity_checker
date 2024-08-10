GCC = gcc -Wall -Wextra -Werror
SANITIZE = -fsanitize=address
INSTALL = sudo apt-get install libssl-dev
LIBS = -lssl -lcrypto

all:
	$(GCC) main.c hashes.c -o programm $(LIBS)
	./programm

check:
	clang-format -i *.c
	cppcheck --enable=all --suppress=missingIncludeSystem main.c

valgrind: clear
	$(GCC) main.c hashes.c -o programm $(LIBS)
	valgrind --tool=memcheck --leak-check=yes --track-origins=yes ./programm 

sanit: clear
	$(GCC) $(SANITIZE) main.c hashes.c -o programm $(LIBS)
	./programm

clear:
	rm programm test_folder.txt