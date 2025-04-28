#include "byte_pair_encoding.h"
#include <bits/pthreadtypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

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

bool read_entire_file(const char *path, void** data, size_t* file_size)
{
  FILE *fp = fopen(path, "rb");
  struct stat finfo;
  int err_stat = fstat(fileno(fp), &finfo);
  *file_size = finfo.st_size;
  *data = malloc(*file_size);
  if (*data == NULL){
    return false;
  }
  int err_read = fread(*data, 1, *file_size, fp);
  fclose(fp);
  return true;
}

bool dump_pairs(const char *file_path, Pair* pairs) {
  return write_entire_file(file_path, pairs, arrlen(pairs)*sizeof(Pair));
}

bool load_pairs(const char *file_path, Pair** pairs){
  size_t file_size = 0;
  char* data = NULL;
  bool ok = read_entire_file(file_path, (void**)&data, &file_size);
  arrsetlen(*pairs, file_size/sizeof(Pair));
  memcpy(*pairs, data, file_size);
  return ok;
}

void generate_dot(const char *file_path, Pair *pairs) {
  // TODO write into buffer and dump to file
  char buffer1[1024];
  char buffer2[1024];
  char* dyn_buffer = NULL;
  int size = sprintf(buffer1, "digraph Pairs {\n");
  arrsetlen(dyn_buffer, size);
  memcpy(dyn_buffer, buffer1, size);
  for (uint32_t token = 0; token < arrlen(pairs); token++ ){
    if (token != pairs[token].l) {
      int size1 = sprintf(buffer1, "  %u -> %u\n", token, pairs[token].l);
      char* where1 = arraddnptr(dyn_buffer, size1);
      memcpy(where1, buffer1, size1);
      int size2 = sprintf(buffer2, "  %u -> %u\n", token, pairs[token].r);
      char* where2 = arraddnptr(dyn_buffer, size2);
      memcpy(where2, buffer2, size2);
    }
  }
  char* where_end = arraddnptr(dyn_buffer, 2);
  sprintf(where_end,"}\n");
  FILE* fb = fopen(file_path, "wb");
  fwrite(dyn_buffer, arrlen(dyn_buffer), 1, fb);
  arrfree(dyn_buffer);
}

char *byte_pair_encode(char *text) {
  u_int32_t *vec_tokens_in = NULL;
  size_t text_size = strlen(text);

  for (int i = 0; i < text_size; i++) {
    arrput(vec_tokens_in, text[i]);
  }

  Pair *pairs_lookup_table = NULL;
  // generate basic lookup table
  for (size_t i = 0; i < 256; i++) {
    arrput(pairs_lookup_table, ((Pair){.l = i, .r = 0}));
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
    arrswap(&vec_tokens_in, &vec_tokens_out);
    arrfree(vec_tokens_out);

    hmfree(freq_map);
  }

  printf("byte pair encode finished with %d length of hashmap\n",
         freq_map_length);
  print_tokens(pairs_lookup_table, vec_tokens_in);

  // print out
  generate_dot("test.dot", pairs_lookup_table);
  dump_pairs("test.dat", pairs_lookup_table);

  arrfree(pairs_lookup_table);
  arrfree(vec_tokens_in);
  // uses ds dynamic array
  return NULL;
}

#define FREQ_COLLECTION_CHUNK_SIZE (16*1024)

#define THREAD_COUNT 8
typedef struct ThreadStuff{
  size_t id;
  Freq** freqs;
  pthread_cond_t collect_freqs;
  pthread_mutex_t* tokens_in_cursor_mutex;
  size_t* tokens_in_cursor;
  char* tokens_in;
} ThreadStuff;

typedef struct ThreadReturnStuff{
  uint32_t ret;
} ThreadReturnStuff;

