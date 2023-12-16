#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <signal.h>
#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

bool is_timed_out = false;

void sig_handler(int signo) {
    is_timed_out = true;
    printf("Time's up!\n");
}

int kill_children(pid_t* pids, const size_t size){
    for(size_t i = 0; i < size; i++) {
        kill(pids[i], 9);
    }
    printf("Children genocide commited!\n");
    return 1;
}

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  int timeout = -1;
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"timeout", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            // your code here
            // error handling
            if (seed <= 0)
            {
              printf("Error! Seed must be greater than 0!\n");
              return 1;
            }
            break;
          case 1:
            array_size = atoi(optarg);
            // your code here
            // error handling
            if (array_size <= 0) 
            {
              printf("Error! Size must be greater than 0!\n");
              return 1;
            }
            break;
          case 2:
            pnum = atoi(optarg);
            // your code here
            // error handling
            if (pnum <= 0) 
            {
              printf("Error! Number of processes must be greater than 0!\n");
              return 1;
            }
            break;
           case 3:
            timeout = atoi(optarg);
            if (timeout <= 0) {
                printf("Error! Timeout must be greater than 0!\n");
                return 1;
            }
            break;
          case 4:
            with_files = true;
            break;

          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" --timeout \"num\" \n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;

  int fd[2];
  if (!with_files)
  {
    
    if (pipe(fd) == -1)
    {
      printf("An error ocurred with opening the pipe!\n");
      return 1;
    }
  }
  int part = array_size/pnum;
  struct MinMax local_min_max;

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  pid_t children[pnum];

  //send signal to ourself when the time is out
  if (timeout > 0) {
    signal(SIGALRM, sig_handler);
    alarm(timeout);
  }

  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    children[i] = child_pid;
    if (child_pid >= 0) {
      // successful fork
      active_child_processes += 1;
      if (child_pid == 0) {
        // child process
        // parallel somehow
        local_min_max = GetMinMax(array, i*part, (i == pnum - 1) ? array_size : (i + 1) * part);
        
        if (with_files) {
          // use files here
          FILE* not_pipe_file = fopen("tmp.txt","a");
	        fwrite(&local_min_max, sizeof(struct MinMax), 1, not_pipe_file);
	        fclose(not_pipe_file);
        } else {
          // use pipe here
          if (write(fd[1], &local_min_max, sizeof(struct MinMax)) == -1)
          {
            printf("An error ocurred with writing to the pipe!\n");
            return 1;
          }
        }
        return 0;
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  int status_pid;

  while (active_child_processes > 0) {
      if(is_timed_out)
          return kill_children(children, pnum);
      if (waitpid(-1, &status_pid, WNOHANG) > 0) { //If any child has died
          active_child_processes -= 1;
      }
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;

    if (with_files) {
      // read from files
      FILE* not_pipe_file = fopen("tmp.txt", "rb");
      fseek(not_pipe_file, i*sizeof(struct MinMax), SEEK_SET);
      fread(&local_min_max, sizeof(struct MinMax), 1, not_pipe_file);
      //printf("Process: %i\tMin: %i\tMax: %i\n", i, local_min_max.min, local_min_max.max);
      fclose(not_pipe_file);
    } else {
      // read from pipes
      read(fd[0], &local_min_max, sizeof(struct MinMax));
      //printf("Process: %i\tMin: %i\tMax: %i\n", i, local_min_max.min, local_min_max.max);
    }

    if (local_min_max.min < min_max.min) min_max.min = local_min_max.min;
    if (local_min_max.max > min_max.max) min_max.max = local_min_max.max;
  }

  if (with_files)
  {
    remove("tmp.txt");
  }
  else
  {
    close(fd[0]);
    close(fd[1]);
  }
  
  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  //printf("\n");
  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  
  fflush(NULL);
  return 0;
}
