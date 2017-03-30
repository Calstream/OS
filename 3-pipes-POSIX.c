#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

char chBuf[sizeof(int)];

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

int main() 
{

	int to_child[2];
	int from_child[2];
	pipe(to_child);
	pipe(from_child);
	int seq_size;
	scanf("%d", &seq_size);
	Int_to_LE(seq_size);
	write(to_child[1], chBuf, sizeof(int));
	int i;
	int el;
	for(i = 0; i < seq_size; i++)
	{
		scanf("%d", &el);
		Int_to_LE(el);
		write(to_child[1], chBuf, sizeof(int));
	}

	if (fork() == 0) 
	{
   /* child program */
		close(to_child[1]); /* close the write end*/
		close(from_child[0]); /* close the read end */
		read(to_child[0], chBuf, sizeof(int));
		int c = LE_to_Int();
		int sum = 0;
		for (i = 0; i < c; i++)
		{
			read(to_child[0], chBuf, sizeof(int));
			int t = LE_to_Int();
			sum += t;
		}
		Int_to_LE(sum);
		write(from_child[1], chBuf, sizeof(int));
	}
	else 
	{
   /* parent program */
		close(from_child[1]); /* close the write end of pipe */
		close(to_child[0]); /* close the read end of pipe */
		int res = 0;
		read(from_child[0], chBuf, sizeof(int)); /* Read from pipe */
		res = LE_to_Int();
		printf("SUM = %d \n", res);
	}
	exit(0);
}
