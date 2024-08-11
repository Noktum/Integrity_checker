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
  char buffer[buffer_size];
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

// запись логов в файл
void logger(char* tag, FILE* file, char* message, char* filename) {
  time_t now;
  time(&now);
  fprintf(file, "%s [%s]: %s - %s\n", ctime(&now), tag, message, filename);
}

// стартовое меню
void hello_msg() {
  system("clear");
  printf("%s", "\t\t\t\t\tIntergrity checker\n");
  printf(
      "Проводит постановку на учет каталогов и проверку их целостности через "
      "генерацию хэш сумм SHA256.\n");
  printf("\t\tЧетко следуйте инструкциям для корректной работы утилиты.\n");
  printf("\nВыберите режим работы утилиты, введя число ниже:\n");
  printf("1. Постановка каталога на контроль целостности.\n");
  printf("2. Проведение контроля целостности каталога.\n");
}

// запрос и получение директорий
void get_paths(int regime, char** catalog, char** list_path) {
  if (regime == 1) {
    printf("\nВведите абсолютный путь до каталога для постановки на контроль ");
    printf("целостности\n(начиная с домашней директории):\n");
  } else if (regime == 2) {
    printf("\nВведите абсолютный путь до каталога для проведения контроля ");
    printf("целостности\n(начиная с домашней директории):\n");
  }
  getter(catalog, 1);

  if (regime == 1) {
    printf("\nВведите полный путь до места сохранения списка контроля ");
    printf("целостности\n(вместе с названием .txt). Если файл существует,\n");
    printf("то он будет перезаписан:\n");
  } else if (regime == 2) {
    printf("\nВведите полный путь до соответствующего списка контроля ");
    printf("целостности (вместе с названием .txt):\n");
  }
  getter(list_path, 0);
}