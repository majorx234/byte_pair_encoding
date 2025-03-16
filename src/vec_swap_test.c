#include <string.h>
#include <stdio.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

typedef struct TestStruct {
  unsigned int member1;
  float member2;
} TestStruct;

int main(int argc, char **argv) {
  TestStruct* first = NULL;
  TestStruct* second = NULL;

  for (size_t i = 0; i < 20; i++) {
    arrput(first,((TestStruct){
        .member1 = i,
        .member2 = i*10.0
        }));
  }

  for (size_t i = 20; i < 100; i++) {
    arrput(second,((TestStruct){
        .member1 = i,
        .member2 = i*20.0
      }));
  }

  size_t first_length = arrlenu(first);
  for (size_t i = 0; i < first_length; i++) {
    printf("first: (%u,%f)\n", first[i].member1, first[i].member2);
  }
  size_t second_length = arrlenu(second);
  for (size_t i = 0; i < second_length; i++) {
    printf("second: (%u,%f)\n", second[i].member1, second[i].member2);
  }

  printf("======== swap vecs =======\n");
  arrswap(&first, &second);
  first_length = arrlenu(first);
  for (size_t i = 0; i < first_length; i++) {
    printf("first: (%u,%f)\n", first[i].member1, first[i].member2);
  }
  second_length = arrlenu(second);
  for (size_t i = 0; i < second_length; i++) {
    printf("second: (%u,%f)\n", second[i].member1, second[i].member2);
  }

  arrfree(first);
  arrfree(second);
  return 0;
}
