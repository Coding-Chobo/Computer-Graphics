#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {
	int x;
	int y;
	int z;
	int length;
}Index;
void Print_List(Index *a,int current);
void InsertAscending(Index* a, int current);
void InsertDescending(Index* a, int current);
int main() {
	int current{ 0 }, key;
	char command;
	Index indexlist[20];
	Index max,min;
	for (int i = 0; i < 20; i++)
	{
		indexlist[i] = {0,0,0,0};
	}
	do
	{
		command = '0';
		printf("Command : ");
		scanf(" %c", &command);
		
		switch (command)
		{
		case '+':
			if (current<20)
			{
				printf("list에 들어갈 x 값을 입력하시오: ");
				scanf(" %d", &indexlist[current].x);
				printf("list에 들어갈 y 값을 입력하시오: ");
				scanf(" %d", &indexlist[current].y);
				printf("list에 들어갈 z 값을 입력하시오: ");
				scanf(" %d", &indexlist[current].z);
				indexlist[current].length += indexlist[current].x * indexlist[current].x;
				indexlist[current].length += indexlist[current].y * indexlist[current].y;
				indexlist[current].length += indexlist[current].z * indexlist[current].z;
				current++;
				Print_List(indexlist, current);
			}
			break;
		case '-':
			if (current > 0)
			{
				indexlist[current - 1] = { 0,0,0,0 };
				current--;
			}
			Print_List(indexlist, current);
			break;
		case 'e':

			if (current > 0 && current < 20)
			{
				for (int i = current; i > 0; i--)
				{
					indexlist[i] = indexlist[i-1];
				}
			}
			if (current < 20)
			{
				printf("list에 들어갈 x 값을 입력하시오: ");
				scanf(" %d", &indexlist[0].x);
				printf("list에 들어갈 y 값을 입력하시오: ");
				scanf(" %d", &indexlist[0].y);
				printf("list에 들어갈 z 값을 입력하시오: ");
				scanf(" %d", &indexlist[0].z);
				indexlist[0].length = 0;
				indexlist[0].length += indexlist[0].x * indexlist[0].x;
				indexlist[0].length += indexlist[0].y * indexlist[0].y;
				indexlist[0].length += indexlist[0].z * indexlist[0].z;
				current++;
			}
			Print_List(indexlist, current);
			break;
		case 'd':
			if (current > 1)
			{
				for (int i = 1; i < current; i++)
				{
					indexlist[i - 1] = indexlist[i];
				}
				indexlist[current-1] = { 0,0,0,0 };
				current--;
			}
			else if (current == 1)
			{
				indexlist[current - 1] = { 0,0,0,0 };
				current--;
			}
			Print_List(indexlist, current);
			break;
		case 'i	':
			printf("현재 리스트는 %d 개입니다.\n", current);
			break;
		case 'c':
			for (int i = 0; i < current; i++)
			{
				indexlist[i] = { 0,0,0,0 };
			}
			current = 0;
			Print_List(indexlist, current);
			break;
		case 'm':
			max = indexlist[0];
			if (current > 1)
			{
				for (int i = 1; i < current; i++)
				{
					if (max.length < indexlist[i].length)
					{
						max = indexlist[i];
					}
				}
			}
			printf("\n원점과 거리가 가장 먼 좌표는 (%d ,%d ,%d)입니다\n", max.x, max.y, max.z);
			break;
		case 'n':
			min = indexlist[0];
			if (current > 1)
			{
				for (int i = 1; i < current; i++)
				{
					if (min.length > indexlist[i].length)
					{
						min = indexlist[i];
					}
				}
			}
			printf("\n원점과 거리가 가장 가까운 좌표는 (%d ,%d ,%d)입니다\n", min.x, min.y, min.z);
			break;
		case 'a':
			for (int i = 1; i < current; i++) {
				Index key = indexlist[i];  // key로 현재 구조체 전체를 저장
				int j = i - 1;

				// key의 길이를 기준으로 정렬
				while (j >= 0 && indexlist[j].length > key.length) {
					indexlist[j + 1] = indexlist[j];  // 구조체 전체를 이동
					j = j - 1;
				}
				indexlist[j + 1] = key;  // 구조체 전체를 올바른 위치에 삽입
			}
			Print_List(indexlist, current);

			break;
		case 's':
			for (int i = 1; i < current; i++) {
				Index key = indexlist[i];  // key로 현재 구조체 전체를 저장
				int j = i - 1;

				// key의 길이를 기준으로 정렬
				while (j >= 0 && indexlist[j].length < key.length) {
					indexlist[j + 1] = indexlist[j];  // 구조체 전체를 이동
					j = j - 1;
				}
				indexlist[j + 1] = key;  // 구조체 전체를 올바른 위치에 삽입
			}
			Print_List(indexlist, current);
			break;
		case 'q':
			return 0;
			break;
		default:
			break;
		}
	} while (command != 'q');
	return 0;
}
void Print_List(Index* a,int current) {
	if (current == 0)
	{
		printf("List is emety\n");
	}
	else
	{
		for (int i = 0; i < current; i++)
		{
			printf(" %d : (%d, %d, %d, %d )\n", i, a[i].x, a[i].y, a[i].z,a[i].length);
		}
		printf("\n");
	}
}