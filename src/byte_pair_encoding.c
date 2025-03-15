#include "byte_pair_encoding.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

typedef struct Pair {
  uint32_t l,r;
} Pair;

typedef struct Freq {
  Pair key;
  size_t value;
} Freq;

int compare_freqs(const void* a_raw, const void* b_raw) {
  const Freq* a = a_raw;
  const Freq* b = b_raw;
  return(int)b->value - (int)a->value;
}

char *byte_pair_encode(char *text) {
  Freq* freq_map = NULL;
  u_int32_t* vec_tokens = NULL;
  size_t text_size = strlen(text);
  for (int i = 0; i < text_size; i++) {
    arrput(vec_tokens,text[i]);
  }
  for (int i = 0; i < arrlen(vec_tokens) - 1; i++) {
    Pair pair = {.l = vec_tokens[i],
                 .r = vec_tokens[i+1]
    };
    ptrdiff_t index = hmgeti(freq_map, pair);
    if (index < 0) hmput(freq_map, pair, 1);
    else freq_map[index].value++;
  }

  int freq_map_length = hmlen(freq_map);
  Freq* vec_freq_sorted = NULL;

  printf("byte pair encode with %d length of hashmap\n",freq_map_length);
  for (size_t i = 0; i < freq_map_length; i++) {
    arrput(vec_freq_sorted, freq_map[i]);
  }

  size_t vec_freq_sorted_length = arrlenu(vec_freq_sorted);
  qsort(vec_freq_sorted, vec_freq_sorted_length, sizeof(*vec_freq_sorted), compare_freqs);

  for (size_t i = 0; i < vec_freq_sorted_length; i++) {
    printf("key: (%u,%u), value %lu\n", vec_freq_sorted[i].key.l,vec_freq_sorted[i].key.r , vec_freq_sorted[i].value);
  }
  arrfree(vec_tokens);
  arrfree(vec_freq_sorted);
  hmfree(freq_map);
  // uses ds dynamic array
}