void parallize_bpe(char* text) {
  Freq* merged_freq = NULL;
  pthread_t thread_hdls[THREAD_COUNT];

  pthread_mutex_t tokens_in_cursor_mutex = {0};
  size_t tokens_in_cursor = 0;
  u_int32_t *vec_tokens_in = NULL;
  size_t text_size = strlen(text);
  Freq* freqs[THREAD_COUNT] = {NULL};
  for (int i = 0; i < text_size; i++) {
    arrput(vec_tokens_in, text[i]);
  }

  for(size_t id = 0; id < THREAD_COUNT; id++) {
    ThreadStuff thread_stuff_template = {
      .id = id,
      .freqs = freqs,
      .collect_freqs = PTHREAD_COND_INITIALIZER,
      .tokens_in_cursor_mutex = &tokens_in_cursor_mutex,
      .tokens_in_cursor = &tokens_in_cursor,
      .tokens_in = NULL,
    };
    ThreadStuff* thread_stuff = malloc(sizeof(ThreadStuff));
    memcpy(thread_stuff, &thread_stuff_template, sizeof(ThreadStuff));
    int err = pthread_create(&thread_hdls[id], NULL, byte_pair_encode_threaded, thread_stuff);
    if (err != 0){
      printf("cannot creat thread: %s\n", strerror(err));
    }
  }
  // join threads and aggregate results
  for(size_t id = 0; id < THREAD_COUNT; id++) {
    ThreadStuff* thread_stuff;
    int err = pthread_join(thread_hdls[id], (void**)&thread_stuff);
    if (err != 0){
      printf("cannot creat thread: %s\n", strerror(err));
    }
    size_t n = hmlen(thread_stuff->freqs[id]);
    for(size_t i = 0; i < n; i++) {
      Pair key = thread_stuff->freqs[id][i].key;
      ptrdiff_t place = hmgeti(merged_freq, key);
      if(place < 0)
        hmputs(merged_freq, thread_stuff->freqs[id][i]);
      else
        merged_freq[place].value += thread_stuff->freqs[id][i].value;
    }
  }
}

void *byte_pair_encode_threaded(void* thread_stuff_raw) {
  ThreadStuff* thread_stuff = (ThreadStuff*)thread_stuff_raw;
  size_t id = thread_stuff->id;

  hmfree(thread_stuff->freqs[id]);
  int tokens_in_count = arrlen(thread_stuff->tokens_in);
  while(true) {
    size_t begin, end = 0;
    pthread_mutex_lock(thread_stuff->tokens_in_cursor_mutex);
    if(*(thread_stuff->tokens_in_cursor) + FREQ_COLLECTION_CHUNK_SIZE <= tokens_in_count){
      begin = *(thread_stuff->tokens_in_cursor);
      *(thread_stuff->tokens_in_cursor) += FREQ_COLLECTION_CHUNK_SIZE;
      end = *(thread_stuff->tokens_in_cursor);
    } else {
      begin = *(thread_stuff->tokens_in_cursor);
      *(thread_stuff->tokens_in_cursor) = tokens_in_count;
      end = *(thread_stuff->tokens_in_cursor);
    }
    pthread_mutex_unlock(thread_stuff->tokens_in_cursor_mutex);
  }
  char *text;
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
    arrswap(&vec_tokens_in, &vec_tokens_out);
    arrfree(vec_tokens_out);

    hmfree(freq_map);
  }

  printf("byte pair encode finished with %d length of hashmap\n",
         freq_map_length);
  print_tokens(pairs_lookup_table, vec_tokens_in);

  // print out
  generate_dot("test.dot", pairs_lookup_table);
  dump_pairs("test.dat", pairs_lookup_table);

  arrfree(pairs_lookup_table);
  arrfree(vec_tokens_in);
  // uses ds dynamic array
  return NULL;
}

void print_pairs(Pair *pairs) {
  if (pairs == NULL){
    return;
  }
  int pair_len = arrlen(pairs);
  if ( pair_len > 0) {
    for (int i = 0; i < pair_len; ++i) {
      printf("pairs[%d] l:%d r:%d\n", i, pairs[i].l, pairs[i].r);
    }
  }
}

void render_token(Pair *pairs, uint32_t token, char **token_out)
{
  if(pairs == NULL)
    return;
  if (token == pairs[token].l) {
    arrput(*token_out, (char)token);
    int arr_last = arrlen(*token_out)-1;
    return;
  }
  render_token(pairs, pairs[token].l, token_out);
  render_token(pairs, pairs[token].r, token_out);
}
