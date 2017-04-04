#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#define BUF_SIZE 15
TCHAR szName[] = TEXT("Global\\CALCShared");

typedef struct data
{
	char state;
	char string[BUF_SIZE+1];
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
		BUF_SIZE + 2,                // maximum object size (low-order DWORD)
		_T("SharedData"));       // name of mapping object

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
		BUF_SIZE + 2);

	if (DATA == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());

		CloseHandle(hShared);
	}

	DATA->state = 69;

	while (1)
	{
		ResetEvent(hEvent);
		int success = 0;
		char temp_str[BUF_SIZE];
		memset(temp_str, 0, BUF_SIZE);
		memset(DATA->string, 0, BUF_SIZE+1);
		while (1)
		{
			printf("Enter string (max length = %d): ", BUF_SIZE);
			success = scanf("%s", temp_str);
			//fgets(temp_str, BUF_SIZE, stdin);
			if (strlen(temp_str) <= BUF_SIZE) break;
			printf("no \n");
			while (getchar() != '\n') continue;
		}
		int t = strlen(temp_str);
		for (int i = 0; i < t; i++)
			DATA->string[i] = temp_str[i];
		SetEvent(hEvent);
		WaitForSingleObject(hEvent, INFINITE);
		char exit = 0;
		printf("Result = %s \n", DATA->string);
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