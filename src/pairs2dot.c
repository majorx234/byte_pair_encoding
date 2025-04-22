#include <string.h>
#include <stdio.h>
#include "byte_pair_encoding.h"

int main(int argc, char **argv) {
  if (argc <= 1) {
    printf("usage: pairs2dot /path/to/pairs /path/to/dot");
    fprintf(stderr, "ERROR: no input is provided\n");
    return 1;
  }
  const char* input_file_path = argv[1];
  if (argc == 2) {
    printf("usage: pairs2dot /path/to/pairs /path/to/dot");
    fprintf(stderr, "ERROR: no output is provided\n");
    return 1;
  }
  const char* output_file_path = argv[2];
  Pair *pairs = NULL;
  if(!load_pairs(input_file_path, &pairs)) return 1;
  generate_dot(output_file_path, pairs);
  return 0;
}

