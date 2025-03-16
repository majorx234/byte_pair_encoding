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

void print_tokens(Pair* pairs, uint32_t *tokens) {
  for (size_t i = 0; i < arrlen(tokens); i++) {
    assert( tokens[i] < arrlen(pairs));
    if (pairs[tokens[i]].l == tokens[i]) {
      printf("%c", tokens[i]);
    } else {
      printf("[%u]", tokens[i]);
    }
  }
  printf("\n");
}

char *byte_pair_encode(char *text) {
  Freq* freq_map = NULL;
  u_int32_t* vec_tokens_in = NULL;
  size_t text_size = strlen(text);
  for (int i = 0; i < text_size; i++) {
    arrput(vec_tokens_in,text[i]);
  }
  for (int i = 0; i < arrlen(vec_tokens_in) - 1; i++) {
    Pair pair = {.l = vec_tokens_in[i],
                 .r = vec_tokens_in[i+1]
    };
    ptrdiff_t index = hmgeti(freq_map, pair);
    if (index < 0) hmput(freq_map, pair, 1);
    else freq_map[index].value++;
  }

  int freq_map_length = hmlen(freq_map);

  printf("byte pair encode with %d length of hashmap\n",freq_map_length);

  // just for printing
  /*
  Freq* vec_freq_sorted = NULL;
  for (size_t i = 0; i < freq_map_length; i++) {
    arrput(vec_freq_sorted, freq_map[i]);
  }
  size_t vec_freq_sorted_length = arrlenu(vec_freq_sorted);
  qsort(vec_freq_sorted, vec_freq_sorted_length, sizeof(*vec_freq_sorted), compare_freqs);
  for (size_t i = 0; i < vec_freq_sorted_length; i++) {
    printf("key: (%u,%u), value %lu\n", vec_freq_sorted[i].key.l,vec_freq_sorted[i].key.r , vec_freq_sorted[i].value);
  }
  arrfree(vec_freq_sorted);
  */

  // generate basic lookup table
  Pair* pairs_lookup_table = NULL;
  for (size_t i = 0; i < 256; i++) {
    arrput(pairs_lookup_table, ((Pair){
      .l = i,
      .r = 0
      }));
  }

  // find maximum
  ptrdiff_t max_freq_index = 0;
  for (size_t i = 1; i < freq_map_length; i++) {
    if (freq_map[i].value > freq_map[max_freq_index].value) {
      max_freq_index = i;
    }
  }
  // add pair with most appearance to pairlookuptable
  arrput(pairs_lookup_table, freq_map[max_freq_index].key);
  // adjust reorder tokens
  uint32_t* vec_tokens_out = NULL;
  for (size_t i = 0; i < arrlen(vec_tokens_in); i++) {
    if (i + 1 > arrlen(vec_tokens_in)) {
      arrput(vec_tokens_out, vec_tokens_in[i]);
    } else {
      Pair pair = {
        .l = vec_tokens_in[i],
        .r = vec_tokens_in[i+1]
      };
      if (memcmp(&pair, &freq_map[max_freq_index], sizeof(pair)) == 0) {
        arrput(vec_tokens_out, arrlen(pairs_lookup_table) -1);
        i += 1; // skip next token
      } else {
        arrput(vec_tokens_out,vec_tokens_in[i]);
      }
    }
  }

  size_t vec_tokens_in_length = arrlenu(vec_tokens_in);
  for (size_t i = 0; i < vec_tokens_in_length; i++) {
    printf("token_in: %d\n", vec_tokens_in[i]);
  }
  size_t vec_tokens_out_length = arrlenu(vec_tokens_out);
  for (size_t i = 0; i < vec_tokens_out_length; i++) {
    printf("token_out: %d\n", vec_tokens_out[i]);
  }
  print_tokens(pairs_lookup_table, vec_tokens_in);
  print_tokens(pairs_lookup_table, vec_tokens_out);

  arrfree(vec_tokens_in);
  arrfree(vec_tokens_out);
  hmfree(freq_map);
  // uses ds dynamic array
  }
