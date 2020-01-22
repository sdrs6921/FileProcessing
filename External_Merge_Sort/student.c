#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "student.h"

void makeRuns(FILE *inputfp, char *inputbuf);//�Ϸ�
void internalsort(char *inputbuf, int n);//�Ϸ�
void kwaymerge(FILE *outputfp, char *inputbuf, char *outputbuf);
void pack(char *recordbuf, const STUDENT *s);//�Ϸ�
void unpack(const char *recordbuf, STUDENT *s);//�Ϸ�
void readChunk(FILE *runfp, char *inputbuf, int chunkid);
void writeOutputbuf(FILE *outputfp, const char *outputbuf, int n);
int compare(const void *a , const void *b);//�Ϸ�
int recordsNum;//���ڵ��� ���� �������� ����

int main(int argc, char *argv[])
{
	FILE *inputfp, *outputfp;
	char inputbuf[INPUT_BUF_SIZE];
	char outputbuf[OUTPUT_BUF_SIZE], headerbuf[HEADER_SIZE];
	int runSize , chunkSize;


	
	
	inputfp = fopen(argv[2], "r");
	fread(headerbuf, HEADER_SIZE, 1, inputfp);
	recordsNum = ((headerbuf[0] << 8) & 0xFF00) + (headerbuf[1] & 0xFF);
	runSize = recordsNum / RECORDS_NUM_INPUTBUF;
	if (recordsNum % RECORDS_NUM_INPUTBUF != 0) runSize++;
	chunkSize = RECORDS_NUM_INPUTBUF / runSize;

	fseek(inputfp, 0, SEEK_SET);//���� �����͸� ó������ �̵�

	makeRuns(inputfp,inputbuf);//Rundata�� ����
    /*
	outputfp = fopen(argv[3], "w");
	kwaymerge(outputfp, inputbuf, outputbuf);
	
	*/
    fclose(inputfp);
	//fclose(outputfp);

	return 0;
}

//
//�־��� ���ڵ� ���Ͽ��� input buffer ũ�⸸ŭ ���ڵ带 �о� ���� ��
//internal sort ������� '�й�' Ű�� ���� ������ �Ͽ� �� ����� 
//run file�� �����Ѵ�. �̶� ���� ���� run file�� ������ �� �ִ�.
//
void makeRuns(FILE *inputfp, char *inputbuf)
{
    int runSize, i, lastRunNum;
    char headerbuf[HEADER_SIZE] = "", fileName[107] = "" , runNum[100] = "";
    FILE *runfp;
    fread(headerbuf , HEADER_SIZE , 1 ,inputfp);
    runSize = (recordsNum / RECORDS_NUM_INPUTBUF);
    lastRunNum = recordsNum % RECORDS_NUM_INPUTBUF;
    if(lastRunNum != 0) runSize++;
    for(i = 0; i < runSize; i++){
          strcpy(fileName, "runData");
          sprintf(runNum , "%d" , i);
          strcat(fileName, runNum);
          strcat(fileName, ".dat");
          if(lastRunNum != 0){
             if(i != runSize-1){
               fread(inputbuf , INPUT_BUF_SIZE , 1 , inputfp);
               runfp = fopen(fileName, "w");
               internalsort(inputbuf , RECORDS_NUM_INPUTBUF);
               fwrite(inputbuf, INPUT_BUF_SIZE, 1, runfp);
               fclose(runfp);
             }else{
               fread(inputbuf, RECORD_SIZE * lastRunNum , 1 , inputfp);              
               runfp = fopen(fileName, "w");
               internalsort(inputbuf,lastRunNum);
               fwrite(inputbuf, RECORD_SIZE * lastRunNum , 1 , runfp);
               fclose(runfp);
             }
          }else{
               fread(inputbuf , INPUT_BUF_SIZE , 1 , inputfp);
               runfp = fopen(fileName, "w");
               internalsort(inputbuf,RECORDS_NUM_INPUTBUF);
               fwrite(inputbuf, INPUT_BUF_SIZE, 1, runfp);
               fclose(runfp);

          }
         memset(fileName,'\0',sizeof(fileName));
         memset(inputbuf,'\0',sizeof(*inputbuf));
     }
}
//
//makeRuns() ���ο��� ȣ���ϴ� ���� �Լ��μ� ���� ���, C ���̺귯������ �����ϴ�
//qsort()�Լ��� ȣ���ؼ� ������ �� ������ �̷� ��� �޵�� compare() �Լ���
//�����ؾ� �Ѵ�. input buffer�� ����Ǿ� �ִ� ���ڵ���� �й� Ű�� ���� ������������
//�����Ѵ�.
//�Ķ���� n�� inputbuf�� ����Ǿ� �ִ� ���ڵ��� ���� ��Ÿ����.
//
void internalsort(char *inputbuf, int n)
{
    STUDENT s[RECORDS_NUM_INPUTBUF];
    int i;
    char recordbuf[RECORD_SIZE];
    for(i = 0; i < n; i++){
        unpack(inputbuf + (i *RECORD_SIZE), &s[i]);
    }
    


    qsort(s, n , sizeof(STUDENT) , compare);
    
    memset(inputbuf,'\0',sizeof(recordbuf));

    for(i = 0; i < n; i++){
        pack(inputbuf + (i*RECORD_SIZE), &s[i]);
    }
}

