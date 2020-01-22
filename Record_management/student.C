
#include <stdio.h>
#include "student.h"

void pack(char *recordbuf, const STUDENT *s);

void unpack(const char *recordbuf, STUDENT *s);

void read(FILE *fp, char *recordbuf, int rrn);

//
//알고리즘: 저장
//학생 파일에서 삭제 레코드의 존재 여부를 검사한 후 삭제 레코드가 존재하면 이 공간에
//새로운 레코드를 저장하며, 만약 존재하지 않으면 파일의 맨마지막에 저장한다.
//
void add(FILE *fp, const STUDENT *s);

//
//알고리즘: 검색
//학생 파일에서 sequential search 기법으로 주어진 학번 키값고 일치하는 레코드를 찾는다.
//출력은 만드시 주어진 print 함수를 사용한다.
//검색 레코드가 존재하면 rrn을, 그렇지 않으면 -1을 리턴한다.
//
int search(FILE *fp, const char *keyval);

//
//알고리즘: 삭제
//학생 파일에서 주어진 학번 키값과 일치하는 레코드를 찾은 후 해당 레코드를 삭제 처리한다.
//
void delete(FILE *fp, const char *keyval);

void printRecord(const STUDENT *s, int n);


int main(int argc, char *argv[])
{
	FILE *fp;
	STUDENT s;
	int search_rrn;
	char recordbuf[RECORD_SIZE];
 
	char init_rrn[2] = "0",init_delete[2] = "-1";
	
	fp = fopen(argv[2], "r+");

	if (fp == NULL) {
		//no file
		printf("파일이 존재하지 않습니다. \n");
		return;
	}		
	else {
		strcpy(s.id, argv[3]);
		if (strncmp(argv[1], "-a", 2) == 0) {
			strcpy(s.name, argv[4]);
			strcpy(s.dept, argv[5]);
			strcpy(s.year, argv[6]);
			strcpy(s.addr, argv[7]);
			strcpy(s.phone, argv[8]);
			strcpy(s.email, argv[9]);

			add(fp, &s);
		}
		else if(strncmp(argv[1], "-d", 2)==0) {
			strcpy(s.id, argv[3]);
			delete(fp, s.id);
		}
		else if (strncmp(argv[1], "-s", 2)==0) {
			search_rrn = search(fp, s.id);
			if (search_rrn == -1) {
            printf("레코드가 존재하지 않습니다. \n");
         }
         else {
            read(fp, recordbuf, search_rrn);
            unpack(recordbuf, &s);
            printRecord(&s, 1);
         }
		}
		else {
		printf("해당 모드가 없습니다. \n");
		}
	}
	return 1;
}



void pack(char *recordbuf, const STUDENT *s) {
	int i, bound;
	for (i = 0; i < strlen((*s).id); i++) {
		if ((*s).id != '"')
			recordbuf[i] = (*s).id[i];
		else {
			i--;
		}
	}
	recordbuf[i] = '#'; i++;
	bound = i;
	for (i; i<strlen((*s).name) + bound; i++) {
		if ((*s).name != '"')
			recordbuf[i] = (*s).name[i - bound];
		else {
			i--;
		}
	}
	recordbuf[i] = '#'; i++;
	bound = i;
	for (i; i<strlen((*s).dept) + bound; i++) {
		if ((*s).dept != '"')
			recordbuf[i] = (*s).dept[i - bound];
		else {
			i--;
		}
	}
	recordbuf[i] = '#'; i++;
	bound = i;
	for (i; i<strlen((*s).year) + bound; i++) {
		if ((*s).year != '"')
			recordbuf[i] = (*s).year[i - bound];
		else {
			i--;
		}
	}
	recordbuf[i] = '#'; i++;
	bound = i;
	for (i; i<strlen((*s).addr) + bound; i++) {
		if ((*s).addr != '"')
			recordbuf[i] = (*s).addr[i - bound];
		else {
			i--;
		}
	}
	recordbuf[i] = '#'; i++;
	bound = i;
	for (i; i<strlen((*s).phone) + bound; i++) {
		if ((*s).phone != '"')
			recordbuf[i] = (*s).phone[i - bound];
		else {
			i--;
		}
	}
	recordbuf[i] = '#'; i++;
	bound = i;

	for (i; i<strlen((*s).email) + bound; i++) {
		if ((*s).email != '"')
			recordbuf[i] = (*s).email[i - bound];
		else {
			i--;
		}
	}
	recordbuf[i] = '#'; i++;
	recordbuf[i] = '\0';

}

