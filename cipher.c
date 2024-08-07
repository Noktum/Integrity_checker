#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int check_file(const char* filename);
void read_file(const char* filename);
void update_file(const char* filename, const char* text);
void encrypt_files(const char* directory, int shift);
void ceaser(FILE* file, int shift);
void operations(FILE* file, char* file_extension, char* full_path, int shift);
void menu(int* choice, char* filename);

int main() {
  int choice = 0;
  char filename[256] = "";
  do {
    scanf("%d", &choice);
    menu(&choice, filename);
  } while (choice != -1);
  return 0;
}

void menu(int* choice, char* filename) {
  char text[256];
  int shift;
  switch (*choice) {
    case 1:  // read and write in terminal
      scanf("%255s", filename);
      if (check_file(filename) == 0) {
        read_file(filename);
        printf("\n");
      }
      break;
    case 2:  // запись строки в конец открытого в 1 пункте файла
      getchar();                         //очищаем буфер ввода
      fgets(text, sizeof(text), stdin);  //считываем строку с пробелами
      if (strlen(filename) == 0) {
        printf("n/a");
        printf("\n");
        break;
      }
      FILE* check = fopen(filename, "r");  //для проверки cppcheck
      if (check) {
        fclose(check);
        update_file(filename, text);
        read_file(filename);
      } else {
        printf("n/a");
        printf("\n");
      }
      break;
    case 3:
      scanf("%d", &shift);
      encrypt_files("ai_modules", shift);
      break;
    case -1:
      break;
    default:
      printf("n/a");
      printf("\n");
      break;
  }
}

int check_file(const char* filename) {
  int exit = 0;
  FILE* file = fopen(filename, "r");
  if (file == NULL) {
    printf("n/a");
    printf("\n");
    exit = -1;
  } else if (fgetc(file) == EOF) {
    printf("n/a");
    printf("\n");
    exit = 1;
    fclose(file);
  }
  return exit;
}

void read_file(const char* filename) {
  FILE* file = fopen(filename, "r");
  char c = EOF;
  while ((c = fgetc(file)) != EOF) {
    putchar(c);
  }
  fclose(file);
}

void update_file(const char* filename, const char* text) {
  FILE* file = fopen(filename, "a");
  fprintf(file, "%s", text);
  fclose(file);
}

void encrypt_files(const char* directory, int shift) {
  char command[256];
  snprintf(command, sizeof(command), "ls %s", directory);
  FILE* pipe = popen(command, "r");
  if (pipe != NULL) {
    char file_path[256];
    while (fgets(file_path, sizeof(file_path), pipe) != NULL) {
      char full_path[256] = "";
      file_path[strcspn(file_path, "\n")] =
          '\0';  // поиск вхождения \n и замена на \0
      strcat(full_path, directory);
      strcat(full_path, file_path);
      char* file_extension = strrchr(file_path, '.');
      if (file_extension != NULL && (strcmp(file_extension, ".c") == 0 ||
                                     strcmp(file_extension, ".h") == 0)) {
        FILE* file = fopen(full_path, "r+");
        operations(file, file_extension, full_path, shift);
      }
    }
    pclose(pipe);
  }
}

void ceaser(FILE* file, int shift) {
  char c;
  while ((c = fgetc(file)) != EOF) {
    if (c >= 'a' && c <= 'z') {
      c = 'a' + (c - 'a' + shift) % 26;
    } else if (c >= 'A' && c <= 'Z') {
      c = 'A' + (c - 'A' + shift) % 26;
    }
    fseek(file, -1, SEEK_CUR);  // смещение назад на 1 символ
    fputc(c, file);
  }
}

void operations(FILE* file, char* file_extension, char* full_path, int shift) {
  if (file) {
    if (strcmp(file_extension, ".c") == 0) {
      ceaser(file, shift);
    } else if (strcmp(file_extension, ".h") == 0) {
      FILE* file = fopen(full_path, "w");
      if (file == NULL) {
        return;
      }
      fclose(file);
    }
    fclose(file);
  }
}