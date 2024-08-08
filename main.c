#include <openssl/ssl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void menu();
void get_paths(int regime, char** catalog, char** file_path);
void get_strings(char** orig);
void processing(char* catalog, char* filename, void (*operation)(char*, char*));
void write_file(char* list, char* filename);
void read_file(char* catalog, char* list, char* filename);

int main() {
  menu();
  return 0;
}

// меню
void menu() {
  // system("clear");
  char* catalog = "\0";
  char* file_path = "\0";
  printf(
      "Выберите режим работы утилиты, введя число ниже:\n1. Постановка "
      "каталога на контроль целостности.\n2. Проведение контроля целостности "
      "каталога.\n");
  int regime = 0;
  scanf("%d", &regime);
  getchar();  // захват символа переноса строки после выбора в меню
  // если ввели букву или неверное число
  while (regime != 1 && regime != 2) {
    printf("Выберите верную опцию: 1 или 2\n");
    scanf("%d", &regime);
    getchar();
  }
  get_paths(regime, &catalog, &file_path);
  switch (regime) {
    case 1:
      if (catalog && file_path) {
        processing(catalog, file_path, write_file);
      }
      break;
    case 2:
      break;
  }
  free(catalog);
  free(file_path);
}

// запрос и получение директорий
void get_paths(int regime, char** catalog, char** file_path) {
  if (regime == 1) {
    printf(
        "Введите абсолютный путь до каталога для постановки на контроль "
        "целостности:\n");
  } else if (regime == 2) {
    printf(
        "Введите абсолютный путь до каталога для проведения контроля "
        "целостности:\n");
  }
  get_strings(catalog);

  if (regime == 1) {
    printf(
        "Введите полный путь до места сохранения списка контроля "
        "целостности (вместе с названием):\n");
  } else if (regime == 2) {
    printf(
        "Введите полный путь до соответствующего списка контроля "
        "целостности:\n");
  }
  get_strings(file_path);
}

// запрос пути до каталогов от пользователя
void get_strings(char** orig) {
  int index = 0, length = 1, capacity = 1, flag = 1;
  char* path = malloc(sizeof(char) * 2);

  char c = getchar();
  while (c != '\n' && flag) {
    path[index] = c;
    length++;
    index++;
    if (index == capacity) {
      capacity *= 2;
      char* temp = realloc(path, (capacity + 1) * sizeof(char));
      if (!path) {
        flag = 0;
        free(path);
      } else {
        path = temp;
      }
    }
    c = getchar();
  }
  if (flag) {
    if (path[index - 1] != '/') {
      path[index] = '/';
    }
    path[length] = '\0';
    *orig = path;
    path = NULL;
  }
}

// обработка каталога
void processing(char* catalog, char* filename,
                void (*operation)(char*, char*)) {
  // system("clear");
  char* command = (char*)malloc(sizeof(char) * (strlen(catalog) + 8));
  snprintf(command, 7 + strlen(catalog), "ls -XN %s", catalog);
  FILE* pipe = popen(command, "r");  // выполнение команды в терминале
  if (pipe != NULL) {
    char file_path[256] = "\0";  // путь до файлов
    // сбор строк по \n символу из pipe
    while (fgets(file_path, sizeof(file_path), pipe) != NULL) {
      char full_path[256] = "\0";
      file_path[strcspn(file_path, "\n")] = '\0';  // замена \n на \0
      strcat(full_path, catalog);
      strcat(full_path, file_path);
      operation(filename, full_path);
    }
  }
  free(command);
  pclose(pipe);
}

// создание контрольного списка
void write_file(char* list, char* filename) {
  FILE* file = fopen(list, "a");
  fprintf(file, "%s ", filename);

  FILE* hash = fopen(filename, "rb");
  
  SHA256_CTX sha256;
  SHA256_Init(&sha256);

  const int buffer_size = 32768;
  unsigned char buffer[buffer_size];
  int bytes_read = 0;

  while ((bytes_read = fread(buffer, 1, buffer_size, hash))) {
    SHA256_Update(&sha256, buffer, bytes_read);
  }
  unsigned char hash_sum[SHA256_DIGEST_LENGTH];
  SHA256_Final(hash_sum, &sha256);
  fclose(hash);

  fprintf(file, "%s\n", hash_sum);
  fclose(file);
}

// сверка с контрольным списком
// void read_file(char* directory, char* list, char* filename) {
//   char* line = "";
//   FILE* list = fopen(filename, "r");
//   fscanf(list, "%s", line);
//   printf("%s %s\n", line, filename);
//   fclose(list);
// }

// void checker(int regime, char* directory, char* file_list) {
//   FILE* checklist = tmpfile();
//   char* command = malloc((strlen(directory) + 3) * sizeof(char));
//   command = "ls ";
//   strcat(command, directory);
//   FILE* pipe = popen(command, "r");
//   if (pipe != NULL) {

//   }
// }