//for qsort() - compare
int compare(const void *a , const void *b){
    STUDENT *s1, *s2;
    s1 = (STUDENT *)a;
    s2 = (STUDENT *)b;
    
    if(atoi((*s1).id) < atoi((*s2).id)) return -1;
    else if(atoi((*s1).id) > atoi((*s2).id)) return 1;
    else return 0;
}


//
//run file���� ������� k-way merge sort�� �����Ѵ�.
//�̶� �ݵ�� �־��� input buffer�� output buffr�� ����ؾ� �Ѵ�.
//
void kwaymerge(FILE *outputfp, char *inputbuf, char *outputbuf)
{	
	int cur_records ,runSize ,chunkSize,i,j,*runVal,minidx,outidx = 0;
	char fileName[107], runNum[100], recordbuf[INPUT_BUF_SIZE];
	FILE **runfp;
	STUDENT *s2;


	runSize = recordsNum / RECORDS_NUM_INPUTBUF;
	chunkSize = RECORDS_NUM_INPUTBUF / runSize;
	runfp = (FILE*)malloc(sizeof(FILE*) * runSize);
	runVal = (int*)calloc(sizeof(int) * runSize,sizeof(int));
	s2 = (STUDENT *)malloc(sizeof(STUDENT) * runSize);



	cur_records = recordsNum;

	for  (i = 0; i < runSize; i++)
	{	
		strcpy(fileName, "runData");
		sprintf(runNum, "%d", i);
		strcat(fileName, runNum);
		strcat(fileName, ".dat");
		runfp[i] = fopen(fileName,"r");
		readChunk(runfp[i], inputbuf + (i*chunkSize), 0);
		printf(inputbuf + (i*chunkSize));
		unpack(inputbuf + (i*chunkSize), &s2[i]);//���� ���� �� ����ü�� ���
	}

	while (cur_records > 0) {
		minidx = 0;
		for (i = 1; i < runSize; i++)
		{
			if (atoi(s2[minidx].id) > atoi(s2[i].id)) minidx = i;
		}

		pack(outputbuf + (outidx * RECORD_SIZE),&s2[minidx]);
		outidx++;
		runVal[minidx]++;

		if (runVal[minidx] == chunkSize) {
			readChunk(runfp[minidx], inputbuf + (i*chunkSize), i);
			runVal[minidx] = 0;
		}
		
		if (outidx == OUTPUT_BUF_SIZE) {
			writeOutputbuf(outputfp, outputbuf, OUTPUT_BUF_SIZE);
			outidx = 0;
			cur_records -= OUTPUT_BUF_SIZE;
		}
		unpack(inputbuf + (runVal[minidx]*RECORD_SIZE), &s2[minidx]);

	}
	
}

