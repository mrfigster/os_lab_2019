#include <stdint.h>

#include <pthread.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>
#include <getopt.h>

#include "utils.h"
#include "lib_sum.h"

void *ThreadSum(void *args) {
  struct SumArgs *sum_args = (struct SumArgs *)args;
  return (void *)(size_t)Sum(sum_args);
}

int main(int argc, char **argv) {
  /*
   *  TODO:
   *  threads_num by command line arguments
   *  array_size by command line arguments
   *	seed by command line arguments
   */
  static struct option options[] = {{"seed",        required_argument, 0, 0},
                                      {"array_size",  required_argument, 0, 0},
                                      {"threads_num", required_argument, 0, 0},
                                      {0, 0,                             0, 0}};


  uint32_t threads_num = 0;
  uint32_t array_size = 0;
  uint32_t seed = 0;
  uint32_t threads_amount = 0;

  /*
   * TODO:
   * your code here
   * Generate array here
   */

  while (true) {
    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);
    if (c == -1)
      break;
    
    switch (c) {
      case 0: {
        switch (option_index) {
          case 0: 
            seed = atoi(optarg);
            if (seed < 0) {
              printf("Error! Seed must be greater than 0!\n");
              return -1;
            }
            break;
          
          case 1:
            array_size = atoi(optarg);
            if (array_size < 0) {
              printf("Error! Size must be greater than 0!\n");
              return -1;
            }
            break;
          case 2: 
            threads_num = atoi(optarg);
            if (threads_num <= 0) {
              printf("Error! Number of threads must be greater than 0!\n");
              return -1;
            }
            threads_amount = threads_num;
            break;
          
        }
        break;
      }
      case '?':
        break;
      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (seed == 0 || array_size == 0 || threads_num == 0) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --threads_num \"num\" \n",
           argv[0]);
    return 1;
  }

  pthread_t threads[threads_num];

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);

  struct timeval start_time;
  gettimeofday(&start_time, NULL);
  struct SumArgs args[threads_num];
  args[0].begin = 0;

    for (uint32_t i = 0; i < threads_num; i++) {
        args[i].array = array;
        if (i != 0)
            args[i].begin = args[i - 1].end;
        if (args[i].begin == array_size)
            break;
        if (i != threads_num - 1) {
            args[i].end = (i + 1) * array_size / threads_num;
            if (args[i].end > array_size) {
                printf("%d %d\n", args[i].begin, args[i].end);
                printf("SHIT!");
                return -1;
            }
        } else {
            args[i].end = array_size;
        }
        if (pthread_create(&threads[i], NULL, ThreadSum, (void *) (args + i))) {
            printf("Error: pthread_create failed!\n");
            return 1;
        }
    }

    int total_sum = 0;
    for (uint32_t i = 0; i < threads_num; i++) {
      int sum = 0;
      pthread_join(threads[i], (void **)&sum);
      total_sum += sum;
    }

    struct timeval finish_time;
    gettimeofday(&finish_time, NULL);

    double elapsed_time = (finish_time.tv_usec - start_time.tv_usec) / 1000.0;
    free(array);
    printf("Total: %d\n", total_sum);
    printf("It took %f milliseconds to find a sum for %d threads\n", elapsed_time, threads_amount);
    return 0;
}