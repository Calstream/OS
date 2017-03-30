#include <stdio.h>
#include <stdlib.h>
//#include <stddef.h>
#include <time.h>
//#include <string.h>
#include <pthread.h>

#define WIDTH 1625       /* array sizes */
#define HEIGHT 1625
#define N_THR 5

typedef struct aux
{
	int sc;
	int** src;
	int** dst;
}aux, *paux;

void* GOL_step(void* Param)
{
	paux data = (paux)Param;
	int** dest = data->dst;
	int** src = data->src;
	//printf("%d ", data->sc);
	int neighbours = 0;
	int ySHFT = 0;
	int xSHFT = 0;
	int i, j;
	int step = WIDTH / N_THR;
	for (i = data->sc; i < data->sc + step; i++)
	{
		for (j = 0; j < HEIGHT; j++)
		{
			neighbours = 0;
			for (ySHFT = -1; ySHFT <= 1; ySHFT++)
			{
				for (xSHFT = -1; xSHFT <= 1; xSHFT++)
				{
					if (xSHFT == 0 && ySHFT == 0)
						continue;
					int ni = i + ySHFT;
					if (ni == -1)
						ni += HEIGHT;
					int nj = j + xSHFT;
					if (nj == -1)
						nj += WIDTH;
					if (src[ni % HEIGHT][nj % WIDTH] == 1)
					{
						neighbours++;
					}
				}
			}
			if (src[i][j] == 1)
			{
				if (neighbours < 2 || neighbours > 3)
					dest[i][j] = 0;
				else
					dest[i][j] = 1;
			}
			else if (src[i][j] == 0)
			{
				if (neighbours == 3)
					dest[i][j] = 1;
				else
					dest[i][j] = 0;
			}
		}
	}
}



//print matrix
void * print_m(int** tbp)
{
	int i, j;
	for (i = 0; i < WIDTH; i++)
	{
		for (j = 0; j < HEIGHT; j++)
		{
			printf("%d ", tbp[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}



int main()
{
	clock_t start = clock();

	int i, j; //counters
	int **src;
	int **dest;

	/* allocate arrays */
	src = malloc(WIDTH * sizeof(int*));
	dest = malloc(WIDTH * sizeof(int*));
	for (i = 0; i < WIDTH; i++) {
		src[i] = malloc(HEIGHT * sizeof(int));
		dest[i] = malloc(HEIGHT * sizeof(int));
	}

	/*  initialize elements of src to 0 or 1 */
	time_t t;
  srand((unsigned) time(&t));
	float x;
	for (i = 0; i < WIDTH; i++)
	{
		for (j = 0; j < HEIGHT; j++)
		{
			x = rand() / ((float)RAND_MAX + 1);
			if (x<0.5)
				src[i][j] = 0;
			else
				src[i][j] = 1;
		}
	}

	//print_m(src);

	int step = WIDTH / N_THR;

  pthread_t thread_handle_array[N_THR];
	int c;

	paux Data_arr[N_THR];
    i = 0;
	for (i = 0; i < N_THR; i++)
	{
		Data_arr[i] = (paux)malloc(N_THR * sizeof(aux));
		Data_arr[i]->src = src;
		Data_arr[i]->dst = dest;
		Data_arr[i]->sc = i * step;
	}

	for (c = 0; c < N_THR; ++c)
	{
    pthread_create(
    &thread_handle_array[c],
    NULL,
    GOL_step,
    Data_arr[c]
    );
	}
    int k = 0;
	for (k = 0; k < N_THR; k++)
    pthread_join(thread_handle_array[k], NULL);
	//print_m(dest);
	clock_t end = clock();
	float seconds = (float)(end - start) / CLOCKS_PER_SEC;
	printf("%f", seconds);
	printf("\n");
}
