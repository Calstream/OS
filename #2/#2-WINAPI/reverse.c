#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#define BUF_SIZE 15;

TCHAR szName[] = TEXT("Global\\CALCShared");

typedef struct data
{
	char state;
	char string[15+1];
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
		17,//BUF_SIZE + 2,                // maximum object size (low-order DWORD)
		_T("SharedData"));

	if (hShared == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n"),
			GetLastError());
		return 1;
	}

	data * DATA = NULL;
	//char * DATA = NULL;
	DATA = (data *)MapViewOfFile(hShared,   // handle to map object
		FILE_MAP_ALL_ACCESS, // read/write permission
		0,
		0,
		17); //BUF_SIZE + 2);

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
		else if (DATA->state == 69)
		{
			int n = strlen(DATA->string);
			for (int i = 0; i < n / 2; i++)
			{
				char temp = DATA->string[i];
				DATA->string[i] = DATA->string[n - i - 1];
				DATA->string[n - i - 1] = temp;
			}


			if (n % 2 == 0)
			{
				int pos = n / 2;
				memmove(DATA->string + pos + 1, DATA->string + pos, n - pos + 1);
				DATA->string[pos] = '-';
			}
/*
			char * temp = "sosi pisos";
			for (int i = 0; i < strlen(temp); i++)
				DATA->string[i] = temp[i];*/
		}
		SetEvent(hEvent);
	}
}