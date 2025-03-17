#include "byte_pair_encoding.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

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

bool write_entire_file(const char *path, const void *data, size_t size)
{
    bool result = true;

    FILE *f = fopen(path, "wb");
    if (f == NULL) {
        goto defer;
    }

    const char *buf = data;
    while (size > 0) {
        size_t n = fwrite(buf, 1, size, f);
        if (ferror(f)) {
            goto defer;
        }
        size -= n;
        buf  += n;
    }

defer:
    if (f) fclose(f);
    return result;
}

bool dump_pairs(const char *file_path, Pair* pairs) {
  return write_entire_file(file_path, pairs, arrlen(pairs));
}

void generate_dot(const char *file_path, Pair *pairs) {
  // TODO write into buffer and dump to file
  printf("digraph Pairs {\n");
  for (uint32_t token = 0; token < arrlen(pairs); token++ ){
    if (token != pairs[token].l) {
      printf("  %u -> %u\n", token, pairs[token].l);
      printf("  %u -> %u\n", token, pairs[token].r);
    }
  }
  printf("}\n");
}

char *byte_pair_encode(char *text) {
  u_int32_t *vec_tokens_in = NULL;
  size_t text_size = strlen(text);
  Pair *pairs_lookup_table = NULL;
  // generate basic lookup table
  for (size_t i = 0; i < 256; i++) {
    arrput(pairs_lookup_table, ((Pair){.l = i, .r = 0}));
  }

  for (int i = 0; i < text_size; i++) {
    arrput(vec_tokens_in, text[i]);
  }

  int freq_map_length = 0;
  while (arrlen(vec_tokens_in) > 1) {
    Freq *freq_map = NULL;
    for (int i = 0; i < arrlen(vec_tokens_in) - 1; i++) {
      Pair pair = {.l = vec_tokens_in[i], .r = vec_tokens_in[i + 1]};
      ptrdiff_t index = hmgeti(freq_map, pair);
      if (index < 0)
        hmput(freq_map, pair, 1);
      else
        freq_map[index].value++;
    }

    freq_map_length = hmlen(freq_map);
    // find maximum
    ptrdiff_t max_freq_index = 0;
    for (size_t i = 1; i < freq_map_length; i++) {
      if (freq_map[i].value > freq_map[max_freq_index].value) {
        max_freq_index = i;
      }
    }
    // check for end condition
    if (freq_map[max_freq_index].value <= 1)
      break;
    // add pair with most appearance to pairlookuptable
    arrput(pairs_lookup_table, freq_map[max_freq_index].key);
    // adjust reorder tokens
    uint32_t *vec_tokens_out = NULL;
    for (size_t i = 0; i < arrlen(vec_tokens_in); i++) {
      if (i + 1 > arrlen(vec_tokens_in)) {
        arrput(vec_tokens_out, vec_tokens_in[i]);
      } else {
        Pair pair = {.l = vec_tokens_in[i], .r = vec_tokens_in[i + 1]};
        if (memcmp(&pair, &freq_map[max_freq_index], sizeof(pair)) == 0) {
          arrput(vec_tokens_out, arrlen(pairs_lookup_table) - 1);
          i += 1; // skip next token
        } else {
          arrput(vec_tokens_out, vec_tokens_in[i]);
        }
      }
    }
    generate_dot("test.dot", pairs_lookup_table);
    dump_pairs("test.dat", pairs_lookup_table);
    arrswap(&vec_tokens_in, &vec_tokens_out);
    arrfree(vec_tokens_out);

    hmfree(freq_map);
  }

  printf("byte pair encode finished with %d length of hashmap\n",
         freq_map_length);
  print_tokens(pairs_lookup_table, vec_tokens_in);

  arrfree(pairs_lookup_table);
  arrfree(vec_tokens_in);
  // uses ds dynamic array
  }
