// Copyright (c) 2015 Tencent Inc.
// Author: Zhuoxiong Zhao (jacksonzhao@tencent.com)
//         Yafei Zhang (kimmyzhang@tencent.com)
//
// 查找与输入的词最相似的Top 100个词
//
// 注意:
//   直接使用word2vec的词典格式, 没有使用text miner的dict.
//   基于word2vec修改, 代码并不符合SPA编码规范:
//     http://word2vec.googlecode.com/svn/trunk/distance.c

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc.h>

#define max_size 2000         // max length of strings
#define N 100                 // number of closest words that will be shown
#define max_w 50              // max length of vocabulary entries

int main(int argc, char **argv) {
  FILE *f;
  char st1[max_size];
  char *bestw[N];
  char file_name[max_size], st[N][max_size];
  float dist, len, bestd[N], vec[max_size];
  long long words, size, a, b, c, d, cn, bi[N];
  float *M;
  char *vocab;

  if (argc < 2) {
    printf("Usage: ./%s <VECTOR.BIN>\n"
           "where VECTOR.BIN is the model trained by word2vec\n",
           argv[0]);
    return 0;
  }

  strncpy(file_name, argv[1], max_size);
  fprintf(stderr, "Loading %s ...\n", file_name);
  f = fopen(file_name, "rb");
  if (f == NULL) {
    printf("Input file not found\n");
    return -1;
  }
  if (fscanf(f, "%lld", &words) != 1) {
    printf("Fail to load variable 'words'\n");
    return -1;
  }
  if (fscanf(f, "%lld", &size) != 1) {
    printf("Fail to load variable 'size'\n");
    return -1;
  }
  vocab = (char *)malloc((long long)words * max_w * sizeof(char));
  if (vocab == NULL) {
    printf("Cannot allocate memory for vocab");
    return -1;
  }
  for (a = 0; a < N; a++) {
    bestw[a] = (char *)malloc(max_size * sizeof(char));
    if (bestw[a] == NULL) {
      printf("Cannot allocate memory for bestw");
      return -1;
    }
  }
  M = (float *)malloc((long long)words * (long long)size * sizeof(float));
  if (M == NULL) {
    printf("Cannot allocate memory: %lld MB    %lld  %lld\n",
           (long long)words * size * sizeof(float) / 1048576, words, size);
    return -1;
  }
  for (b = 0; b < words; b++) {
    a = 0;
    while (1) {
      vocab[b * max_w + a] = fgetc(f);
      if (feof(f) || (vocab[b * max_w + a] == ' ')) break;
      if ((a < max_w) && (vocab[b * max_w + a] != '\n')) a++;
    }
    vocab[b * max_w + a] = 0;
    for (a = 0; a < size; a++) fread(&M[a + b * size], sizeof(float), 1, f);
    len = 0;
    for (a = 0; a < size; a++) len += M[a + b * size] * M[a + b * size];
    len = sqrt(len);
    for (a = 0; a < size; a++) M[a + b * size] /= len;
  }
  fclose(f);
  f = NULL;
  fprintf(stderr, "Done.\n");

  while (1) {
    for (a = 0; a < N; a++) bestd[a] = 0;
    for (a = 0; a < N; a++) bestw[a][0] = 0;
    a = 0;
    while (1) {
      st1[a] = fgetc(stdin);
      if ((int)st1[a] == EOF) {
        free(M);
        M = NULL;
        free(vocab);
        vocab = NULL;
        size_t i;
        for (i = 0; i < N; i++) {
          free(bestw[i]);
          bestw[i] = NULL;
        }
        return 0;
      }
      if ((st1[a] == '\n') || (a >= max_size - 1)) {
        st1[a] = 0;
        break;
      }
      a++;
    }
    if (!strcmp(st1, "EXIT")) break;
    cn = 0;
    b = 0;
    c = 0;
    while (1) {
      st[cn][b] = st1[c];
      b++;
      c++;
      st[cn][b] = 0;
      if (st1[c] == 0) break;
      if (st1[c] == ' ') {
        cn++;
        b = 0;
        c++;
      }
    }
    cn++;
    for (a = 0; a < cn; a++) {
      for (b = 0; b < words; b++) if (!strcmp(&vocab[b * max_w], st[a])) break;
      if (b == words) b = -1;
      bi[a] = b;
      if (b == -1) {
        printf("%s\n%s\n", st1, st1);
        break;
      }
    }
    if (b == -1) continue;
    for (a = 0; a < size; a++) vec[a] = 0;
    for (b = 0; b < cn; b++) {
      if (bi[b] == -1) continue;
      for (a = 0; a < size; a++) vec[a] += M[a + bi[b] * size];
    }
    len = 0;
    for (a = 0; a < size; a++) len += vec[a] * vec[a];
    len = sqrt(len);
    for (a = 0; a < size; a++) vec[a] /= len;
    for (a = 0; a < N; a++) bestd[a] = -1;
    for (a = 0; a < N; a++) bestw[a][0] = 0;
    for (c = 0; c < words; c++) {
      a = 0;
      for (b = 0; b < cn; b++) if (bi[b] == c) a = 1;
      if (a == 1) continue;
      dist = 0;
      for (a = 0; a < size; a++) dist += vec[a] * M[a + c * size];
      for (a = 0; a < N; a++) {
        if (dist > bestd[a]) {
          for (d = N - 1; d > a; d--) {
            bestd[d] = bestd[d - 1];
            strncpy(bestw[d], bestw[d - 1], max_size * sizeof(char));
          }
          bestd[a] = dist;
          strncpy(bestw[a], &vocab[c * max_w], max_size * sizeof(char));
          break;
        }
      }
    }

    printf("Top 100 similar words for \"%s\":\n", st1);
    for (a = 0; a < N; a++) {
      printf("%s\t%10f\n", bestw[a], bestd[a]);
    }
  }
  return 0;
}
