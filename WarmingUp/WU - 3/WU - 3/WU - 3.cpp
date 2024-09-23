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
				printf("list�� �� x ���� �Է��Ͻÿ�: ");
				scanf(" %d", &indexlist[current].x);
				printf("list�� �� y ���� �Է��Ͻÿ�: ");
				scanf(" %d", &indexlist[current].y);
				printf("list�� �� z ���� �Է��Ͻÿ�: ");
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
				printf("list�� �� x ���� �Է��Ͻÿ�: ");
				scanf(" %d", &indexlist[0].x);
				printf("list�� �� y ���� �Է��Ͻÿ�: ");
				scanf(" %d", &indexlist[0].y);
				printf("list�� �� z ���� �Է��Ͻÿ�: ");
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
			printf("���� ����Ʈ�� %d ���Դϴ�.\n", current);
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
			printf("\n������ �Ÿ��� ���� �� ��ǥ�� (%d ,%d ,%d)�Դϴ�\n", max.x, max.y, max.z);
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
			printf("\n������ �Ÿ��� ���� ����� ��ǥ�� (%d ,%d ,%d)�Դϴ�\n", min.x, min.y, min.z);
			break;
		case 'a':
			for (int i = 1; i < current; i++) {
				Index key = indexlist[i];  // key�� ���� ����ü ��ü�� ����
				int j = i - 1;

				// key�� ���̸� �������� ����
				while (j >= 0 && indexlist[j].length > key.length) {
					indexlist[j + 1] = indexlist[j];  // ����ü ��ü�� �̵�
					j = j - 1;
				}
				indexlist[j + 1] = key;  // ����ü ��ü�� �ùٸ� ��ġ�� ����
			}
			Print_List(indexlist, current);

			break;
		case 's':
			for (int i = 1; i < current; i++) {
				Index key = indexlist[i];  // key�� ���� ����ü ��ü�� ����
				int j = i - 1;

				// key�� ���̸� �������� ����
				while (j >= 0 && indexlist[j].length < key.length) {
					indexlist[j + 1] = indexlist[j];  // ����ü ��ü�� �̵�
					j = j - 1;
				}
				indexlist[j + 1] = key;  // ����ü ��ü�� �ùٸ� ��ġ�� ����
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