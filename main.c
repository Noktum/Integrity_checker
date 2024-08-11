#include "integrity_checker.h"

int main() {
  openlog("integrity_control", LOG_CONS, LOG_SYSLOG);
  menu();
  closelog();
  return 0;
}

// меню
void menu() {
  FILE* log = fopen("journal.log", "a");
  char* catalog = "\0";
  char* list_path = "\0";
  hello_msg();
  int regime = 0;
  scanf("%d", &regime);
  getchar();  // захват символа переноса строки после выбора в меню
  while (regime != 1 && regime != 2) {  // если ввели букву или неверное число
    printf("Выберите верную опцию: 1 или 2\n");
    scanf("%d", &regime);
    getchar();
  }
  get_paths(regime, &catalog, &list_path);

  switch (regime) {
    case 1:
      logger("info", log, "Начало постановки на контроль целостности каталога",
             catalog);
      if (catalog && list_path) {
        processing(log, catalog, list_path, write_file, "w");
      }
      break;
    case 2:
      logger("info", log, "Начало проверки целостности каталога", catalog);
      if (catalog && list_path) {
        processing(log, catalog, list_path, read_file, "r");
      }
      break;
  }
  free(catalog);
  free(list_path);
  fclose(log);
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
void processing(FILE* log, char* catalog, char* listname,
                void (*operation)(FILE*, FILE*, FILE*, char*, char*), char* regime) {
  char* command = malloc(sizeof(char) * (strlen(catalog) + 8));
  snprintf(command, 8 + strlen(catalog), "ls -XN %s", catalog);
  FILE* pipe = popen(command, "r");  // выполнение команды в терминале
  FILE* file = fopen(listname, regime);
  char* checker = gen_check(catalog);

  if (file_check(listname) == 0) {
    logger("info", log,
           "Ошибка при создании/чтении списка контроля целостности", listname);
    free(command);
    free(checker);
    pclose(pipe);
  } else {
    operation(log, pipe, file, catalog, checker);
    free(command);
    free(checker);
    pclose(pipe);
    fclose(file);
  }
}

// объекдинение каталога и имени файла в 1 строку
char* to_full_path(char* catalog, char* file_path) {
  char* full_path =
      malloc(sizeof(char) * (strlen(catalog) + strlen(file_path) + 1));
  snprintf(full_path, sizeof(char) * (strlen(catalog) + strlen(file_path) + 1),
           "%s%s", catalog, file_path);
  return full_path;
}

// запись строки в контрольный файл
void write_file(FILE* log, FILE* pipe, FILE* file, char* catalog,
                char* checker) {
  fprintf(file, "%s\n", checker);
  if (pipe) {
    char* file_path =
        malloc(sizeof(char) * strlen(catalog) * 2);  // путь до файлов
    while (fscanf(pipe, "%s", file_path) != EOF) {
      char* full_path = to_full_path(catalog, file_path);
      fprintf(file, "%s ", full_path);
      char* hash = hash_generate(full_path);
      fprintf(file, "%s\n", hash);
      logger("info", log, "В список контроля целостности внесен файл",
             full_path);
      free(hash);
      free(full_path);
    }
    free(file_path);
  }
}

void read_file(FILE* log, FILE* pipe, FILE* file, char* catalog,
               char* checker) {
  char* id = malloc(sizeof(char) * 70);
  fscanf(file, "%s", id);
  if (strcmp(checker, id) == 0 && pipe != NULL) {
    int flag = 1;
    char* file_path = malloc(sizeof(char) * strlen(catalog) * 2);
    char* list_check = malloc(sizeof(char) * strlen(catalog) * 2);
    char* hash_check = malloc(sizeof(char) * 70);
    while (fscanf(pipe, "%s", file_path) != EOF &&
           fscanf(file, "%s %s", list_check, hash_check) != EOF) {
      char* full_path = to_full_path(catalog, file_path);
      if (strcmp(full_path, list_check) != 0) {
        logger("error", log, "Удален или добавлен новый файл в каталог",
               full_path);
        flag = 0;
        break;
      }
      char* hash = hash_generate(full_path);
      if (strcmp(hash, hash_check) != 0) {
        logger("error", log, "Изменен файл", full_path);
        flag = 0;
      }
      free(hash);
      free(full_path);
    }
    free(hash_check);
    free(list_check);
    free(file_path);
    if (flag) {
      printf("Целостность католога заверена\n");
    } else {
      printf("Целостность каталога нарушена\n");
    }
  } else if (strcmp(checker, id) != 0) {
    logger("info", log, "Неверный список контроля целостности для каталога",
           catalog);
  }
  free(id);
}

int file_check(char* listname) {
  int length = strlen(listname), flag = 1;
  if (listname[length - 1] != 't' || listname[length - 2] != 'x' ||
      listname[length - 3] != 't' || listname[length - 4] != '.') {
    flag = 0;
  }
  if (flag) {
    FILE* file = fopen(listname, "r");
    if (!file) {
      flag = 0;
    } else {
      fclose(file);
    }
  }
  return flag;
}