//
//����4���� ������ �Լ��μ� �ʿ��ϸ� ����Ѵ�. (�׷��� ������ ���� ����� �ʿ䰡 ����)
//
void pack(char *recordbuf, const STUDENT *s)
{
	int i;
    strcpy(recordbuf, (*s).id);
    strcat(recordbuf,"#");
    strcat(recordbuf, (*s).name);
    strcat(recordbuf,"#");
    strcat(recordbuf, (*s).dept);
    strcat(recordbuf, "#");
    strcat(recordbuf, (*s).year);
    strcat(recordbuf, "#");
    strcat(recordbuf, (*s).addr);
    strcat(recordbuf, "#");
    strcat(recordbuf, (*s).phone);
    strcat(recordbuf, "#");
    strcat(recordbuf, (*s).email);
    strcat(recordbuf, "#");

	i = strlen(recordbuf);
	//for (; i < RECORD_SIZE; i++) recordbuf[i] = '\0';
}
void unpack(const char *recordbuf, STUDENT *s)
{
    int i,bound;
	for (i = 0; i < 9; i++)
	{
		if (recordbuf[i] != '#') {
			(*s).id[i] = recordbuf[i];
		}
		else {
			(*s).id[i] = '\0';
			i++;
			break;
		}
	}

	bound = i;

	for (i ; i < 30; i++)
	{
		if (recordbuf[i] != '#') {
			(*s).name[i-bound] = recordbuf[i];
		}
		else {
			(*s).name[i - bound] = '\0';
			i++;
			break;
		}
	}

	bound = i;

	for (i; i < 51; i++)
	{
		if (recordbuf[i] != '#') {
			(*s).dept[i-bound] = recordbuf[i];
		}
		else {
			(*s).dept[i - bound] = '\0';
			i++;
			break;
		}
	}

	bound = i;

	for (i; i < 53; i++)
	{
		if (recordbuf[i] != '#') {
			(*s).year[i - bound] = recordbuf[i];
		}
		else {
			(*s).year[i - bound] = '\0';
			i++;
			break;
		}
	}
	
	bound = i;

	for (i; i < 84; i++)
	{
		if (recordbuf[i] != '#') {
			(*s).addr[i - bound] = recordbuf[i];
		}
		else {
			(*s).addr[i - bound] = '\0';
			i++;
			break;
		}
	}

	bound = i;

	for (i; i < 100; i++)
	{
		if (recordbuf[i] != '#') {
			(*s).phone[i - bound] = recordbuf[i];
		}
		else {
			(*s).phone[i - bound] = '\0';
			i++;
			break;
		}
	}

	bound = i;

	for (i ; i < 127; i++)
	{
		if (recordbuf[i] != '#') {
			(*s).email[i - bound] = recordbuf[i];
		}
		else {
			(*s).email[i - bound] = '\0';
			break;
		}
	}

}

//
//K-way merge sort���� run���κ��� �ش� chunk�� input buffer�� �о� ���� �� ����Ѵ�.
//�̶� �о���� chunk�� input buffer���� �ڽ��� ��ġ�� �����Ѵ�.
//�־��� run�� �� ��°��� �о���� chunk�� input buffer���� �� ��° chunk ��ġ��
//�����Ѵ�.
//
void readChunk(FILE *runfp, char *inputbuf, int chunkid)
{
	int runSize, chunkSize;
	long curFile;
	runSize = recordsNum / RECORDS_NUM_INPUTBUF;
	chunkSize = RECORDS_NUM_INPUTBUF / runSize;
	if (recordsNum % RECORDS_NUM_INPUTBUF != 0) runSize++;
	
	fread(inputbuf, chunkSize, 1, runfp);
}

//
//k-way merge sort�� �߰� ��� ���ڵ尡 output buffer�� ������ ���� ���ο� ���ڵ� ���Ͽ�
//output buffer�� ���ڵ���� �����Ѵ�. ���� sort�� ������ �κп��� output buffer��
//���� �ʾƵ� ���Ͽ� �����ؾ� �� ��쵵 �߻��� �� ������ 'n' �Ķ���Ͱ� �ʿ��ϴ�.
//
void writeOutputbuf(FILE *outputfp, const char *outputbuf, int n)
{
	fwrite(outputbuf, n*OUTPUT_BUF_SIZE, 1, outputfp);
}
