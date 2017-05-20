#include "bfc.h"

int main(int argc, char** argv) {

  // Check argument count
  if (argc <= 1) {
    fprintf(stderr, "Missing filename\n");
    return 1;
  }

  // Open the file
  FILE* fp;
  if (!(fp = fopen(argv[1], "r"))) {
    fprintf(stderr, "Could not open file: %s\n", argv[1]);
    return 1;
  }

  struct stat inputStat;
  if (fstat(fileno(fp), &inputStat) < 0) {
    fprintf(stderr, "Could not stat file %s\n", argv[1]);
    return 1;
  }

  char* buffer = malloc(inputStat.st_size);
  fread(buffer, inputStat.st_size, 1, fp);

  printf("Program:\n");
  for (int i = 0; i < inputStat.st_size; i++) {
    printf("%c", buffer[i]);
  }
  printf("\n");

  free(buffer);
  fclose(fp);

  return 0;
}
