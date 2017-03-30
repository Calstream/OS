/* 
2048/2
0.977
0.889
0.966
0.957
0.959

2048/4
0.761
0.807
0.811
0.956
0.818

2048/8
0.777
0.788
0.787
0.845
0.798
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define WIDTH 2048       /* array sizes */
#define HEIGHT 2048
#define N_THR 4 // threads

int global_counter = 0;
DWORD WINAPI ex(LPVOID Param)
{
	int ari = (int)Param;
	printf("ari");
	printf("%d ", ari);
	//LPVOID myp = Param;
}

typedef struct aux
{
	int sc;
	int** src;
	int** dst;
}aux, *paux;

DWORD WINAPI GOL_step(LPVOID Param)
{
	paux data = (paux)Param;
	int** dest = data->dst;
	int** src = data->src;
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

	/*  allocate   */
	src = malloc(WIDTH * sizeof(int*));
	dest = malloc(WIDTH * sizeof(int*));
	for (i = 0; i < WIDTH; i++) {
		src[i] = malloc(HEIGHT * sizeof(int));
		dest[i] = malloc(HEIGHT * sizeof(int));
	}

	/*  init src  */
	time_t t;
	srand((unsigned)time(&t));
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

	/*aux Data;
	Data.src = src;
	Data.dst = dest;
	Data.sc = 0;*/

	int step = WIDTH / N_THR;



	HANDLE thread_handle_array[N_THR];
	int c;

	paux Data_arr[N_THR];
	
	for (int i = 0; i < N_THR; i++)
	{
		Data_arr[i] = (paux)malloc(N_THR * sizeof(aux));
		if (Data_arr[i] == NULL)
			ExitProcess(2);
		Data_arr[i]->src = src;
		Data_arr[i]->dst = dest;
		Data_arr[i]->sc = i * step;
	}


	for (c = 0; c < N_THR; ++c)
	{
		
		thread_handle_array[c] = CreateThread(
			NULL,
			0,
			GOL_step,
			Data_arr[c], // TBC! startcol, ?dest
			0,
			NULL
		);
	}

	

	WaitForMultipleObjects(N_THR, thread_handle_array, TRUE, INFINITE);
	for (int ñ = 0; ñ < N_THR; ñ++)
		CloseHandle(thread_handle_array[ñ]);
	
	//print_m(dest);
	clock_t end = clock();
	float seconds = (float)(end - start) / CLOCKS_PER_SEC;
	printf("%f", seconds);
	getchar();
}
