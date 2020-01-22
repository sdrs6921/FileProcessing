#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "student.h"

void makeRuns(FILE *inputfp, char *inputbuf);//완료
void internalsort(char *inputbuf, int n);//완료
void kwaymerge(FILE *outputfp, char *inputbuf, char *outputbuf);
void pack(char *recordbuf, const STUDENT *s);//완료
void unpack(const char *recordbuf, STUDENT *s);//완료
void readChunk(FILE *runfp, char *inputbuf, int chunkid);
void writeOutputbuf(FILE *outputfp, const char *outputbuf, int n);
int compare(const void *a , const void *b);//완료
int recordsNum;//레코드의 갯수 전역변수 선언

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

	fseek(inputfp, 0, SEEK_SET);//파일 포인터를 처음으로 이동

	makeRuns(inputfp,inputbuf);//Rundata를 만듬
    /*
	outputfp = fopen(argv[3], "w");
	kwaymerge(outputfp, inputbuf, outputbuf);
	
	*/
    fclose(inputfp);
	//fclose(outputfp);

	return 0;
}

//
//주어진 레코드 파일에서 input buffer 크기만큼 레코드를 읽어 들인 후
//internal sort 방식으로 '학번' 키에 대해 정렬한 하여 그 결과를 
//run file에 저장한다. 이때 여러 개의 run file이 생성될 수 있다.
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
//makeRuns() 내부에서 호출하는 정렬 함수로서 예를 들면, C 라이브러리에서 제공하는
//qsort()함수를 호출해서 구현할 수 있으며 이런 경우 받드시 compare() 함수를
//제공해야 한다. input buffer에 저장되어 있는 레코드들을 학번 키에 따라 오름차순으로
//정렬한다.
//파라미터 n은 inputbuf에 저장되어 있는 레코드의 수를 나타낸다.
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
//run file들을 대상으로 k-way merge sort를 수행한다.
//이때 반드시 주어진 input buffer와 output buffr를 사용해야 한다.
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
		unpack(inputbuf + (i*chunkSize), &s2[i]);//가장 작은 값 구조체에 담기
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
//과제4에서 구현한 함수로서 필요하면 사용한다. (그렇지 않으면 굳이 사용할 필요가 없음)
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
//K-way merge sort에서 run으로부터 해당 chunk를 input buffer로 읽어 들일 때 사용한다.
//이때 읽어들인 chunk는 input buffer에서 자신의 위치에 저장한다.
//주어진 run이 두 번째라면 읽어들인 chunk는 input buffer에서 두 번째 chunk 위치에
//저장한다.
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
//k-way merge sort의 중간 결과 레코드가 output buffer에 완전히 차면 새로운 레코드 파일에
//output buffer의 레코드들을 저장한다. 물론 sort의 마지막 부분에서 output buffer가
//차지 않아도 파일에 저장해야 할 경우도 발생할 수 있으며 'n' 파라미터가 필요하다.
//
void writeOutputbuf(FILE *outputfp, const char *outputbuf, int n)
{
	fwrite(outputbuf, n*OUTPUT_BUF_SIZE, 1, outputfp);
}
