#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <iostream>
#include <time.h>
#define Max 4
void ShowMatrix(int a[][4], int b[][4]);
void MakeMatrix(int a[][4], int b[][4]);
void ShowResult(int a[][4]);
double Det3by3(int a[3][3]);
double Det4by4(int a[][4]);
int main() {
	int Matrix1[4][4], Matrix2[4][4], Result[4][4], temp[4][4], temp2[4][4];
	char command;
	int even = 0;

	srand(time(NULL));
	MakeMatrix(Matrix1, Matrix2);
	do
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				Result[i][j] = 0;
			}
		}
		ShowMatrix(Matrix1, Matrix2);
		printf("Get Command : ");
		scanf("%s", &command);
		printf("\nResult \n\n");
		switch (command)
		{
		case'm':
			for (int k = 0; k < Max; k++)
			{
				for (int i = 0; i < Max; i++)
				{
					for (int j = 0; j < Max; j++)
					{
						Result[i][j] += Matrix1[i][k] * Matrix2[k][j];
					}
				}
			}
			ShowResult(Result);
			break;
		case'a':
			for (int i = 0; i < Max; i++)
			{
				for (int j = 0; j < Max; j++)
				{
					Result[i][j] = Matrix1[i][j] + Matrix2[i][j];
				}
			}
			ShowResult(Result);
			break;

		case'd':
			for (int i = 0; i < Max; i++)
			{
				for (int j = 0; j < Max; j++)
				{
					Result[i][j] = Matrix1[i][j] - Matrix2[i][j];
				}
			}
			ShowResult(Result);
			break;

		case'r':
			printf("Det( Matrix1 ) : %f\n", Det4by4(Matrix1));
			printf("Det( Matrix2 ) : %f\n\n", Det4by4(Matrix2));
			break;

		case't':
			for (int i = 0; i < Max; i++)
			{
				for (int j = 0; j < Max; j++)
				{
					temp[i][j] = Matrix1[i][j];
					temp2[i][j] = Matrix2[i][j];
				}
			}
			for (int i = 0; i < Max; i++)
			{
				for (int j = 0; j < Max; j++)
				{
					Matrix1[i][j] = temp[j][i];
					Matrix2[i][j] = temp[j][i];
				}
			}
			printf("Det( Matrix1 ) : %f\n", Det4by4(Matrix1));
			printf("Det( Matrix2 ) : %f\n\n", Det4by4(Matrix2));
			break;

		case'e':
			for (int i = 0; i < Max; i++)
			{
				for (int j = 0; j < Max; j++)
				{
					temp[i][j] = Matrix1[i][j];
					temp2[i][j] = Matrix2[i][j];
				}
			}
			if (even == 0)
			{
				{
					for (int i = 0; i < Max; i++)
					{
						for (int j = 0; j < Max; j++)
						{
							if (Matrix1[i][j] == 1)
							{
								printf("  ");
							}
							else
							{
								printf("%d ", Matrix1[i][j]);
							}
						}
						printf("\n");
					}
					printf("\n");
					for (int i = 0; i < Max; i++)
					{
						for (int j = 0; j < Max; j++)
						{
							if (Matrix2[i][j] == 1)
							{
								printf("  ");
							}
							else
							{
								printf("%d ", Matrix2[i][j]);
							}
						}
						printf("\n");
					}
				}
				even++;
			}
			else if (even == 1)
			{
				for (int i = 0; i < Max; i++)
				{
					for (int j = 0; j < Max; j++)
					{
						if (Matrix1[i][j] == 1)
						{
							printf("%d ", Matrix1[i][j]);
						}
						else
						{
							printf("  ");
						}
					}
					printf("\n");
				}
				printf("\n");
				for (int i = 0; i < Max; i++)
				{
					for (int j = 0; j < Max; j++)
					{
						if (Matrix2[i][j] == 1)
						{
							printf("%d ", Matrix2[i][j]);
						}
						else
						{
							printf("  ");
						}
					}
					printf("\n");
				}
				even++;
			}
			else
			{
				printf("\n\n");
				ShowMatrix(Matrix1, Matrix2);
				even = 0;
			}

			break;
		case's':
			MakeMatrix(Matrix1, Matrix2);
			break;
		case'1':
			ShowMatrix(Matrix1, Matrix2);
			break;
		case'2':
			for (int i = 0; i < Max; i++)
			{
				for (int j = 0; j < Max; j++)
				{
					Matrix1[i][j] = Matrix1[i][j] * 2;
					Matrix2[i][j] = Matrix2[i][j] * 2;
				}
			}
			break;
		case'3':
			for (int i = 0; i < Max; i++)
			{
				for (int j = 0; j < Max; j++)
				{
					Matrix1[i][j] = Matrix1[i][j] * 3;
					Matrix2[i][j] = Matrix2[i][j] * 3;
				}
			}
			break;
		case'4':
			for (int i = 0; i < Max; i++)
			{
				for (int j = 0; j < Max; j++)
				{
					Matrix1[i][j] = Matrix1[i][j] * 4;
					Matrix2[i][j] = Matrix2[i][j] * 4;
				}
			}
			break;
		case'5':
			for (int i = 0; i < Max; i++)
			{
				for (int j = 0; j < Max; j++)
				{
					Matrix1[i][j] = Matrix1[i][j] * 5;
					Matrix2[i][j] = Matrix2[i][j] * 5;
				}
			}
			break;
		case'6':
			for (int i = 0; i < Max; i++)
			{
				for (int j = 0; j < Max; j++)
				{
					Matrix1[i][j] = Matrix1[i][j] * 6;
					Matrix2[i][j] = Matrix2[i][j] * 6;
				}
			}
			break;
		case'7':
			for (int i = 0; i < Max; i++)
			{
				for (int j = 0; j < Max; j++)
				{
					Matrix1[i][j] = Matrix1[i][j] * 7;
					Matrix2[i][j] = Matrix2[i][j] * 7;
				}
			}
			break;
		case'8':
			for (int i = 0; i < Max; i++)
			{
				for (int j = 0; j < Max; j++)
				{
					Matrix1[i][j] = Matrix1[i][j] * 8;
					Matrix2[i][j] = Matrix2[i][j] * 8;
				}
			}
			break;
		case'9':
			for (int i = 0; i < Max; i++)
			{
				for (int j = 0; j < Max; j++)
				{
					Matrix1[i][j] = Matrix1[i][j] * 9;
					Matrix2[i][j] = Matrix2[i][j] * 9;
				}
			}
			break;

		}
	} while (command != 'q');

	return 0;
}
void ShowMatrix(int a[][4], int b[][4]) {
	printf("Matrix 1 \n");
	for (int i = 0; i < Max; i++)
	{
		for (int j = 0; j < Max; j++)
		{
			printf("%d ", a[i][j]);
		}
		printf("\n");
	}
	printf("\nMatrix 2 \n");
	for (int i = 0; i < Max; i++)
	{
		for (int j = 0; j < Max; j++)
		{
			printf("%d ", b[i][j]);
		}
		printf("\n");
	}
}
void MakeMatrix(int p[][4], int q[][4]) {
	int a, b, c = 0, d = 0;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			p[i][j] = rand() % 2;
			q[i][j] = rand() % 2;
		}
	}
	a = rand() % 4;
	b = rand() % 4;
	p[a][b] = 2;
	q[a][b] = 2;
	do
	{
		p[c][d] = rand() % 2;
		q[c][d] = rand() % 2;
		c = rand() % 4;
		d = rand() % 4;
		p[c][d] = 2;
		q[c][d] = 2;
	} while (a == c && b == d);
}
void ShowResult(int a[][4]) {
	for (int i = 0; i < Max; i++)
	{
		for (int j = 0; j < Max; j++)
		{
			printf("%d ", a[i][j]);
		}
		printf("\n");
	}
}
double Det3by3(int a[3][3]) {
	return a[0][0] * (a[1][1] * a[2][2] - a[1][2] * a[2][1]) -
		a[0][1] * (a[1][0] * a[2][2] - a[1][2] * a[2][0]) +
		a[0][2] * (a[1][0] * a[2][1] - a[1][1] * a[2][0]);
}
double Det4by4(int a[][4]) {
	double det = 0;
	int submatrix[3][3];

	// 첫 번째 행을 기준으로 라플라스 전개
	for (int x = 0; x < 4; x++) {
		int subi = 0; // submatrix의 행 인덱스
		for (int i = 1; i < 4; i++) {
			int subj = 0; // submatrix의 열 인덱스
			for (int j = 0; j < 4; j++) {
				if (j == x)
					continue;
				submatrix[subi][subj] = a[i][j];
				subj++;
			}
			subi++;
		}

		// 4x4 행렬의 행렬식은 3x3 행렬식의 라플라스 전개로 계산
		det += (x % 2 == 0 ? 1 : -1) * a[0][x] * Det3by3(submatrix);
	}

	return det;
}