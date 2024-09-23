#include <iostream>
#include <string> 
#include <stdlib.h>
#include <Windows.h>
#include <stdio.h>

#define SIZE 30
typedef struct Square {
    int x{};
    int y{};
    int width_x{};
    int width_y{};
}Square;
void init_board(char board[][SIZE]);
void Print_board(char board[][SIZE], Square player);
void Set_Position(Square* player, int index[][2]);
void Set_Board(char board[][SIZE], Square player, Square player2);

int main() {
    char board[SIZE][SIZE]{};
    bool start{};
    int Start_index[2][2]{};
    Square player{};
    Square player2{};

    char command;
    do
    {
        if (!start)
        {
            std::cout << "1번 사각형의 시작 위치를 지정하십시오. (ex: 1 1 4 6): ";
            std::cin >> Start_index[0][0] >> Start_index[0][1] >> Start_index[1][0] >> Start_index[1][1];
            Set_Position(&player, Start_index);
            std::cout << "2번 사각형의 시작 위치를 지정하십시오. (ex: 1 1 4 6): ";
            std::cin >> Start_index[0][0] >> Start_index[0][1] >> Start_index[1][0] >> Start_index[1][1];
            Set_Position(&player2, Start_index);
            Set_Board(board, player, player2);
            start = true;
        }
        Print_board(board, player);
        std::cout << "명령어를 입력하십시오 : ";
        std::cin >> command;
        switch (command)
        {
        case 'x':
            if (player.x == SIZE)
            {
                player.x = 1;
            }
            else
            {
                player.x++;
            }
            Set_Board(board, player, player2);

            break;
        case 'X':
            if (player.x == 1)
            {
                player.x = SIZE;
            }
            else
            {
                player.x--;
            }
            Set_Board(board, player, player2);

            break;
        case 'y':
            if (player.y == SIZE)
            {
                player.y = 1;
            }
            else
            {
                player.y++;
            }
            Set_Board(board, player, player2);

            break;
        case 'Y':
            if (player.y == 1)
            {
                player.y = SIZE;
            }
            else
            {
                player.y--;
            }
            Set_Board(board, player, player2);

            break;
        case 's':
            player.width_x--;
            player.width_y--;
            Set_Board(board, player, player2);

            break;
        case 'S':
            if (player.x + player.width_x - 1 < SIZE && player.width_x < SIZE)
            {
                player.width_x++;
            }
            if (player.y + player.width_y - 1 < SIZE && player.width_y < SIZE)
            {
                player.width_y++;
            }
            Set_Board(board, player, player2);

            break;
        case 'w':
            if (player2.x == SIZE)
            {
                player2.x = 1;
            }
            else
            {
                player2.x++;
            }
            Set_Board(board, player, player2);

            break;
        case 'W':
            if (player2.x == 1)
            {
                player2.x = SIZE;
            }
            else
            {
                player2.x--;
            }
            Set_Board(board, player, player2);

            break;
        case 'a':
            if (player2.y == SIZE)
            {
                player2.y = 1;
            }
            else
            {
                player2.y++;
            }
            Set_Board(board, player, player2);

            break;
        case 'A':
            if (player2.y == 1)
            {
                player2.y = SIZE;
            }
            else
            {
                player2.y--;
            }
            Set_Board(board, player, player2);

            break;
        case 'd':
            player2.width_x--;
            player2.width_y--;
            Set_Board(board, player, player2);

            break;
        case 'D':
            if (player2.x + player2.width_x - 1 < SIZE && player2.width_x < SIZE)
            {
                player2.width_x++;
            }
            if (player2.y + player2.width_y - 1 < SIZE && player2.width_y < SIZE)
            {
                player2.width_y++;
            }
            Set_Board(board, player, player2);

            break;
        case 'r':
            start = false;
            break;
        default:
            break;
        }
    } while (command != 'q');
}
void init_board(char board[][SIZE]) {
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            board[i][j] = '.';
        }
    }
}
void Print_board(char board[][SIZE], Square player) {
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            if (board[i][j] == 'O')
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
            }
            else if (board[i][j] == 'X')
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
            }
            else
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            }
            std::cout << board[i][j] << " ";
        }
        std::cout << std::endl;
    }
}
void Set_Position(Square* player, int index[][2]) {
    int min_x = (index[0][0] < index[1][0]) ? index[0][0] : index[1][0];
    int max_x = (index[0][0] > index[1][0]) ? index[0][0] : index[1][0];
    int min_y = (index[0][1] < index[1][1]) ? index[0][1] : index[1][1];
    int max_y = (index[0][1] > index[1][1]) ? index[0][1] : index[1][1];
    player->x = min_x;
    player->y = min_y;
    player->width_x = max_x - min_x + 1;
    player->width_y = max_y - min_y + 1;
}
void Set_Board(char board[][SIZE], Square player, Square player2) {
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            board[i][j] = '.';
        }
    }
    //플레이어 1
    for (int i = 0; i < player.width_x; i++) {
        for (int j = 0; j < player.width_y; j++) {
            int x = (player.x - 1 + i) % SIZE;
            int y = (player.y - 1 + j) % SIZE;
            board[y][x] = 'O';
        }
    }
    // 플레이어2 
    for (int i = 0; i < player2.width_x; i++) {
        for (int j = 0; j < player2.width_y; j++) {
            int x = (player2.x - 1 + i) % SIZE;
            int y = (player2.y - 1 + j) % SIZE;
            if (board[y][x] == 'O')
            {
                board[y][x] = 'X';
            }
            else
            {
                board[y][x] = 'O';
            }
        }
    }
}