#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void menu();
void get_paths(int regime, char** catalog, char** filename);
void get_strings(char** orig);
void write_file(FILE* file, char* filename);

int main() {
  menu();
  return 0;
}

void menu() {
  system("clear");
  char* catalog = "";
  char* filename = "";
  printf(
      "Выберите режим работы утилиты, введя число ниже:\n1. Постановка "
      "каталога на контроль целостности.\n2. Проведение контроля целостности "
      "каталога.\n");
  int regime = 0;
  scanf("%d", &regime);
  getchar();  // захват символа переноса строки после выбора в меню
  while (regime != 1 && regime != 2) {
    printf("Выберите верную опцию: 1 или 2\n");
    scanf("%d", &regime);
    getchar();
  }
  switch (regime) {
    case 1:
      get_paths(regime, &catalog, &filename);

      break;
    case 2:
    get_paths(regime, &catalog, &filename);
      break;
  }
  free(catalog);
  free(filename);
}

void get_paths(int regime, char** catalog, char** filename) {
  if (regime == 1) {
    printf(
        "Введите абсолютный путь до каталога для постановки на контроль "
        "целостности:\n");
  } else if (regime == 2) {
    printf(
        "Введите абсолютный путь до каталога для проведения контроля "
        "целостности:\n");
  }
  *catalog = get_strings();

  if (regime == 1) {
    printf(
        "Введите абсолютный путь до каталога для сохранения списка контроля "
        "целостности:\n");
  } else if (regime == 2) {
    printf(
        "Введите абсолютный путь до соответствующего списка контроля "
        "целостности:\n");
  }
  *filename = get_strings();
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
    if (index + 1 == capacity) {
      capacity *= 2;
      path = realloc(path, capacity * sizeof(char) + 1);
      if (path == (void*)0) {
        flag = 0;
        free(path);
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

void write_file(FILE* file, char* filename) { fprintf(file, "%s\n", filename); }

void read_file(FILE* file, char* filename) {
  char* line = "";
  fscanf(file, "%s", line);
  printf("%s %s\n", line, filename);
}

// void checker(int regime, char* directory, char* file_list) {
//   FILE* checklist = tmpfile();
//   char* command = malloc((strlen(directory) + 3) * sizeof(char));
//   command = "ls ";
//   strcat(command, directory);
//   FILE* pipe = popen(command, "r");
//   if (pipe != NULL) {

//   }
// }

// постановка каталога на учет
// void set_on() {
//   system("clear");

//   snprintf(command, sizeof(command), "ls %s", directory);
//   FILE* list = fopen(filename, "w");
//   FILE* pipe = popen(command, "r");  // выполнение команды в терминале
//   if (pipe != NULL) {
//     char file_path[256];  // путь до файлов
//     // сбор строк по \n символу из pipe
//     while (fgets(file_path, sizeof(file_path), pipe) != NULL) {
//       char full_path[256] = "";
//       file_path[strcspn(file_path, "\n")] = '\0';  // замена \n на \0
//       strcat(full_path, directory);
//       strcat(full_path, file_path);
//       write_file(list, full_path);
//     }
//   }
//   pclose(pipe);
// }