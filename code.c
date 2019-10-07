#define _CRT_SECURE_NO_WARNINGS

// 헤더
#include "header.h"


char file[] = "calendar.txt";
// 0월을 빼기위해, 2월의 경우 윤년계산식에서 따로 계산하므로
int lastday[13] = { 0, 31, 0, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
LIST calendar[10];
MY_ACCOUNT acc[10];

// 함수
void head();		// 메뉴출력
void clearbuffer(); // 버퍼비우는함수
int count_file(FILE *f);// 파일 총 행 수 구하는 함수
 
/* 달력 */
void SORT_calendar(FILE *f, LIST calendar[], int *year, int *month); // 정렬
void VIEW_calendar(FILE *f);   // 1. 일정조회
int CALC_calendar(int *year, int *month); // 달력계산
void print_calendar(int *year, int *month, int *info_DAY); // 달력 출력

/* 일정관리 메뉴 */
void CALENDAR();	// 일정 메뉴선택 (달력,조회/관리)
void management_calendar(FILE *f); // 3. 일정관리
void cal_ADD(FILE *f);	// 일정추가
void cal_MOD(FILE *f);	// 일정수정
void cal_DEL(FILE *f);	// 일정삭제

/* 가계관리 메뉴 */
void ACCOUNT();					// 가계메뉴선택 (조회 / 관리)
void VIEW_ACCOUNT();			// 가계조회(월/일)
void SORTING_month_account();	// 정렬>출력(월별)
void SORTING_day_account();		// 정렬>출력(일별)
void MANAGEMENT_ACCOUNT();			// 가계관리모드 (추가 / 수정 / 삭제)
void accout_ADD(FILE *account);		// 가계추가
void account_MOD(FILE *account);	// 가계수정
void account_DEL(FILE *account);	// 가계삭제

int main() {
	FILE *f;
	enum MENU { LOOK_CAL = 1, LOOK_ACCOUNT, EXIT };
	int menu = 0, temp;

	if ((f = fopen(file, "a+")) == NULL) {
		puts("파일이 열리지 않습니다.");
		exit(1);
	}
	while (1) {
		head();   // 메뉴
		scanf("%d", &menu);

		switch (menu)
		{
		case LOOK_CAL:   // 일정 관리
			CALENDAR();
		break;

		case LOOK_ACCOUNT:   // 가계관리
			ACCOUNT();
			break;

		case EXIT:
			puts("프로그램을 종료합니다.");
			return 0;

		default:
			puts("\t잘못된 입력\n\n");
			printf("\n\tPLESE ENTER ANY KEY: "); scanf("%d", &temp);
			break;
		}
	}

	return 0;
}



void head() {   // 메뉴출력

	system("cls");
	puts("");
	puts("\t┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
	puts("\t┃　　　　　　　　　　　　　　　　　　　┃");
	puts("\t┃ 　Personal _ Management _ Program　　┃");
	puts("\t┃　==================================　┃");
	puts("\t┃　　　　　　　　　　　　　　　　　　　┃");
	puts("\t┃　　　　　1. 일정　관리　 　　　　　　┃");
	puts("\t┃　　　　　2. 가계　관리　 　　　　　　┃");
	puts("\t┃　　　　　3. 프로그램 종료 　 　　　　┃");
	puts("\t┃　　　　　　　　　　　　　　　　　　　┃");
	puts("\t┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
	printf("\t PLEASE ENTER THE MENU NUMBER : ");
}

void clearbuffer() { // 버퍼 비우는 함수
					 // 입력 버퍼에서 문자를 계속 꺼내고 \n를 꺼내면 반복을 중단
	while (getchar() != '\n');
}

int count_file(FILE *f) {
	int size = 0, leng = 0;
	char string[250] = { '\0' };

	rewind(f);
	while (!feof(f)) {
		fgets(string, 250, f);
		leng = strlen(string);

		if (leng == 0) {
			break;			// 라인을 읽었는데 그 라인이 공백이면 그냥 브레이크
		}
		else size += 1;

		string[0] = '\0';
	}

	return size;
}

/* 달력 출력하는 함수 */

void SORT_calendar(FILE *f, LIST calendar[], int *year, int *month) {	// 파일내용 정렬해서 입력하는 함수
	LIST temp;
	FILE *m;
	char string[250] = { '\0' };
	int size = 0, i = 0, j = 0;

	m = fopen("month_temp.txt", "w");


	size = count_file(f);
	rewind(f);

	for (i = 0; i < size; i++) {
		fgets(string, sizeof(string), f);	// 한줄 받아서 정보읽기
		sscanf(string, "[%d %d %d] %[^\n]s ", &calendar[i].cale.year, &calendar[i].cale.month, &calendar[i].cale.day, &calendar[i].content);
		if ((calendar[i].cale.year == *year) && (calendar[i].cale.month == *month)) { // 맞으면 입력
			fputs(string, m);
		}
	}

	fclose(m);
	fclose(f);

	m = fopen("month_temp.txt", "r");
	size = count_file(m);

	rewind(m);

	for (i = 0; i < size; i++) {			// 정렬
		fgets(string, sizeof(string), m);	// 해당 필요데이터만 빈 파일에 있는 상태, 라인을 읽고 데이터읽음
		sscanf(string, "[%d %d %d] %[^\n]s ", \
			&calendar[i].cale.year, &calendar[i].cale.month, &calendar[i].cale.day, &calendar[i].content);
		string[0] = '\0';

		for (j = 0; j<i; j++) {	// i = 1일때, j=0~1, 2일때는 0~2 ... 
			if (calendar[i].cale.day < calendar[j].cale.day) {  // j는 i앞의 값만 검사한다.
				temp = calendar[i];								// 검사해서 앞 배열의 일정이 더 큰 경우(뒷날일 경우)
				calendar[i] = calendar[j];						// 서로 스위칭
				calendar[j] = temp;
			}
		}
	}

	for (i = 0; i < size; i++) {
		printf("\t[%d %d %d] %s \n", calendar[i].cale.year, calendar[i].cale.month, calendar[i].cale.day, calendar[i].content);
	}

	fclose(m);
}

void VIEW_calendar(FILE *f) {
	int year, month;
	int info_DAY = 0;      // 시작요일 구할 변수
	int i = 0, temp;
	while (1) {
		puts("\n\t예시) 2018 06");
		printf("\t년 월 입력:  "); scanf("%d %d", &year, &month);

		if (month > 0 && month < 13)
			break;
		else
			puts("\t잘못된 입력");
	}

	info_DAY = CALC_calendar(&year, &month);
	print_calendar(&year, &month, &info_DAY);

	printf("\t     [ %d년 %d월 일정]\n", year, month);

	SORT_calendar(f, calendar, &year, &month);

	puts("");
	printf("\tPLESE ENTER ANY KEY: "); scanf("%d", &temp);
	fclose(f);
}

int CALC_calendar(int *year, int *month) {   // 달력 시작요일 계산하는 함수
	int total = 0;
	int i;

	for (i = 1; i < *year; i++) {   // 0001년 ~ 입력년도 직전년도까지 총 날짜 수를 구한다.
									// 계속해서 더하면서 total값에 총 일수 저장
		if ((i % 400 == 0) || (i % 4 == 0) && (i % 100 != 0))
			total += 366;       // 윤년의 조건 : 400으로 떨어지는 년도는 무조건 윤년 
		else                    // 4로 떨어지는 년도는 윤년인데, 100으로 떨어지는 년도는 윤년이 아님 
			total += 365;
	}

	for (i = 1; i < *month; i++) {   // 1월~입력 월까지 총 날짜 수를 구한다
		if (i == 2) {				 // 2월일 경우 윤년검사를 해야 함
			if ((*year % 400 == 0) || (*year % 4 == 0) && (*year % 100 != 0))
				total += 29;
			else
				total += 28;
		}
		total += lastday[i];
	}

	total += 1;

	return total % 7;   // 0부터 일 월 화 수 목 금 토
}

void print_calendar(int *year, int *month, int *info_DAY) {   // 입력 년, 월, 시작요일 정보
	int i = 0, count = 1;
	int total;   // 입력 년 총 일수

	switch (*month)
	{
	case 1: case 3: case 5: case 7: case 8: case 10: case 12:
		total = 31;

	case 2:
		if ((*year % 400 == 0) || (*year % 4 == 0) && (*year % 100 != 0))
			total = 29;
		else
			total = 28;

	case 4: case 6: case 9: case 11:
		total = 30;
	}



	printf("\n\t\t[ %d년 %d월 ]\n", *year, *month);
	printf("\t 일  월  화  수  목  금  토\n");
	printf("\t=============================\n\t");
	for (i = 1; i <= *info_DAY; i++) {
		printf("    ");
		count++;
	}
	for (i = 1; i <= total; i++) {
		printf("%3d ", i);

		if (count == 7) {
			printf("\n\t");
			count = 0;
		}
		count++;
	}
	printf("\n\t=============================\n");
}


/* 일정관리 관련 함수 */

void CALENDAR() {
	FILE *f;
	enum CANLENDAR_MENU { VIEW = 1, MANAGEMENT, EXIT };
	int op, temp;

	if ((f = fopen(file, "a+")) == NULL) {
		puts("파일이 열리지 않습니다.");
		exit(1);
	}

	while (1) {
		system("cls");
		puts("");
		puts("\t┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
		puts("\t┃　　　　　　　　　　　　　　　　　　　┃");
		puts("\t┃　　　 일　정　조　회 & 관　리　　　　┃");
		puts("\t┃　==================================　┃");
		puts("\t┃　　　　　　　　　　　　　　　　　　　┃");
		puts("\t┃　　　　1. 달력 및 일정 조회　 　　 　┃");
		puts("\t┃　　　　2. 일정　관리 　　　　　　　  ┃");
		puts("\t┃　　　　3. 나가기　　　　　　 　　　　┃");
		puts("\t┃　　　　　　　　　　　　　　　　　　　┃");
		puts("\t┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
		printf("\t PLEASE ENTER THE MENU NUMBER : "); scanf("%d", &op);

		switch (op) {
		case VIEW:
			VIEW_calendar(f);	// 가계조회함수	- 월별, 일별 조회해야함
			break;

		case MANAGEMENT:
			management_calendar(f);	// 가계관리함수 - 추가 / 수정 / 삭제 
			break;

		case EXIT:
			system("cls");
			break;

		default:
			printf("\n\t잘못된 입력\n");
			printf("\n\tPLESE ENTER ANY KEY: "); scanf("%d", &temp);
		}
		break;
	}
}

void management_calendar(FILE *f) {   // 일정관리메뉴
	enum management_menu { ADD = 1, MOD, DEL, EXIT };
	int op, temp;   // 메뉴입력

	while (1) {
		system("cls");
		puts("");
		puts("\t┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
		puts("\t┃　　　　　　　　　　　　　　　　　　　┃");
		puts("\t┃　　　　일　정　관　리　모　드　　　　┃");
		puts("\t┃　==================================　┃");
		puts("\t┃　　　　　　　　　　　　　　　　　　　┃");
		puts("\t┃　　　 　　1. 추가 　 　　　 　  　　 ┃");
		puts("\t┃　　　　 　2. 수정 　　　 　　　　    ┃");
		puts("\t┃　　　　 　3. 삭제 　 　　　     　   ┃");
		puts("\t┃　　　　　 4. 나가기　　　　  　  　　┃");
		puts("\t┃　　　　　　　　　　　　　　　　　　　┃");
		puts("\t┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
		printf("\t PLEASE ENTER THE MENU NUMBER : "); scanf("%d", &op);

		switch (op)
		{
		case ADD:
			cal_ADD(f);
			break;

		case MOD:
			cal_MOD(f);
			break;

		case DEL:
			cal_DEL(f);
			break;

		case EXIT:
			system("cls");
			break;

		default:
			puts("\n\t잘못된 입력");
			printf("\n\tPLESE ENTER ANY KEY: "); scanf("%d", &temp);
		}
		break;
	}
}

void cal_ADD(FILE *f) {   // 일정추가모드
	int size = 0, temp;

	size = count_file(f);   // 현재포인터(줄)이 총 몇개인가.

	printf("\n\t추가 할 일정의 년,월,일을 입력하세요.\n\t ex) 2018 12 25 : ");
	scanf("%d %d %d", &calendar[size].cale.year, &calendar[size].cale.month, &calendar[size].cale.day);
	clearbuffer();
	printf("\t일정의 내용을 입력하세요. :"); gets_s(calendar[size].content, 100);

	// 파일에 쓰기
	fprintf(f, "[%d %d %d] %s\n", \
		calendar[size].cale.year, calendar[size].cale.month, calendar[size].cale.day, calendar[size].content);

	puts("\t일정이 추가되었습니다.\n");


	fclose(f);

	if ((f = fopen(file, "a+")) == NULL) {
		puts("파일이 열리지 않습니다.");
		exit(1);
	}

	printf("\tPLESE ENTER ANY KEY: "); scanf("%d", &temp);
}

void cal_MOD(FILE *f) {
	int i = 0, j = 0, size = 0, num = 0, op = 0;
	int search_month = 0, search_day = 0;
	int MOD_month = 0, MOD_day = 0, MOD_year = 0;
	char MOD_cont[100];
	char string[250];	//깡통
	int temp;
	FILE *fp;

	printf("\t수정할 월,일을 입력하세요. "); scanf("%d %d", &search_month, &search_day);	// 입력받아서

	size = count_file(f);
	rewind(f);

	for (i = 0; i < size; i++) {
		fgets(string, sizeof(string), f);
		sscanf(string, "[%d %d %d] %[^\n]s ", &calendar[i].cale.year, &calendar[i].cale.month, &calendar[i].cale.day, &calendar[i].content);
		if ((calendar[i].cale.month == search_month) && (calendar[i].cale.day == search_day)) {
			printf("\t[%d] : %s", num, string); num++;
		}
	}

	if (num == 0) {
		printf("\t내역이 없습니다.\n");
	}

	else {
		printf("\n\t수정 할 일정 번호를 입력하세요. "); scanf("%d", &op);		// 해당일정 전까지 그대로 출력하고 그 뒤부터는 덮어쓰기
		if (op > num) {
			puts("잘못된 번호 입력");
		}
		else {
			printf("\t수정 할 일정의 년,월,일을 입력하세요.\n\t ex) 2017 12 25 : ");
			scanf("%d %d %d", &MOD_year, &MOD_month, &MOD_day);
			clearbuffer();
			printf("\t해당 일정의 내용을 입력하세요. :"); gets_s(MOD_cont, 100);

			rewind(f);	// 추가 -> 읽기모드

			fp = fopen("temp.txt", "w+");	//쓰기 읽기

			for (j = 0; j < size; j++) {
				if (j == (i - num + op))
					fprintf(fp, "[%d %d %d] %s\n", MOD_year, MOD_month, MOD_day, MOD_cont);
				else {
					fgets(string, sizeof(string), f);	//f에 있는걸 읽어와서(string에 값이들어가고)
					fputs(string, fp);	// fp에 쭉 쓰고
				}
			}
			fclose(f); // f닫고

			f = fopen(file, "w");	// 다시 f 열어서(쓰기모드)

			fseek(fp, 0L, SEEK_SET);	// 읽기모드로 전환

			while (!feof(fp)) {
				if ((fgets(string, sizeof(string), fp)) != NULL)
					fputs(string, f);	// 쓴 다음에
			}
			fclose(fp);
			fclose(f);				// 닫는다.

			printf("\n\n\t수정 완료\n");
		}
	}
	printf("\tPLESE ENTER ANY KEY: "); scanf("%d", &temp);

	if ((f = fopen(file, "a+")) == NULL) {
		puts("파일이 열리지 않습니다.");
		exit(1);
	}
}

void cal_DEL(FILE *f) {
	int i = 0, j = 0, size = 0, num = 0, op = 0;
	int DEL_month = 0, DEL_day = 0;
	char string[250];	//깡통
	int temp;
	FILE *fp;

	printf("\t삭제할 월,일을 입력하세요. "); scanf("%d %d", &DEL_month, &DEL_day);	// 입력받아서

	size = count_file(f);

	// 해당일정 전까지 그대로 출력하고 그 뒤부터는 덮어쓰기
	rewind(f);	// 추가 -> 읽기모드


	for (i = 0; i < size; i++) {
		fgets(string, sizeof(string), f);
		sscanf(string, "[%d %d %d] %[^\n]s ", &calendar[i].cale.year, &calendar[i].cale.month, &calendar[i].cale.day, &calendar[i].content);
		if ((calendar[i].cale.month == DEL_month) && (calendar[i].cale.day == DEL_day)) {
			printf("\t[%d] : %s", num, string); num++;
		}
	}
	if (num == 0) {
		printf("일정이 없습니다.\n");
	}

	else {
		printf("\t삭제 할 일정 번호를 입력하세요. "); scanf("%d", &op);
		fp = fopen("temp.txt", "w");	//쓰기 읽기

		rewind(f);
		for (j = 0; j < size; j++) {
			fgets(string, sizeof(string), f);	// f에 있는걸 읽어와서(string에 값이들어가고)
			if (j != (i - j + op)) {			// 해당 빼고
				fputs(string, fp);				// fp에 쭉 쓰고
			}
		}

		printf("\n\t %d월 %d일 일정을 삭제 하였습니다.\n", DEL_month, DEL_day);
		fclose(f); // f닫고

		f = fopen(file, "w");	// 다시 f 열어서(쓰기모드)
		fp = fopen("temp.txt", "r");	// 다시 f 열어서(쓰기모드)

		rewind(fp);

		while (!feof(fp)) {
			if ((fgets(string, sizeof(string), fp)) != NULL)
				fputs(string, f);	// 쓴 다음에
		}
		fclose(fp);
		fclose(f);				// 닫는다.


		if ((f = fopen(file, "a+")) == NULL) {
			puts("파일이 열리지 않습니다.");
			exit(1);
		}
	}
	printf("\tPLESE ENTER ANY KEY: "); scanf("%d", &temp);

}


/* 가계관리 관련 함수 */

void ACCOUNT() {
	enum ACOOUNT_MENU { VIEW = 1, MANAGEMENT, EXIT };
	int op, temp;


	while (1) {
		system("cls");
		puts("");
		puts("\t┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
		puts("\t┃　　　　　　　　　　　　　　　　　　　┃");
		puts("\t┃　　　　가　계　조　회 & 관　리　　 　┃");
		puts("\t┃　==================================　┃");
		puts("\t┃　　　　　　　　　　　　　　　　　　　┃");
		puts("\t┃　　　　 　1. 가계　조회　　　 　　 　┃");
		puts("\t┃　　　　 　2. 가계　관리　　　　　　  ┃");
		puts("\t┃　　　　　 3. 나가기　　　　 　　 　　┃");
		puts("\t┃　　　　　　　　　　　　　　　　　　　┃");
		puts("\t┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
		printf("\t PLEASE ENTER THE MENU NUMBER : "); scanf("%d", &op);

		switch (op) {
		case VIEW:
			VIEW_ACCOUNT();		// 가계조회함수	- 월별, 일별 조회해야함
			break;

		case MANAGEMENT:
			MANAGEMENT_ACCOUNT();	// 가계관리함수 - 추가 / 수정 / 삭제 
			break;

		case EXIT:
			system("cls");
			break;

		default:
			printf("\n\t잘못된 입력\n");
			printf("\n\tPLESE ENTER ANY KEY: "); scanf("%d", &temp);
		}
		break;
	}
}

void VIEW_ACCOUNT() {
	int op, temp;

	while (1) {
		system("cls");
		puts("");
		puts("\t┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
		puts("\t┃　　　　　　　　　　　　　　　　　　　┃");
		puts("\t┃　　　　가　계　조　회　모　드　　　　┃");
		puts("\t┃　==================================　┃");
		puts("\t┃　　　　　　　　　　　　　　　　　　　┃");
		puts("\t┃　　　 　　1. 월별 조회 　　 　  　　 ┃");
		puts("\t┃　　　　 　2. 일별 조회　 　　　　    ┃");
		puts("\t┃　　　　　 3. 나가기　　　　  　  　　┃");
		puts("\t┃　　　　　　　　　　　　　　　　　　　┃");
		puts("\t┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
		printf("\t PLEASE ENTER THE MENU NUMBER : "); scanf("%d", &op);

		switch (op)
		{
		case 1:
			SORTING_month_account();	// 월별 정렬(1월~12월)
			break;

		case 2:
			SORTING_day_account();	// 일별 정렬(특정 월에 대해)
			break;

		case 3:
			system("cls");
			break;

		default:
			puts("\n\t잘못된 입력");
			printf("\n\tPLESE ENTER ANY KEY: "); scanf("%d", &temp);

		}
		break;
	}

}

void SORTING_month_account() {
	FILE *account;
	int size = 0, i = 0, temp;
	char string[250];
	int month[13] = { 0 };	// 총 금액 저장배열 (0월 제외하고)

	account = fopen("account.txt", "a+");

	size = count_file(account);
	rewind(account);

	for (i = 0; i < size; i++) {
		fgets(string, sizeof(string), account);
		sscanf(string, "[%d %d] %s : %d", &acc[i].month, &acc[i].day, &acc[i].list, &acc[i].money);

		month[acc[i].month] += acc[i].money;
	}
	puts("");
	for (i = 1; i <= 12; i++) {
		printf("\t%2d월 총금액: %8d원\n", i, month[i]);
	}
	printf("\n\tPLESE ENTER ANY KEY: "); scanf("%d", &temp);

}

void SORTING_day_account() {
	FILE *account;
	FILE *fp;
	MY_ACCOUNT tp;
	int op_month, size = 0, i = 0, j = 0, temp;
	int total = 0;
	char string[250];

	printf("\n\t조회할 월을 입력하세요 "); scanf("%d", &op_month);

	account = fopen("account.txt", "a+");
	fp = fopen("temp.txt", "w");

	size = count_file(account);
	rewind(account);

	for (i = 0; i < size; i++) {
		fgets(string, sizeof(string), account);
		sscanf(string, "[%d %d] %s : %d", &acc[i].month, &acc[i].day, &acc[i].list, &acc[i].money);

		if (acc[i].month == op_month) {
			fputs(string, fp);
		}
	}

	fclose(fp);
	fp = fopen("temp.txt", "r");
	size = count_file(fp);
	if (size == 0) {
		printf("\t해당 월 가계정보가 없습니다.\n");
	}

	else {
		rewind(fp);
		for (i = 0; i < size; i++) {
			string[0] = '\0';
			fgets(string, sizeof(string), fp);
			sscanf(string, "[%d %d] %s : %d원", &acc[i].month, &acc[i].day, &acc[i].list, &acc[i].money);
			total += acc[i].money;
		}
		for (i = 0; i < size; i++) {
			for (j = 0; j < i; j++) {
				if (acc[i].day < acc[j].day) {
					tp = acc[i];
					acc[i] = acc[j];
					acc[j] = tp;
				}
			}
		}

		for (i = 0; i < size; i++) {
			printf("\t[%d %2d] %15s : %8d원\n", acc[i].month, acc[i].day, acc[i].list, acc[i].money);
		}
		printf("\n\t %d월 총금액: %d원\n", op_month, total);
	}
	printf("\n\tPLESE ENTER ANY KEY: "); scanf("%d", &temp);
}

void MANAGEMENT_ACCOUNT() {
	FILE *account;	// 가계저장할 파일
	enum ACCOUNT_MENU { ADD = 1, MOD, DEL, EXIT };
	int op, temp;   // 메뉴입력

	account = fopen("account.txt", "a+");

	while (1) {
		system("cls");
		puts("");
		puts("\t┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
		puts("\t┃　　　　　　　　　　　　　　　　　　　┃");
		puts("\t┃　　　　가　계　관　리　모　드　　　　┃");
		puts("\t┃　==================================　┃");
		puts("\t┃　　　　　　　　　　　　　　　　　　　┃");
		puts("\t┃　　　 　　1. 추가 　 　　　 　  　　 ┃");
		puts("\t┃　　　　 　2. 수정 　　　 　　　　    ┃");
		puts("\t┃　　　　 　3. 삭제 　 　　　     　   ┃");
		puts("\t┃　　　　　 4. 나가기　　　　  　  　　┃");
		puts("\t┃　　　　　　　　　　　　　　　　　　　┃");
		puts("\t┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
		printf("\t PLEASE ENTER THE MENU NUMBER : "); scanf("%d", &op);

		switch (op)
		{
		case ADD:
			accout_ADD(account);	// 가계추가함수
			break;

		case MOD:
			account_MOD(account);
			break;

		case DEL:
			account_DEL(account);
			break;

		case EXIT:	// 나가기
			system("cls");
			break;

		default:
			puts("\n\t잘못된 입력");
			printf("\n\tPLESE ENTER ANY KEY: "); scanf("%d", &temp);
		}
		break;
	}
}

void accout_ADD(FILE *account) {
	int size, temp; // 라인갯수

	size = count_file(account);   // 현재포인터(줄)이 총 몇개인가.

	printf("\n\t월,일을 입력하세요.\n\tex) 12 25 : ");
	scanf("%d %d", &acc[size].month, &acc[size].day);
	clearbuffer();

	printf("\t내용과 내역을 입력하세요.\n \t(내용은 붙여서 입력) :");
	scanf("%s", acc[size].list); scanf("%d", &acc[size].money);

	// 파일에 쓰기
	fprintf(account, "[%d %d] %s : %d원\n", acc[size].month, acc[size].day, acc[size].list, acc[size].money);

	puts("\t추가되었습니다.\n");
	printf("\tPLESE ENTER ANY KEY: "); scanf("%d", &temp);

	fclose(account);
}

void account_MOD(FILE *account) {
	FILE *fp;
	int MM, MD, op;	//수정할 달, 일, 수정번호
	int filesize = 0;	// 파일 크기
	char string[250];
	int i = 0, j = 0, k = 0, temp;
	MY_ACCOUNT tp;

	if ((account = fopen("account.txt", "a+")) == NULL) {
		puts("파일이 열리지 않습니다.");
		exit(1);
	}

	printf("\t수정할 월, 일을 입력하세요. "); scanf("%d %d", &MM, &MD);

	filesize = count_file(account);
	rewind(account);

	for (i = 0; i<filesize; i++) {
		fgets(string, sizeof(string), account);	// 가계 저장된 파일을 읽어서 저장한다
		sscanf(string, "[%d %d] %s : %d", &acc[i].month, &acc[i].day, &acc[i].list, &acc[i].money);

		if ((acc[i].month == MM) && (acc[i].day == MD)) {	 // 같으면
			printf("\t[%d] : %s", j, string); j++;			 // 출력한다, j카운트
		}
	}
	printf("\n\t수정 할 가계의 번호 입력. "); scanf("%d", &op);

	printf("\t내용과 내역을 다시 입력하세요. \n\t(내용은 붙여서 입력) :");
	scanf("%s", tp.list); scanf("%d", &tp.money);	// 빈 깡통구조체에 입력

	fp = fopen("temp.txt", "w+");	//쓰기 깡통파일 열기

	rewind(account);
	for (k = 0; k < filesize; k++) {
		fgets(string, sizeof(string), account);	// 나머지는 원래 파일에서 읽어서 쓴다.
		if (k == i - j + op) {	// 전체 일정 갯수 -  수정할 월, 일 (겹치면) + 리스트의 op번째.
			fprintf(fp, "[%d %d] %s : %d원\n", MM, MD, tp.list, tp.money);	// 즉 해당 가계내역만 바꾼걸로 쓰고
		}
		else {
			fputs(string, fp);
			string[0] = '\0';	// 스트링비우기 
		}
	}
	fclose(account);
	account = fopen("account.txt", "w");	// 쓰기모드로 다시 열어서

	rewind(fp);
	while (!feof(fp)) {
		fgets(string, sizeof(string), fp);	// 깡통파일(temp)에 썼던 가계내역을 원래 account 파일에 덮어쓴다.
		fputs(string, account);
		string[0] = '\0';
	}
	fclose(fp);

	printf("\t수정완료\n\n");
	printf("\tPLESE ENTER ANY KEY: "); scanf("%d", &temp);

	fclose(account);
}

void account_DEL(FILE *account) {
	FILE *fp;
	int DM, DD, op;	//수정할 달, 일, 수정번호
	int filesize = 0;	// 파일 크기
	char string[250];
	int i = 0, j = 0, k = 0, temp;

	printf("\t삭제할 월,일을 입력하세요. >>"); scanf("%d %d", &DM, &DD);	// 입력받아서

	filesize = count_file(account);
	rewind(account);
	for (i = 0; i < filesize; i++) {
		fgets(string, sizeof(string), account);
		sscanf(string, "[%d %d] %s %d", &acc[i].month, &acc[i].day, &acc[i].list, &acc[i].money);
		if ((acc[i].month == DM) && (acc[i].day == DD)) {
			printf("\t[%d] : %s", j, string); j++;
		}
	}

	if (j == 0) {
		printf("\t검색한 날짜의 가계가 없습니다.\n");
	}
	else {
		printf("\n\t삭제 할 가계의 번호를 입력하세요. "); scanf("%d", &op);


		fp = fopen("temp.txt", "w+");	//쓰기 읽기

		for (k = 0; k < filesize; k++) {
			if (k != (i - j + op)) {
				fgets(string, sizeof(string), account);
				fputs(string, fp);	// fp에 쭉 쓰고
			}
		}
		printf("\n\t%d월 %d일 %d번째 가계를 삭제 하였습니다.\n", DM, DD, op);

		fclose(account);
		account = fopen("account.txt", "w");

		rewind(fp);
		while (!feof(fp)) {
			fgets(string, sizeof(string), fp);
			fputs(string, account);
			string[0] = '\0';
		}
		fclose(fp);
	}
	fclose(account);
	printf("\tPLESE ENTER ANY KEY: "); scanf("%d", &temp);

}

