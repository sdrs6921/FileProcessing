#ifndef _STUDENT_H_
#define _STUDENT_H_

#define RECORD_SIZE		127	//id(8) + name(20) + department(20) + year(1) + address(30) + phone(15) +  email(26) + delimeters(7)
#define HEADER_SIZE		4	//#records(2) + head(2)
// 필요한 경우 'define'을 추가할 수 있음

typedef struct _STUDENT
{
	char id[9];		// 학번
	char name[21];		// 이름
	char dept[21];		// 학과
	char year[2];		// 학년
	char addr[31];		// 주소:w
	char phone[16];		// 전화번호
	char email[27];		// 이메일 주소
} STUDENT;

#endif
