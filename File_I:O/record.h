#ifndef _RECORD_H_
#define _RECORD_H_

//student record size = 200 bytes
typedef struct _Student
{
	char id[11];		//학번: 10 bytes
	char name[21];		//이름: 20 bytes
	char address[51];	//주소: 50 bytes
	char univ[21];		//학교: 20 bytes
	char dept[31];		//학과: 30 bytes
	char others[71];	//그외: 70 bytes
} Student;

#endif
