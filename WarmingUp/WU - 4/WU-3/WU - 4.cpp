#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include <string.h>
#define SIZE 5
using namespace std;
void Print_Board(char board[SIZE][SIZE]);
void Show_Card(char board[2][SIZE][SIZE], char position[5], int* point);
void Shuffle_Card(char board[SIZE][SIZE]);
void init_Card(char board[2][SIZE][SIZE]);

int main() {
    srand(time(NULL));
    char board[2][5][5];
    char position[5];
    char command = 'r';
    int chance = 21;
    int totalpoint = 0;
    init_Card(board);
    do
    {
        for (int i = 0; i < 4; i++)
        {
            position[i] = '\0';
        }
        Print_Board(board[0]);
        printf("확인할 두 개의 칸을 정하십시오.(ex : a1b3)\n남은 기회 : %d\n\n", chance);
        scanf(" %s", position);
        chance--;
        if (chance == 0 || board[0] == board[1])
        {
            printf("게임이 끝났습니다 %d점 입니다.\n다시하시려면 r을 입력, 끝내시려면 q를 입력해주십시오.", totalpoint);
            std::cin >> command;
            chance = 21;
            init_Card(board);
        }   
        Show_Card(board, position, &totalpoint);
        printf("\n");
    } while (command != 'q' && command == 'r');


    return 0;
}

void Print_Board(char board[SIZE][SIZE]) {
    printf(" ");
    for (int i = 0; i < SIZE; i++)
    {
        printf("%3c", i + 97);
    }
    printf("\n");
    for (int i = 0; i < SIZE; i++)
    {
        printf("%d", i + 1);
        for (int j = 0; j < SIZE; j++)
        {
            if (board[i][j] != '*')
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), board[i][j] - 65);
            }
            printf("%3c", board[i][j]);
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
        }
        printf("\n");
    }
}
void init_Card(char board[2][SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            board[0][i][j] = '*';
        }
    }
    char alpha[25];
    for (int i = 0; i < 24; i++)
    {
        alpha[i] = 65 + i / 2;
    }
    alpha[24] = '@';
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            board[1][i][j] = alpha[5 * i + j];
        }
    }
    Shuffle_Card(board[1]);
}
void Show_Card(char board[2][SIZE][SIZE], char position[5], int* point) {
    int check[4];
    check[1] = position[0] - 97;
    check[0] = position[1] - '1';
    check[3] = position[2] - 97;
    check[2] = position[3] - '1';
    board[0][check[0]][check[1]] = board[1][check[0]][check[1]];
    board[0][check[2]][check[3]] = board[1][check[2]][check[3]];
    Print_Board(board[0]);

    if (board[1][check[0]][check[1]] == board[1][check[2]][check[3]] || board[1][check[0]][check[1]] == '@' || board[1][check[2]][check[3]] == '@')
    {
        point += 4;
    }
    else
    {
        board[0][check[0]][check[1]] = '*';
        board[0][check[2]][check[3]] = '*';
    }
}
void Shuffle_Card(char board[SIZE][SIZE]) {
    int j, row_i, row_j, col_i, col_j;
    char temp;
    for (int i = SIZE * SIZE - 1; i > 0; i--)
    {
        j = rand() % (i + 1);
        row_i = i / SIZE;
        row_j = j / SIZE;
        col_i = i % SIZE;
        col_j = j % SIZE;
        temp = board[row_i][col_i];
        board[row_i][col_i] = board[row_j][col_j];
        board[row_j][col_j] = temp;
    }
}