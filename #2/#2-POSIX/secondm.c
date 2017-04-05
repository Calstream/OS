#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "memory.h"

#define BUF_SIZE 15;

//TCHAR szName[] = TEXT("Global\\CALCShared");

/*typedef struct data
{
	char state;
	char string[15+1];
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

  pthread_condattr_t attributes;
  pthread_condattr_init(&attributes);
  pthread_condattr_setpshared(&attributes, PTHREAD_PROCESS_SHARED);

  pthread_cond_init(&DATA->second_work, &attributes);
  pthread_cond_init(&DATA->first_work, &attributes);

  pthread_mutexattr_t mutex_attributes;
  pthread_mutexattr_init(&mutex_attributes);
  pthread_mutexattr_setpshared(&mutex_attributes, PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(&DATA->mutex, &mutex_attributes);

  DATA->f_w=1;
  DATA->s_w=0;
	//HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, _T("StateChanged"));
	//HANDLE hShared;
	//LPCTSTR pBuf;

	//hShared = CreateFileMapping(
		//INVALID_HANDLE_VALUE,    // use paging file
		//NULL,                    // default security
		//PAGE_READWRITE,          // read/write access
		//0,                       // maximum object size (high-order DWORD)
		//17,//BUF_SIZE + 2,                // maximum object size (low-order DWORD)
		//_T("SharedData"));

	//if (hShared == NULL)
	//{
		//_tprintf(TEXT("Could not create file mapping object (%d).\n"),
			//GetLastError());
		//return 1;
	//}

	//data * DATA = NULL;
	//char * DATA = NULL;
	//DATA = (data *)MapViewOfFile(hShared,   // handle to map object
		//FILE_MAP_ALL_ACCESS, // read/write permission
		//0,
		//0,
		//17); //BUF_SIZE + 2);

	while (1)
	{
		//WaitForSingleObject(hEvent, INFINITE);
		//ResetEvent(hEvent);
    pthread_mutex_lock(&DATA->mutex);
    //printf("waiting for signal\n");
    while (!DATA->s_w){
      pthread_cond_wait(&DATA->second_work, &DATA->mutex);
    }
		if (DATA->state == 0)
		{
			//UnmapViewOfFile(DATA);
			//CloseHandle(hShared);
			//SetEvent(hEvent);
      DATA->s_w=0;
      DATA->f_w = 1;
  		pthread_mutex_unlock(&DATA->mutex);
  		pthread_cond_signal(&DATA->first_work);
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
		//SetEvent(hEvent);
    DATA->s_w=0;
    DATA->f_w = 1;
		pthread_mutex_unlock(&DATA->mutex);
		pthread_cond_signal(&DATA->first_work);
	}
}