void unpack(const char *recordbuf, STUDENT *s) {
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

void read(FILE *fp, char *recordbuf, int rrn) {
	fseek(fp, rrn*RECORD_SIZE+HEADER_SIZE , SEEK_SET);
	fread(recordbuf, RECORD_SIZE, 1, fp);
}

void add(FILE *fp, const STUDENT *s) {
	char max_rrn_buff[3] = "" , delete_header_buff[3] = "", recent_delete_buff[3] = "";
	int max_rrn, delete_header,i;
	char writebuf[RECORD_SIZE] = "";

	pack(writebuf, s);

	if (search(fp,(*s).id) == -1) {
		fseek(fp, 0, SEEK_SET);
		fread(max_rrn_buff , 2, 1, fp);//record_size
		max_rrn = atoi(max_rrn_buff);
		fread(delete_header_buff, 2, 1, fp);//delete_rrn
		delete_header = atoi(delete_header_buff);
		if (delete_header == -1)
		{
			//null recent delete data
			fseek(fp, max_rrn*RECORD_SIZE + HEADER_SIZE, SEEK_SET);
			fwrite(writebuf,RECORD_SIZE,1,fp);	
		}
		else {
			//recent delete data 
			fseek(fp, delete_header*RECORD_SIZE + HEADER_SIZE, SEEK_SET);
			fread(recent_delete_buff,2,1,fp);
			fseek(fp, -2, SEEK_CUR);
			fwrite(writebuf, RECORD_SIZE, 1, fp);
			fseek(fp, 2, SEEK_SET);
			fwrite(recent_delete_buff, 2, 1, fp);
		}

		max_rrn++;
		*max_rrn_buff = max_rrn + '0';
		fseek(fp, 0, SEEK_SET);
		fwrite(max_rrn_buff, 2, 1, fp);
	}
	else {
		printf("학번이 같아서 추가되지 않습니다. \n");
	}
}

void delete(FILE *fp, const char *keyval) {
	int delete_rrn;
	char deletebuf[2] = "" , recent_delete_buf[2] = "", max_rrn_num[2] = "";
	delete_rrn = search(fp, keyval);
	*deletebuf = delete_rrn + '0';
	if(delete_rrn != -1){
		fseek(fp, 0, SEEK_SET);
		//rrn_num --
		fread(max_rrn_num, 2, 1, fp);
		*max_rrn_num = (atoi(max_rrn_num) - 1) + '0';
		fseek(fp, -2, SEEK_CUR);
		fwrite(max_rrn_num, 2, 1, fp);
		//delete
		fread(recent_delete_buf, 2, 1, fp);
		fseek(fp, -2, SEEK_CUR);
		fwrite(deletebuf, 2, 1, fp);
		fseek(fp, delete_rrn*RECORD_SIZE, SEEK_CUR);
		fwrite("*", 1, 1, fp);
		fwrite(recent_delete_buf, 2, 1, fp);
	}
	else {
		printf("삭제할 레코드가 없습니다 \n");
	}
}

int search(FILE *fp, const char *keyval) {
	int max_rrn , rrn , i;
	char deleteMark[1] = "";
	char read_max_rrn[2] = "";
	char recordbuf[127] = "";
	STUDENT s;
	fseek(fp, 0, SEEK_SET);
	fread(read_max_rrn, 2,1,fp);
	max_rrn = atoi(read_max_rrn);
	fseek(fp, 2, SEEK_CUR);
	for (rrn = 0; rrn < max_rrn; rrn++)
	{
		fread(deleteMark, 1, 1, fp);
		if (strncmp(deleteMark , "*" , 1) == 0){
			fseek(fp,-1,SEEK_CUR);
			continue;
		}
		else {
			fseek(fp, -1, SEEK_CUR);
		}

		read(fp, recordbuf, rrn);

		unpack(recordbuf,&s);

		if (strcmp(s.id, keyval) == 0) {
			return rrn;
		}
	}
	return -1;
}

void printRecord(const STUDENT *s, int n)
{
	int i;

	for(i=0; i<n; i++)
	{
		printf("%s#%s#%s#%s#%s#%s#%s\n", s[i].id, s[i].name, s[i].dept, s[i].year, s[i].addr, s[i].phone, s[i].email);
	}
}
