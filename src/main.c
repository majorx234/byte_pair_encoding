#include <string.h>
#include <stdio.h>
#include "byte_pair_encoding.h"

int main(int argc, char **argv) {
  if(argc !=2) {
    printf("usage: byte_par_encode \"some text to encode\"");
  }
  byte_pair_encode(argv[1]);
  return 0;
}
