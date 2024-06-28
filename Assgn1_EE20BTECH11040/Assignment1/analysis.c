#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

// Function to check if a number is a tetrahedral number
int is_tetrahedral(int num) {
  int p = 1, tetrahedral;
  while (1) {
    tetrahedral = (p * (p + 1) * (p + 2)) / 6;
    if (tetrahedral == num)
      return 1; // Number is tetrahedral
    else if (tetrahedral > num)
      return 0; // Number is not tetrahedral
    p++;
  }
}

int main() {
  // FILE *input_file = fopen("input.txt", "r");
  // if (!input_file) {
  //     printf("Error opening input file.");
  //     return 0;
  // }

  int N = 1000000;
  // fscanf(input_file, "%d %d", &N, &K);
  // fclose(input_file);
  clock_t start_time, end_time;
  double execution_time;
  for (int n = 0; n <= 4; n++) {
    int temp = 3 * n;
    int K = 1 << n;
    if (N <= 0 || K <= 0) {
      printf("Invalid Input");
      return 0;
    }

    int rows = K;
    int columns;
    if (N % K == 0)
      columns = N / K;
    else
      columns = (N / K) + 1;
    start_time = clock();
    int **division = (int **)malloc(rows * sizeof(int *));
    if (division == NULL) {
      printf("Memory allocation failed.\n");
      return 1;
    }
    for (int i = 0; i < rows; i++) {
      division[i] = (int *)malloc(columns * sizeof(int));
      if (division[i] == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
      }
    }

    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < columns; j++) {
        int number = i + (j * rows) + 1;
        if (number > N)
          division[i][j] = -1; // Number is not present
        else
          division[i][j] = number;
      }
    }

    int shm_fd1, shm_fd2;
    int size1 = 4096, size2 = 4;
    int *ptr1, *ptr2;
    char name1[] = "SharedMemory1", name2[] = "SharedMemory2";

    shm_fd1 = shm_open(name1, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd1, size1);
    ptr1 = mmap(0, size1, PROT_WRITE, MAP_SHARED, shm_fd1, 0);

    shm_fd2 = shm_open(name2, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd2, size2);
    ptr2 = mmap(0, size2, PROT_WRITE, MAP_SHARED, shm_fd2, 0);
    ptr2[0] = 0;

    pid_t pid;
    for (int process = 0; process < K; process++) {
      pid = fork();
      if (pid < 0) {
        printf("Error while Forking \n");
        return 0;
      } else if (pid == 0) {
        char filename[20];
        sprintf(filename, "OutFile%d.txt", process + 1);
        FILE *f_child = fopen(filename, "w");
        fprintf(f_child, "Process Number %d\n\n", process + 1);

        int shm_fd1_child = shm_open(name1, O_RDWR, 0666);
        ftruncate(shm_fd1_child, size1);
        int *ptr1_child =
            mmap(0, size1, PROT_WRITE, MAP_SHARED, shm_fd1_child, 0);

        int shm_fd2_child = shm_open(name2, O_RDWR, 0666);
        ftruncate(shm_fd2_child, size2);
        int *ptr2_child =
            mmap(0, size2, PROT_WRITE, MAP_SHARED, shm_fd2_child, 0);

        for (int i = 0; i < columns; i++) {
          int num = division[process][i];
          if (num > 0) {
            if (is_tetrahedral(num)) {
              fprintf(f_child, "%d: Is a tetrahedral number\n", num);
              ptr1_child[ptr2_child[0]] = num;
              ptr1_child[ptr2_child[0] + 1] =
                  getpid(); // Store child process ID
              ptr2_child[0] += 2;
            } else {
              fprintf(f_child, "%d: Not a tetrahedral number\n", num);
            }
          }
        }
        fclose(f_child);
        exit(0);
      }
    }

    for (int i = 0; i < K; i++) {
      wait(NULL);
    }

    FILE *f_main = fopen("OutMain.txt", "w");
    fprintf(f_main, "Tetrahedral Numbers identified between 1 and %d:\n\n", N);

    for (int i = 0; i < ptr2[0]; i += 2) {
      if (ptr1[i + 1] != 0) {
        fprintf(f_main, "Process %d : %d\n", ptr1[i + 1], ptr1[i]);
      }
    }
    fclose(f_main);
    for (int i = 0; i < rows; i++) {
      free(division[i]);
    }
    free(division);
    end_time = clock();
    execution_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    execution_time = execution_time * 1000;
    printf("For K = %d, Execution Time: %lf milli seconds\n", K,
           execution_time);
  }
  return 0;
}
