GCC = gcc -Wall -Wextra -Werror
SANITIZE = -fsanitize=address
FILES = main.c helpers.c
LIBS = -lssl -lcrypto
INSTALL = apt-get install libssl-dev

all:
	$(GCC) $(FILES) -o programm $(LIBS)
	./programm

check:
	clang-format -i $(FILES)
	cppcheck --enable=all --suppress=missingIncludeSystem main.c

valgrind: clear
	$(GCC) $(FILES) -o programm $(LIBS)
	valgrind --tool=memcheck --leak-check=yes --track-origins=yes --leak-check=full --show-leak-kinds=all -s ./programm 

sanit:
	$(GCC) $(SANITIZE) $(FILES) -o programm $(LIBS)
	./programm

install:
	$(INSTALL)

clear:
	rm programm