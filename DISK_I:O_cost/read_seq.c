#include "stdio.h"
#include "stdlib.h"
#include "sys/time.h"

int main(int argc, char **argv)
{
	struct timeval start, end;
	int operating_time;

	
	int num_of_records, i;
	char *read_dat[200];

	if (argc != 2)
		exit(1);

	num_of_records = atoi(argv[1]);

	FILE *fp;

	fp = fopen("student.dat","r");

	gettimeofday(&start, NULL);
	for (i = 0; i < num_of_records; i++)
	{
		fread(read_dat, sizeof(read_dat), 1, fp);
	}
	gettimeofday(&end, NULL);

	operating_time = (int)(1000000 * ((end.tv_sec) - (start.tv_sec)) + ((end.tv_usec) - (start.tv_usec)));

	printf("elapsed_time : %d us", operating_time);

	return 0;
}
