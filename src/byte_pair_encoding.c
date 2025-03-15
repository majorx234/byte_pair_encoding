#include "byte_pair_encoding.h"
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#include "stdio.h"

typedef struct Pair {
  char pair[2];
} Pair;

typedef struct Freq {
  Pair key;
  size_t value;
} Freq;

typedef struct Freqs {
  Freq *items;
  size_t count;
  size_t capacity;
} Freqs;

char *byte_pair_encode(char *text) {
  Freq* freq_map = NULL;
  hmdefault(freq_map, 0);
  size_t text_size = strlen(text);
  for (int i = 0; i < text_size - 1; i++) {
    Pair pair = {.pair = {text[i],text[i+1]} };
    ptrdiff_t index = hmgeti(freq_map, pair);
    if (index < 0) hmput(freq_map, pair, 1);
    else freq_map[index].value++;
  }

  int freq_map_length = hmlen(freq_map);
  printf("byte pair encode with %d length of hashmap\n",freq_map_length);
  for (size_t i = 0; i < freq_map_length; i++) {
    printf("key %.2s, value: %d \n",freq_map[i].key.pair, freq_map[i].value);
  }
}
