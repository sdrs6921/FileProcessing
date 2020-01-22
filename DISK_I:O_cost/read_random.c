#include "stdio.h"
#include "stdlib.h"
#include "sys/time.h"
#include "time.h"

#define SUFFLE_NUM	10000

void GenRecordSequence(int *list, int n);
void swap(int *a, int *b);


int main(int argc, char **argv)
{
	struct timeval start, end;
	int operating_time;

	int *read_order_list;
	int num_of_records, i;
	char *read_dat[200];

	if (argc != 2)
		exit(1);

	num_of_records = atoi(argv[1]);

	read_order_list = (int *)calloc(num_of_records,sizeof(int));


	GenRecordSequence(read_order_list, num_of_records);

	FILE *fp;

	fp = fopen("student.dat", "r");
	gettimeofday(&start, NULL);
	for (i = 0; i < num_of_records; i++)
	{
		fseek(fp, read_order_list[i] * sizeof(read_dat), SEEK_SET);
		fread(read_dat, sizeof(read_dat), 1, fp);
	}
	gettimeofday(&end, NULL);

	operating_time = (int)(1000000*((end.tv_sec)-(start.tv_sec)) + ((end.tv_usec)-(start.tv_usec)));

	printf("elapsed_time : %d us", operating_time);

	fclose(fp);
	return 0;
}

void GenRecordSequence(int *list, int n)
{
	int i, j, k;

	srand((unsigned int)time(0));

	for (i = 0; i<n; i++)
	{
		list[i] = i;
	}

	for (i = 0; i<SUFFLE_NUM; i++)
	{
		j = rand() % n;
		k = rand() % n;
		swap(&list[j], &list[k]);
	}

	return;
}

void swap(int *a, int *b)
{
	int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;

	return;
}
