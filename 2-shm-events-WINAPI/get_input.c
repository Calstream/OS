/*В общей памяти хранятся следующие значения : 
два целых операнда, 
операция('+', '-', '*', '/'), 
целый результат 
и состояние : «данные введены, нужен результат», «результат вычислен, можно вводить новые данные» и «выход».
Первая программа в цикле(пока пользователь не попросил завершиться) вводит данные из клавиатуры или файла, 
записывает их в общую память и устанавливает состояние, сигнализируя, что данные введены.Вторая программа, 
дождавшись данных, выполняет над ними указанную операцию, записывает результат и устанавливает состояние.
Первая программа считывает результат и выводит его на консоль.Реализовать ожидание изменения состояния 
при помощи событий или условных переменных — в зависимости от используемого интерфейса.*/
//int1 int2 char int3 char - 14 bytes total
//awaiting res = -1
//res ready = 1
//exit = 0

#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#define BUF_SIZE sizeof(data)
TCHAR szName[] = TEXT("Global\\CALCShared");
TCHAR szMsg[] = TEXT("Message from first process.");

typedef struct data
{
	int a;
	int b;
	char op;
	int res;
	char state;
}data;

int main()
{
	
	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, _T("StateChanged")); // TRUE(set) - go, FALSE(reset) - wait
	HANDLE hShared;
	//LPCTSTR pBuf;

	hShared = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		BUF_SIZE,                // maximum object size (low-order DWORD)
		_T("SharedData"));       // name of mapping object

	if (hShared == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n"),
			GetLastError());
		return 1;
	}
	data * DATA = NULL;
	DATA = (data *)MapViewOfFile(hShared,   // handle to map object
		FILE_MAP_ALL_ACCESS, // read/write permission
		0,
		0,
		BUF_SIZE);

	if (DATA == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());

		CloseHandle(hShared);
	}
	DATA->a = 420;
	DATA->b = 69;
	DATA->res = 73;
	DATA->op = '-';
	DATA->state = 1;

	while (1)
	{
		ResetEvent(hEvent);
		int success = 0;
		int temp_int;
		char temp_char;

		while (1)
		{
			printf("Enter first number: ");
			success = scanf(" %d", &temp_int);
			if (success) break;
			printf("that's not a number \n");
			while (getchar() != '\n') continue;
		}
		DATA->a = temp_int;

		while (1)
		{
			printf("Enter second number: ");
			success = scanf(" %d", &temp_int);
			if (success) break;
			printf("that's not a number \n");
			while (getchar() != '\n') continue;
		}
		DATA->b = temp_int;

		while (1)
		{
			printf("Enter operation(+, -, *, /): ");
			success = scanf(" %c", &temp_char);
			if (success && (temp_char == '+' || temp_char == '-' || temp_char == '*' || temp_char == '/')) break;
			printf("that's not a valid operation \n");
			while (getchar() != '\n') continue;
		}
		DATA->op = temp_char;

		DATA->state = -1;
		SetEvent(hEvent);
		WaitForSingleObject(hEvent, INFINITE);
		if (DATA->state == 1)
			printf("%d %c %d = %d \n", DATA->a, DATA->op, DATA->b, DATA->res);
		else if (DATA->state == -1)
		{
			printf("\nSomething went wrong \n");
			Sleep(1000);
			exit(0);
		}
		char exit = 0;
		while (tolower(exit) != 'y' && tolower(exit) != 'n')
		{
			printf("Do you want to exit? (y/n): ");
			scanf(" %c", &exit);
		}
		if (tolower(exit) == 'y')
		{
			DATA->state = 0;
			SetEvent(hEvent);
			WaitForSingleObject(hEvent, INFINITE);
			break;
		}
	}
	UnmapViewOfFile(DATA);
	CloseHandle(hShared);
	CloseHandle(hEvent);
	return 0;
}