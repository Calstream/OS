#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>

/*Задано вещественное число a и натуральное число m.
Массив A размера n заполняется случайными целыми числами в полуинтервале [0, a).
Затем необходимо скопировать все нечётные значения из массива A в список L1,
а все чётные — в список L2. Задачу по распределению чисел по спискам необходимо выполнить,
разделив работу между p потоками. По окончании вывести содержимое обоих списков вместе с их размерами.*/

//#define m 10	// количество интервалосв
#define a 90.32
#define n 8 // размер массива
#define N_THR 4 // количество потоков (четное)

HANDLE even_mutex;
HANDLE odd_mutex;

struct node
{
	int data;
	struct node *next;
};

void push_front(struct node * head, int new_val)
{
	struct node* new_node = (struct node*)malloc(sizeof(struct node));

	new_node->next = head;
	new_node->data = new_val;
	head = new_node;
}

void print_list(struct node * head)
{
	struct node * t = head;
	while (t != NULL)
	{
		printf("%d ", t->data);
		t = t->next;
	}
}

void print_arr(int * arr)
{
	for (int i = 0; i < n; i++)
		printf("%d ", arr[i]);
}

typedef struct aux
{
	int * array;
	struct node * list_ev;
	struct node * list_od;
	int beg;
	int end;
	int tid;
	int * ev_cnt;
	int * od_cnt;
}aux, *paux;

DWORD WINAPI thread_proc(LPVOID Param)
{
	paux data = (paux)Param;
	int beg = data->beg;
	int end = data->end;

	for (size_t i = beg; i < end; i++)
	{
		//int x = data->array[i];
		if (data->array[i] % 2)
		{
			//WaitForSingleObject(odd_mutex, INFINITE);
			//push_front(data->list_od, x);
			//data->list_od
			*(data->od_cnt) += 1;
			printf("od_cnt inc, tid = %d, x = %d \n", data->tid, data->array[i]);
			//ReleaseMutex(odd_mutex, INFINITE);
		}
		else
		{
			//WaitForSingleObject(even_mutex, INFINITE);
			//push_front(data->list_ev, x);
			*(data->ev_cnt) += 1;
			printf("ev_cnt inc, tid = %d, x = %d \n", data->tid, data->array[i]);
			//ReleaseMutex(even_mutex, INFINITE);
		}
	}
}

int main()
{
	//объявление и инициализация массива А
	int * A;
	int ev = 0;
	int od = 0;
	A = malloc(n * sizeof(int));
	for (int i = 0; i < n; i++)
	{
		int x = rand() % (int)floor(a);
		A[i] = x;
	}
	print_arr(A);
	printf("\n");
	struct node* L_odd = malloc(sizeof(struct node));
	struct node* L_even = malloc(sizeof(struct node));

	odd_mutex = CreateMutex(NULL, FALSE, NULL);
	even_mutex = CreateMutex(NULL, FALSE, NULL);


	paux Data_arr[N_THR];

	int arr_b;
	if (n % N_THR == 0)
		arr_b = n / N_THR;
	else arr_b = n / N_THR + 1;

	for (int i = 0; i < N_THR; i++)
	{
		Data_arr[i] = (paux)malloc(N_THR * sizeof(aux));
		Data_arr[i]->array = A;
		Data_arr[i]->ev_cnt = &ev;
		Data_arr[i]->od_cnt = &od;
		Data_arr[i]->list_ev = L_even;
		Data_arr[i]->list_od = L_odd;
		Data_arr[i]->beg = i * arr_b;
		Data_arr[i]->end = Data_arr[i]->beg + arr_b;
		Data_arr[i]->tid = i;
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
	//printf("ODD LIST: ");
	//print_list(L_odd);
	//printf("\n");
	//printf("EVEN LIST: ");
	//print_list(L_even);
	//printf("\n");
	printf("%d %d", ev, od);
	getchar();
}