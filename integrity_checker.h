#ifndef INTEGR_CHECK
#define INTEGR_CHECK

#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <time.h>

void hello_msg();
void menu();
void get_paths(int regime, char** catalog, char** file_path);
void getter(char** orig, int mode);
void processing(FILE* log, char* catalog, char* listname, void (*operation)(FILE*, FILE*, FILE*, char*, char*),
                char* regime);
void write_file(FILE* log, FILE* pipe, FILE* file, char* catalog, char* checker);
void read_file(FILE* log, FILE* pipe, FILE* file, char* catalog, char* checker);
int file_check(char* listname);
char* to_full_path(char* catalog, char* file_path);
char* hash_generate(char* file);
char* gen_check(char* catalog);
void logger(char* tag, FILE* file, char* message, char* filename);

#endif // !INTEGR_CHECK