// project_1.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdio.h"
#include "stdlib.h"
#include "record.h"
#pragma warning(disable:4996)

//
// input parameters: 저장할 레코드 수                 
//

int main(int argc, char **argv)
{
	/**************************************************************************************************************/
	/* 1. 'student.dat' 레코드 파일을 생성하여 사용자로부터 입력받은 레코드 수 만큼의 레코드를 저장하는 코드 구현 */
	/*  - 레코드의 크기는 무조건 200 바이트를 준수                                                                */
	/*  - 레코드와 레코드 사이에 어떤 데이터도 들어가면 안됨                                                      */
	/*  - 레코드의 필드값은 임의로 생성해서 사용해도 무방                                                         */
	/*  - 만약 100개의 레코드를 저장하면 파일 크기는 정확히 20,000 바이트가 되어야 함                             */
	/**************************************************************************************************************/
	/**************************************************************************************************************/
	/* 2. 위에서 생성한 'student.dat' 파일로부터 입력받은 레코드 수만큼의 레코드를 읽어들이는 코드 구현           */
	/*  - 반드시 레코드 하나하나씩(200 바이트씩) 읽어들일 것                                                      */
	/**************************************************************************************************************/
	struct _Student *s; // header파일내의 구조체 변수 선언
	struct _Student *s2;
 	int size, i , st_top;
	char storage[200];

	printf("저장하실 학생의 수를 입력하시오 : ");
	scanf("%d", &size);
	/*저장할 레코드 수를 입력해서 구조체 동적 할당*/
	s = (struct _Student*)calloc(size,sizeof(struct _Student));
	s2 = (struct _Student*)calloc(size, sizeof(struct _Student));

	for (i = 0; i < size; i++)
	{
		/*저장할 학생 수 만큼 반복*/
		printf("학번을 입력하세요 : ");
		scanf(" %[^\n]s" , s[i].id);
		printf("이름을 입력하세요 : ");
		scanf(" %[^\n]s" , s[i].name);
		printf("주소를 입력하시오 : ");
		scanf(" %[^\n]s" , s[i].address);
		printf("학교를 입력하시오 : ");
		scanf(" %[^\n]s" , s[i].univ);
		printf("학과를 입력하시오 : ");
		scanf(" %[^\n]s" , s[i].dept);
		printf("기타사항을 입력하시오 : ");
		scanf(" %[^\n]s" , s[i].others);
	}
	
	FILE *fp;

	/*파일 데이터를 새로 만들기 위해서*/
	fp = fopen("student.dat", "w");
	fclose(fp);
	for ( i = 0; i < size; i++)
	{
		for (st_top = 0; st_top < 200; st_top++) {
			if (st_top < 10)
				storage[st_top] = s[i].id[st_top];
			else if(st_top <30)
				storage[st_top] = s[i].name[st_top-10];				
			else if (st_top < 80)
				storage[st_top] = s[i].address[st_top-30];
			else if (st_top < 100)
				storage[st_top] = s[i].univ[st_top-80];
			else if (st_top < 130) {
				storage[st_top] = s[i].dept[st_top - 100];
			}
			else
				storage[st_top] = s[i].others[st_top-130];
		}
		/*앞에서 파일을 초기화 만들었으므로 추가모드*/
		fp = fopen("student.dat", "a");
		fwrite(&storage, sizeof(storage), 1, fp);
		fclose(fp);
	}


	fclose(fp);
	
	fp = fopen("student.dat", "r+");

	

	for  (i = 0; i < size; i++)
	{
		fread(storage, 200, 1, fp);
		for (st_top = 0; st_top < 200; st_top++) {
			if (st_top < 10)
				s2[i].id[st_top] = storage[st_top];
			else if (st_top <30)
				s2[i].name[st_top - 10] = storage[st_top];
			else if (st_top < 80)
				s2[i].address[st_top - 30] = storage[st_top];
			else if (st_top < 100)
				s2[i].univ[st_top - 80] = storage[st_top];
			else if (st_top < 130) {
				s2[i].dept[st_top - 100] = storage[st_top];
			}
			else
				s2[i].others[st_top - 130] = storage[st_top];
		}
	}
	fclose(fp);
	
	printf("%s",s2[0].id);
	printf("%s", s2[0].name);
	printf("%s", s2[0].address);
	printf("%s", s2[0].univ);
	printf("%s", s2[0].dept);
	printf("%s", s2[0].others);
	
	
	/*동적할당 해제*/
	free(s);
	
	system("pause");
	return 0;
}
