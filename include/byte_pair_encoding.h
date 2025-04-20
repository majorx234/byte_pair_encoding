#ifndef BYTE_PAIR_ENDOING_H_
#define BYTE_PAIR_ENDOING_H_
#include <stdint.h>
#include <stdlib.h>

typedef struct Pair {
  uint32_t l,r;
} Pair;

typedef struct Freq {
  Pair key;
  size_t value;
} Freq;

char *byte_pair_encode(char *text);
bool load_pairs(const char *file_path, Pair** pairs);

#endif // BYTE_PAIR_ENDOING_H_
