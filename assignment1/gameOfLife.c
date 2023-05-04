#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define SIZE 50
#define ONE_THREAD 1
#define EIGHT_THREADS 8

//count live neighbors of a given cell
int countLiveNeighbors(int grid[][SIZE], int row, int col) {
    int count = 0;
    for (int i = row - 1; i <= row + 1; i++) {
        for (int j = col - 1; j <= col + 1; j++) {
            if (i >= 0 && i < SIZE && j >= 0 && j < SIZE && grid[i][j] == 1) {
                count++;
            }
        }
    }
    if (grid[row][col] == 1) {
        count--;
    }
    return count;
}

//update the grid
void updateGrid(int grid[][SIZE], int num_threads) {
    #pragma omp parallel num_threads(num_threads)
    {
        int thread_id = omp_get_thread_num();
        int chunk_size = SIZE / num_threads;
        int start_row = thread_id * chunk_size;
        int end_row = start_row + chunk_size - 1;
        if (thread_id == num_threads - 1) {
            end_row = SIZE - 1;
        }
        for (int i = start_row; i <= end_row; i++) {
            for (int j = 0; j < SIZE; j++) {
                int live_neighbors = countLiveNeighbors(grid, i, j);
                if (grid[i][j] == 1 && (live_neighbors < 2 || live_neighbors > 3)) {
                    grid[i][j] = 0;
                }
                else if (grid[i][j] == 0 && live_neighbors == 3) {
                    grid[i][j] = 1;
                }
            }
        }
    }
}

//display the grid
void displayGrid(int grid[][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf("%c", grid[i][j] ? '*' : ' ');
        }
        printf("\n");
    }
    printf("\n");
}

void threadRuntime(int grid[][SIZE], int threadSize) {
    clock_t start, end;
    start = clock();
    // Display the starting state of the grid
    printf("Starting state of the grid:\n");
    displayGrid(grid);

    // Update the grid for a reasonable number of iterations
    int num_iterations = 500;
    for (int iter = 1; iter <= num_iterations; iter++) {
        updateGrid(grid, threadSize);
    }
    end = clock();
    
    // Display the final state of the grid
    printf("Final state of the grid:\n");
    displayGrid(grid);
    printf("Total time taken: %f\n", (double)(end-start)/CLOCKS_PER_SEC);
}

int main() {
    // Seed the random number generator
    srand(time(NULL));

    // Initialize the grid with random values
    int fgrid[SIZE][SIZE];
    int sgrid[SIZE][SIZE];
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            int val = rand() % 2;
            fgrid[i][j] = val;
            sgrid[i][j] = val;
        }
    }
    //using game of life algo for one thread
    printf("Runtime for %d thread in use\n", ONE_THREAD);
    threadRuntime(fgrid, ONE_THREAD);
    printf("\n");

    //using same algo but running in eight threads
    printf("Runtime for %d threads in use\n", EIGHT_THREADS);
    threadRuntime(sgrid, EIGHT_THREADS);
    printf("\n");

    return 0;
}