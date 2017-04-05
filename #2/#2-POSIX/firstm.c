#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "memory.h"

#define BUF_SIZE 15
//TCHAR szName[] = TEXT("Global\\CALCShared");

/*typedef struct data
{
	char state;
	char string[BUF_SIZE+1];
}data;*/
pConnect DATA;
int main()
{
  int segment = shmget(MEMORY_KEY, sizeof(Connect), 0);
  if (-1 == segment) {
    perror("Can't allocate shared memory.");
    return -1;
  }
  DATA = (pConnect)shmat(segment, NULL, 0);
  if ((void *) -1 == DATA) {
    perror("Can't attach shared memory to the process.");
    return -1;
  }

	//HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, _T("StateChanged")); // TRUE(set) - go, FALSE(reset) - wait
	//HANDLE hShared;
	//LPCTSTR pBuf;

	//hShared = CreateFileMapping(
		//INVALID_HANDLE_VALUE,    // use paging file
		//NULL,                    // default security
		//PAGE_READWRITE,          // read/write access
		//0,                       // maximum object size (high-order DWORD)
		//BUF_SIZE + 2,                // maximum object size (low-order DWORD)
		//_T("SharedData"));       // name of mapping object

	//if (hShared == NULL)
	//{
		//_tprintf(TEXT("Could not create file mapping object (%d).\n"),
		//	GetLastError());
		//return 1;
	//}
	//data * DATA = NULL;
	//char * DATA = NULL;
	//DATA = (data *)MapViewOfFile(hShared,   // handle to map object
		//FILE_MAP_ALL_ACCESS, // read/write permission
		//0,
		//0,
		//BUF_SIZE + 2);

	//if (DATA == NULL)
	//{
		//_tprintf(TEXT("Could not map view of file (%d).\n"),
		//	GetLastError());

		//CloseHandle(hShared);
	//}

	DATA->state = 69;

	while (1)
	{
		//ResetEvent(hEvent);
    //pthread_mutex_lock(&DATA->mutex);
    //while (!DATA->f_w)
			//pthread_cond_wait(&DATA->first_work, &DATA->mutex);
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
		//SetEvent(hEvent);
		//WaitForSingleObject(hEvent, INFINITE);
    DATA->f_w = 0;
    DATA->s_w = 1;
    pthread_mutex_unlock(&DATA->mutex);
    pthread_cond_signal(&DATA->second_work);
    while (!DATA->f_w)
        pthread_cond_wait(&DATA->first_work, &DATA->mutex);
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
			//SetEvent(hEvent);
			//WaitForSingleObject(hEvent, INFINITE);
      DATA->f_w = 0;
      DATA->s_w = 1;
      pthread_mutex_unlock(&DATA->mutex);
      pthread_cond_signal(&DATA->second_work);

      while (!DATA->f_w)
          pthread_cond_wait(&DATA->first_work, &DATA->mutex);
			break;
		}
	}
	//UnmapViewOfFile(DATA);
	//CloseHandle(hShared);
	//CloseHandle(hEvent);
	return 0;
}
