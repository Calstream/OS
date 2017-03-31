#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#define BUF_SIZE sizeof(data);
TCHAR szName[] = TEXT("Global\\CALCShared");

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
	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, _T("StateChanged"));
	HANDLE hShared;
	//LPCTSTR pBuf;

	hShared = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		sizeof(data),                // maximum object size (low-order DWORD)
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
		sizeof(data));
	while (1)
	{
		WaitForSingleObject(hEvent, INFINITE);
		ResetEvent(hEvent);
		if (DATA->state == 0)
		{
			UnmapViewOfFile(DATA);
			CloseHandle(hShared);
			SetEvent(hEvent);
			exit(0);
		}
		else if (DATA->state == 1)
		{
			printf("Something is wrong");
			getchar();
		}
		else if (DATA->state == -1)
		{
			if (DATA->op == '-')
				DATA->res = DATA->a - DATA->b;
			else if (DATA->op == '+')
				DATA->res = DATA->a + DATA->b;
			else if (DATA->op == '*')
				DATA->res = DATA->a * DATA->b;
			else if (DATA->op == '/')
				DATA->res = DATA->a / DATA->b;
			else 
			{
				printf("Something is wrong");
				getchar();
			}
			DATA->state = 1;
			SetEvent(hEvent);
		}
	}
}