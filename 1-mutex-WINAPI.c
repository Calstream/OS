#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <math.h>

#define m 10	// количество интервалосв
#define a 0
#define b 10
#define n 10	// размер массива
#define N_THR 4 // количество потоков

HANDLE * MUT;

void print_arr(double * arr)
{
	for (int i = 0; i < n; i++)
		printf("%f ", arr[i]);
}

void print_arr_i(int * arr)
{
	for (int i = 0; i < n; i++)
		printf("%d ", arr[i]);
}

typedef struct aux
{
	double * arr;
	int * cnt;
	int beg;
	int step;
}aux, *paux;

DWORD WINAPI thread_proc(LPVOID Param)
{
	paux data = (paux)Param;
	int beg = data->beg;
	int end = beg + data->step;
	if (end > n)
		end = n;
	for (size_t i = beg; i < end; i++)
	{
		double x = data->arr[i];
		int inv = get_interval(x);
		DWORD wait_res = WaitForSingleObject(MUT[inv], INFINITE);
		data->cnt[inv]++;
		//printf("\nthread %d, i = %d, x = %f\n",beg,i, data->arr[i]);
		ReleaseMutex(MUT[inv]);
	}
}

int get_interval(double x)
{
	double step = (b - a) / m;
	double t = x / step;
	int ft = (int)floor(t);
	return ft;
}

//вспомогательная структура данных для передачи массива и индексов в поток


int main()
{
	//объявление и инициализация массива А
	double * A;
	A = malloc(n * sizeof(double));
	time_t t;
	srand((unsigned)time(&t));
	for (int i = 0; i < n; i++)
	{
		double x;
		x = (double)rand() / ((double)RAND_MAX);
		A[i] = a + x * (b - a);
	}

	//шаг
	double h = (b - a) / m;

	//массив счетчиков
	int * counters;
	counters = calloc(m, sizeof(int));


	//массив мьютексов
	MUT = malloc(m * sizeof(HANDLE));
	for (int i = 0; i < m; i++)
	{
		MUT[i] = CreateMutex(NULL, FALSE, NULL);
	}

	//DWORD wait_res = WaitForSingleObject(MUT[0], INFINITE);
	print_arr(A);
	//int * intv;
	//intv = malloc(n * sizeof(int));
	//for (size_t i = 0; i < n; i++)
	//{
	//	intv[i] = get_interval(A[i]);
	//	//printf("\n A[%d] = %f: int = %d",i,A[i],in);
	//}

	paux Data_arr[N_THR];

	int arr_b;
	if (n % N_THR == 0)
		arr_b = n / N_THR;
	else arr_b = n / N_THR + 1;
	
	for (int i = 0; i < N_THR; i++)
	{
		Data_arr[i] = (paux)malloc(N_THR * sizeof(aux));
		Data_arr[i]->arr = A;
		Data_arr[i]->cnt = counters;
		Data_arr[i]->beg = i * arr_b;
		Data_arr[i]->step = arr_b; //!
	}

	//print_arr_i(Data_arr[0]->cnt);

	HANDLE thread_handle_array[N_THR];

	for (int c = 0; c < N_THR; ++c)
	{

		thread_handle_array[c] = CreateThread(
			NULL,
			0,
			thread_proc,
			Data_arr[c],  
			0,
			NULL
		);
	}

	WaitForMultipleObjects(N_THR, thread_handle_array, TRUE, INFINITE);
	for (int c = 0; c < N_THR; c++)
		CloseHandle(thread_handle_array[c]);
	printf("\n");
	print_arr_i(counters);

	getchar();
}
































/*

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define WIDTH 2048       
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

	/*  allocate   
	src = malloc(WIDTH * sizeof(int*));
	dest = malloc(WIDTH * sizeof(int*));
	for (i = 0; i < WIDTH; i++) {
		src[i] = malloc(HEIGHT * sizeof(int));
		dest[i] = malloc(HEIGHT * sizeof(int));
	}

	/*  init src  
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
	Data.sc = 0;

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

Contact GitHub API Training Shop Blog About

© 2017 GitHub, Inc.Terms Privacy Security Status Help


*/