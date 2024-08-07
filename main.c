#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void menu();
void set_on();
char* get_path();
void write_file(FILE* file, char* filename);
void check();

int main() {
  menu();
  return 0;
}

void menu() {
  system("clear");
  printf(
      "Выберите режим работы утилиты, введя число ниже:\n1. Постановка "
      "каталога на контроль целостности.\n2. Проведение контроля целостности "
      "каталога.\n");
  int regime = 0;
  scanf("%d", &regime);
  getchar();  // захват символа переноса строки после выбора в меню
  while (regime != 1 && regime != 2) {
    switch (regime) {
      case 1:
        set_on();
        break;
      case 2:
        check();
        break;
      default:
        printf("Выберите верную опцию: 1 или 2\n");
        scanf("%d", &regime);
        getchar();
        break;
    }
  }
}

// постановка каталога на учет
void set_on() {
  system("clear");
  printf("Введите абсолютный путь до каталога, который будет заверен:\n");
  // проверяемый каталог, команда для терминала, путь и название списка файлов
  char *directory = get_path(), command[strlen(directory) + 3], *filename;

  printf(
      "Введите абсолютный путь до каталога, куда нужно будет сохранить\nсписок "
      "контроля целостности:\n");
  filename = get_path();  // куда сохранить список контроля
  filename = realloc(filename, strlen(filename) + strlen("control_list.txt"));
  strcat(filename, "control_list.txt");
  printf("%s\n%s\n", directory, filename);
  snprintf(command, sizeof(command), "ls %s", directory);
  FILE* list = fopen(filename, "w");
  FILE* pipe = popen(command, "r");  // выполнение команды в терминале
  if (pipe != NULL) {
    char file_path[256];  // путь до файлов
    // сбор строк по \n символу из pipe
    while (fgets(file_path, sizeof(file_path), pipe) != NULL) {
      char full_path[256] = "";
      file_path[strcspn(file_path, "\n")] = '\0';  // замена \n на \0
      strcat(full_path, directory);
      strcat(full_path, file_path);
      write_file(list, full_path);
    }
  }
  pclose(pipe);
}

// запрос пути до каталога от пользователя
char* get_path() {
  int index = 0, length = 1, capacity = 1;
  char* path = malloc(sizeof(char) * 2);
  char c = getchar();
  while (c != '\n') {
    path[index] = c;
    length++;
    index++;
    if (index + 1 == capacity) {
      capacity *= 2;
      path = realloc(path, capacity * sizeof(char) + 1);
      printf("!%ld\n", strlen(path));
    }
    c = getchar();
  }
  if (path[index - 1] != '/') {
    path[index] = '/';
  }
  path[length] = '\0';
  return path;
}

void write_file(FILE* file, char* filename) { fprintf(file, "%s\n", filename); }

void read_file(FILE* file, char* filename) {
  char* line = "";
  fscanf(file, "%s", line);
  printf("%s %s\n", line, filename);
}

void check() {
  system("clear");
  printf("Введите абсолютный путь до каталога требующего проверки целостности:\n");
  // проверяемый каталог, команда для терминала, путь и название списка файлов
  char *directory = get_path(), command[strlen(directory) + 3], *filename;

  printf(
      "Введите абсолютный путь до каталога, где находится\nсписок "
      "контроля целостности:\n");
  filename = get_path();  // куда сохранить список контроля
  filename = realloc(filename, strlen(filename) + strlen("control_list.txt"));
  strcat(filename, "control_list.txt");
  printf("%s\n%s\n", directory, filename);
  snprintf(command, sizeof(command), "ls %s", directory);
  FILE* list = fopen(filename, "r");
  FILE* pipe = popen(command, "r");  // выполнение команды в терминале
  if (pipe != NULL) {
    char file_path[256];  // путь до файлов
    // сбор строк по \n символу из pipe
    while (fgets(file_path, sizeof(file_path), pipe) != NULL) {
      char full_path[256] = "";
      file_path[strcspn(file_path, "\n")] = '\0';  // замена \n на \0
      strcat(full_path, directory);
      strcat(full_path, file_path);
      read_file(list, full_path);
    }
  }
  pclose(pipe);
}