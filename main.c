#include "integrity_checker.h"

int main() {
  openlog("integrity_control", LOG_CONS, LOG_LOCAL1);
  menu();
  closelog();
  return 0;
}

void hello_msg() {
  system("clear");
  printf("%s", "\t\t\t\t\tIntergrity checker\nПроводит постановку на учет каталогов и проверку их целостности через генерацию хэш сумм SHA256.\n");
  printf("\t\tЧетко следуйте инструкциям для корректной работы утилиты.\n");
  printf("\nВыберите режим работы утилиты, введя число ниже:\n");
  printf("1. Постановка каталога на контроль целостности.\n");
  printf("2. Проведение контроля целостности каталога.\n");
}

// меню
void menu() {
  char* catalog = "\0";
  char* list_path = "\0";
  hello_msg();
  int regime = 0;
  scanf("%d", &regime);
  getchar();  // захват символа переноса строки после выбора в меню
  // если ввели букву или неверное число
  while (regime != 1 && regime != 2) {
    printf("Выберите верную опцию: 1 или 2\n");
    scanf("%d", &regime);
    getchar();
  }
  get_paths(regime, &catalog, &list_path);

  switch (regime) {
    case 1:
      if (catalog && list_path) {
        processing(catalog, list_path, write_file, "w");
      }
      break;
    case 2:
      break;
  }
  free(catalog);
  free(list_path);
}

// запрос и получение директорий
void get_paths(int regime, char** catalog, char** list_path) {
  if (regime == 1) {
    printf(
        "\nВведите абсолютный путь до каталога для постановки на контроль "
        "целостности (начиная с домашней директории):\n");
  } else if (regime == 2) {
    printf(
        "\nВведите абсолютный путь до каталога для проведения контроля "
        "целостности (начиная с домашней директории):\n");
  }
  getter(catalog, 1);

  if (regime == 1) {
    printf(
        "\nВведите полный путь до места сохранения списка контроля "
        "целостности (вместе с названием .txt). Если файл существует, он будет "
        "перезаписан:\n");
  } else if (regime == 2) {
    printf(
        "\nВведите полный путь до соответствующего списка контроля "
        "целостности (вместе с названием .txt):\n");
  }
  getter(list_path, 0);
}

// запрос пути до каталогов от пользователя
void getter(char** orig, int mode) {
  int i = 0, length = 0, capacity = 1, flag = 1;
  char* string = malloc(sizeof(char) * (capacity + 2));
  char c = getchar();
  for (i = 0; c != '\n' && flag; i++) {
    length++;
    string[i] = c;
    if (i + 1 == capacity) {
      capacity *= 2;
      char* temp = realloc(string, sizeof(char) * (capacity + 2));
      if (!temp) {
        free(string);
        flag = 0;
      } else {
        string = temp;
        for (int j = i + 1; j < capacity + 2; j++) {
          string[j] = '\0';
        }
      }
    }
    c = getchar();
  }
  if (string[i] != '/' && mode && flag) {
    string[length] = '/';
    string[length + 1] = '\0';
  } else if (flag) {
    string[length] = '\0';
  }
  *orig = string;
}

// обработка каталога
void processing(char* catalog, char* listname, void (*operation)(FILE*, char*),
                char* regime) {
  // system("clear");
  char* command = malloc(sizeof(char) * (strlen(catalog) + 8));
  snprintf(command, 8 + strlen(catalog), "ls -XN %s", catalog);
  FILE* pipe = popen(command, "r");  // выполнение команды в терминале
  FILE* file = fopen(listname, regime);
  char* checker = gen_check(catalog);
  fprintf(file, "%s\n", checker);
  if (pipe != NULL) {
    char file_path[256] = "\0";  // путь до файлов
    while (fgets(file_path, sizeof(file_path), pipe) != NULL) {
      file_path[strcspn(file_path, "\n")] = '\0';  // замена \n на \0
      char* full_path = to_full_path(catalog, file_path);
      operation(file, full_path);
      free(full_path);
    }
  }
  free(command);
  free(checker);
  pclose(pipe);
  fclose(file);
}

char* to_full_path(char* catalog, char* file_path) {
  char* full_path =
      malloc(sizeof(char) * (strlen(catalog) + strlen(file_path) + 1));
  snprintf(full_path, sizeof(char) * (strlen(catalog) + strlen(file_path) + 1),
           "%s%s", catalog, file_path);
  return full_path;
}

// создание контрольного списка
void write_file(FILE* file, char* filename) {
  fprintf(file, "%s ", filename);
  char* hash = hash_generate(filename);
  fprintf(file, "%s\n", hash);
  free(hash);
}
