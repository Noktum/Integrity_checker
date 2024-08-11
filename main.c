#include "integrity_checker.h"

int main() {
  menu();
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
    scanf("Режим: %d", &regime);
    getchar();
  }
  get_paths(regime, &catalog, &list_path);

  switch (regime) {
    case 1:
      if (catalog && list_path) {
        processing(log, catalog, list_path, write_file, "w");
      }
      break;
    case 2:
      if (catalog && list_path) {
        processing(log, catalog, list_path, read_file, "r");
      }
      break;
  }
  free(catalog);
  free(list_path);
  fclose(log);
}

/*
чтение строки неизвестной длины из потока до разделителя
flag и length нужны только для ввода каталога и создания списка
*/
char* stream_read(char** string, int* lenght, FILE* stream, int* flag,
                  char delim) {
  int i = 0, capacity = 1;
  *flag = 1, *lenght = 0;
  char* line = realloc(*string, sizeof(char) * (capacity + 2));
  char c = fgetc(stream);
  if (c == '\n') {
    line[0] = '\0';
  } else if (c == EOF) {
    free(line);
    line = NULL;
  }
  for (i = 0; *flag && c != delim && c != EOF; i++) {
    (*lenght)++;
    line[i] = c;
    if (i + 1 == capacity) {
      capacity *= 2;
      char* temp = realloc(line, sizeof(char) * (capacity + 2));
      if (!temp) {
        free(line);
        *flag = 0;
      } else {
        line = temp;
        for (int j = i + 1; j < capacity + 2; j++) {
          line[j] = '\0';
        }
      }
    }
    c = fgetc(stream);
  }
  *string = line;
  return line;
}

// обработка каталога
void processing(FILE* log, char* catalog, char* listname,
                void (*operation)(FILE*, FILE*, FILE*, char*, char*),
                char* regime) {
  char* command = malloc(sizeof(char) * (strlen(catalog) + 33));
  snprintf(command, 33 + strlen(catalog), "find %s -maxdepth 1 -type f | sort",
           catalog);
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

// запись строки в контрольный файл
void write_file(FILE* log, FILE* pipe, FILE* file, char* catalog,
                char* checker) {
  logger("info", log, "Начало постановки на контроль целостности каталога",
         catalog);
  fprintf(file, "%s\n", checker);
  if (pipe != NULL) {
    int length = 0, flag = 1;
    char* file_path = NULL;  // путь до файлов
    while (stream_read(&file_path, &length, pipe, &flag, '\n') != NULL) {
      fprintf(file, "%s", file_path);
      putc(28, file);
      char* hash = hash_generate(file_path);
      fprintf(file, "%s\n", hash);
      logger("info", log, "В список контроля целостности внесен файл",
             file_path);
      free(hash);
    }
    free(file_path);
  }
}

// четние списка контроля целостности
void read_file(FILE* log, FILE* pipe, FILE* file, char* catalog,
               char* checker) {
  logger("info", log, "Начало проверки целостности каталога", catalog);
  char* id = malloc(sizeof(char) * 70);
  fgets(id, 70, file);
  id[strcspn(id, "\n")] = '\0';
  if (strcmp(checker, id) == 0 && pipe != NULL) {
    int flag = 1, stream_flag = 1;
    int length = 0;
    char* file_path = NULL;
    char* list_check = NULL;
    char* hash_check = NULL;
    while (stream_read(&file_path, &length, pipe, &stream_flag, '\n') != NULL &&
           stream_read(&list_check, &length, file, &stream_flag, (char)28) !=
               NULL &&
           stream_read(&hash_check, &length, file, &stream_flag, '\n') !=
               NULL) {
      if (flag == 0) {
        break;
        break;
      }
      char* hash = hash_generate(file_path);
      file_checker(log, &flag, list_check, hash_check, file_path, hash);
      free(hash);
    }
    free(hash_check);
    free(list_check);
    free(file_path);
    if (flag) {
      printf("\nЦелостность католога заверена\n");
      logger("info", log, "Целостность католога заверена", catalog);
    } else {
      printf("\nЦелостность каталога нарушена\n");
      logger("error", log, "Целостность католога нарушена", catalog);
    }
  } else if (strcmp(checker, id) != 0) {
    logger("info", log, "Неверный список контроля целостности для каталога",
           catalog);
  }
  free(id);
}

//проверка соответствия хэшей и названий файлов
void file_checker(FILE* log, int* flag, char* list_check, char* hash_check,
                  char* file_path, char* hash) {
  if (strcmp(list_check, file_path) > 0) {
    logger("error", log, "В каталоге удален файл", list_check);
    *flag = 0;
  } else if (strcmp(list_check, file_path) < 0) {
    logger("error", log, "В каталог добавлен файл", file_path);
    *flag = 0;
  }

  if (strcmp(hash, hash_check) != 0) {
    logger("error", log, "Изменен файл", file_path);
    *flag = 0;
  }
}

// проверка что список котроля txt и открывается
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
