#include "integrity_checker.h"

// генератор хеша для поданного файла (пути до него)
char* hash_generate(char* file) {
  FILE* temp = fopen(file, "rb");  // открытие файла на бинарное чтение

  char* string = NULL;
  size_t bufferSize = 0;
  FILE* myStream = open_memstream(
      &string, &bufferSize);  // поток для перепечатки хэш суммы в читаемый вид

  EVP_MD_CTX* sha256 = EVP_MD_CTX_new();  // объект для вычисления хэша
  unsigned int mdlen, i;
  EVP_DigestInit_ex(sha256, EVP_sha256(),
                    NULL);  // уточнение функции для вычислений

  // всякое для чтения из файла (сколько и куда читаем)
  const int buffer_size = 32768;
  unsigned char buffer[buffer_size];
  int bytes_read = 0;

  while ((bytes_read = fread(buffer, 1, buffer_size, temp))) {
    EVP_DigestUpdate(sha256, buffer, bytes_read);
  }

  unsigned char hash[EVP_MAX_MD_SIZE];
  EVP_DigestFinal_ex(sha256, hash, &mdlen);
  for (i = 0; i < mdlen; i++) {
    fprintf(myStream, "%02x",
            hash[i]);  // перепечатываем хэш в адекватную строку
  }
  EVP_MD_CTX_free(sha256);
  fclose(myStream);
  fclose(temp);
  return string;
}

// генерация хэша проверочной строки для контрольного списка
char* gen_check(char* catalog) {
  char* control_part1 = "This is filecheck string for ";
  char* control =
      malloc(sizeof(char) * (strlen(catalog) + strlen(control_part1) + 2));
  snprintf(control,
           sizeof(char) * (strlen(catalog) + strlen(control_part1) + 2), "%s%s",
           control_part1, catalog);

  char* string = NULL;
  size_t bufferSize = 0;
  FILE* myStream = open_memstream(&string, &bufferSize);

  EVP_MD_CTX* sha256 = EVP_MD_CTX_new();
  unsigned int mdlen, i;
  EVP_DigestInit_ex(sha256, EVP_sha256(), NULL);

  EVP_DigestUpdate(sha256, control, strlen(control));

  unsigned char hash[EVP_MAX_MD_SIZE];
  EVP_DigestFinal_ex(sha256, hash, &mdlen);
  for (i = 0; i < mdlen; i++) {
    fprintf(myStream, "%02x", hash[i]);
  }
  EVP_MD_CTX_free(sha256);
  fclose(myStream);
  free(control);
  return string;
}