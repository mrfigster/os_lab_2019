#include <stdint.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int factorial_value = 1;

typedef struct {
  int begin;
  int end;
} FactorialPart;

void *calculate_factorial(void *args) {

  FactorialPart *f_part = (FactorialPart *) args;
  int res = 1;

  for (int i = f_part->begin; i < f_part->end; i++)
    res *= (i + 1);

  pthread_mutex_lock(&mutex);
  factorial_value *= res;
  pthread_mutex_unlock(&mutex);

}

int main(int argc, char **argv) {

  uint32_t k = -1;
  uint32_t threads_num = -1;
  uint32_t mod = -1;

  while (1) {
    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "?", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            k = atoi(optarg);
            if (k <= 0) {
              printf("Number must be positive\n");
              return 1;
            }
            break;
          case 1:
            threads_num = atoi(optarg);
            if (threads_num <= 0) {
              printf("Threads number must be positive\n");
              return 1;
            }
            break;
          case 2:
            mod = atoi(optarg);
            if (mod <= 0) {
              printf("Mod number must be positive\n");
              return 1;
            }
            break;
        }
        break;

      case '?':break;

      default:printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (k == -1 || threads_num == -1 || mod == -1) {
    printf("Usage: %s --k \"num\" --pnum \"num\" --mod \"num\" \n",
           argv[0]);
    return 1;
  }

  
  int i = 0, beg = 0;
  pthread_t threads[threads_num];
  FactorialPart factorial_parts[threads_num];

  while (beg < k)
  {
    int step = k > threads_num ? (k - beg) / (threads_num - i) : 1;
    factorial_parts[i].begin = beg;
    factorial_parts[i].end = (beg + step >= k) ? k: beg + step;
    if (pthread_create(&threads[i], NULL, calculate_factorial, (void *) &factorial_parts[i])) {
      perror("\nError! Can't create tread!\n");
      return 1;
    }
    beg += step;
    i++;
  }


  for (uint32_t t = i - 1; t > 0; t--) {
    pthread_join(threads[t], NULL);
  }
  printf("Factorial (without mod): %i\n", factorial_value);
  printf("Factorial (with mod): %i\n", factorial_value % mod);

  return 0;
}