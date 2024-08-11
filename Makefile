GCC = gcc -Wall -Wextra -Werror
SANITIZE = -fsanitize=address
FILES = main.c helpers.c
LIBS = -lssl -lcrypto
INSTALL = apt-get install libssl-dev

all:
	$(GCC) $(FILES) -o program $(LIBS)
	./program

check:
	clang-format -i $(FILES)
	cppcheck --enable=all --suppress=missingIncludeSystem main.c

valgrind: clear
	$(GCC) $(FILES) -o program $(LIBS)
	valgrind --tool=memcheck --leak-check=yes --track-origins=yes --leak-check=full --show-leak-kinds=all -s ./program

sanit:
	$(GCC) $(SANITIZE) $(FILES) -o program $(LIBS)
	./program

install:
	$(INSTALL)

clear:
	rm program