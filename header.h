#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>


struct date	// 일정 구조체
{
	int year;   //년
	int month;   //월
	int day;   //일
};

typedef struct	// 일정관리 구조체
{
	struct date cale;      //일정
	char content[100];      //일정내용
}LIST;

typedef struct	// 가계 구조체
{
	int month;	// 월
	int day;	// 일
	int money;	// 금액
	char list[30];	// 내역
}MY_ACCOUNT;
