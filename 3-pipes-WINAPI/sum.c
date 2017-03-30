/*Родительский процесс считывает последовательность целых чисел переменной длины и
передаёт её дочернему процессу, который находит её сумму и возвращает обратно родительскому процессу. 
Родительский процесс выводит эту сумму на консоль. По окончании действий оба процесса должны завершиться.*/
#include <windows.h>
#include <stdio.h>

#pragma warning(disable: 4996)

CHAR chBuf[sizeof(int)];

HANDLE fromChildRd = INVALID_HANDLE_VALUE;
HANDLE fromChildWr = INVALID_HANDLE_VALUE;
HANDLE toChildRd = INVALID_HANDLE_VALUE;
HANDLE toChildWr = INVALID_HANDLE_VALUE;
int a;

// little endian to number
int LE_to_Int()
{
	return (chBuf[3] << 24) | (chBuf[2] << 16) | (chBuf[1] << 8) | chBuf[0];
}

// number to little endian
void Int_to_LE(int n)
{
	chBuf[0] = (char)n;
	chBuf[1] = (char)((n >> 8) & 0xFF);
	chBuf[2] = (char)((n >> 16) & 0xFF);
	chBuf[3] = (char)((n >> 24) & 0xFF);
}

void createChild()
{
	TCHAR cmdLine[] = TEXT("child");
	PROCESS_INFORMATION piProcInfo;
	STARTUPINFO siStartInfo;
	BOOL bSuccess = FALSE;

	

	ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

	// Set up members of the STARTUPINFO structure. 
	// This structure specifies the STDIN and STDOUT handles for redirection.

	ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.hStdOutput = fromChildWr;
	siStartInfo.hStdInput = toChildRd;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES; // WutFace

	// Create the child process. 

	bSuccess = CreateProcess(NULL,
		cmdLine,     // command line 
		NULL,          // process security attributes 
		NULL,          // primary thread security attributes 
		TRUE,          // handles are inherited ?
		0,             // creation flags 
		NULL,          // use parent's environment 
		NULL,          // use parent's current directory 
		&siStartInfo,  // STARTUPINFO pointer 
		&piProcInfo);  // receives PROCESS_INFORMATION 

		// Close handles to the child process and its primary thread.
		// Some applications might keep these handles to monitor the status
		// of the child process, for example. 

		CloseHandle(piProcInfo.hProcess);
		CloseHandle(piProcInfo.hThread);
}

// scanf and write seq_len ints to pipe
void WriteToPipe(int seq_len)
{
	Int_to_LE(seq_len);
	a = WriteFile(toChildWr, chBuf, sizeof(int), NULL, NULL);
	for (int i = 0; i < seq_len; i++)
	{
		int t = 0;
		scanf("%d", &t);
		Int_to_LE(t);
		a = WriteFile(toChildWr, chBuf, sizeof(int), NULL, NULL);
	}
}

int ReadFromPipe(void)
{
	ReadFile(fromChildRd, chBuf, sizeof(int), NULL, NULL);
	int res = LE_to_Int();
	return res;
}



int main()
{
	SECURITY_ATTRIBUTES saAttr;

	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	a = CreatePipe(&fromChildRd, &fromChildWr, &saAttr, 0);
	a = SetHandleInformation(fromChildRd, HANDLE_FLAG_INHERIT, 0);

	a = CreatePipe(&toChildRd, &toChildWr, &saAttr, 0);
	a = SetHandleInformation(toChildWr, HANDLE_FLAG_INHERIT, 0);
	
	createChild();
	int seq_len = 0;
	scanf("%d", &seq_len);
	WriteToPipe(seq_len);
	printf("result = %d \n", ReadFromPipe());
	getchar();
	scanf("%d", seq_len);
}