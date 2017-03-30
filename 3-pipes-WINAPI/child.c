/*Родительский процесс считывает последовательность целых чисел переменной длины и
передаёт её дочернему процессу, который находит её сумму и возвращает обратно родительскому процессу.
Родительский процесс выводит эту сумму на консоль. По окончании действий оба процесса должны завершиться.*/
#include <windows.h>
#include <stdio.h>

#pragma warning(disable: 4996)

CHAR chBuf[sizeof(int)];

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

int main(void)
{
	HANDLE hStdin, hStdout;
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	if (
		(hStdout == INVALID_HANDLE_VALUE) ||
		(hStdin == INVALID_HANDLE_VALUE)
		)
		ExitProcess(1);

	
	DWORD read_n = 0;
	ReadFile(hStdin, chBuf, sizeof(int), read_n, NULL);
	int seq_len = LE_to_Int();

	int sum = 0;
	for (int i = 0; i < seq_len; i++)
	{
		int t;
		ReadFile(hStdin, chBuf, sizeof(int), NULL, NULL);
		t = LE_to_Int();
		sum += t;
	}	
	Int_to_LE(sum);
	WriteFile(hStdout, chBuf, sizeof(int), NULL, NULL);
	
	return 0;
}