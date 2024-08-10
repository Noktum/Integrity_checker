#ifndef INTEGR_CHECK
#define INTEGR_CHECK

#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

void menu();
void get_paths(int regime, char** catalog, char** file_path);
void getter(char** orig, int mode);
void processing(char* catalog, char* listname, void (*operation)(FILE*, char*),
                char* regime);
void write_file(FILE* file, char* filename);
char* to_full_path(char* catalog, char* file_path);
char* hash_generate(char* file);
char* gen_check(char* catalog);

#endif // !INTEGR_CHECK