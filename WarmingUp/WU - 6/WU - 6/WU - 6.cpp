#include <iostream>
#include <string>
#include <time.h>
#include <windows.h>

#define max_direct 4
#define SIZE 30
#define MAX_FAIL_COUNT 50  // 최대 실패 횟수 설정

typedef struct Position {
    int x{};
    int y{};
    bool is_survive{};
} Position;

void init_Board(char board[][SIZE]);
void Print_Board(char board[][SIZE], Position player);
bool Make_Load(char board[][SIZE]);
void is_Block(char board[][SIZE], Position* Load, Position prevLoad, int direction);

int main() {
    char board[SIZE][SIZE];
    Position player{ 0, 0, false };
    srand(time(NULL));
    char command{};

    init_Board(board);

    // 경로 생성이 실패할 경우 재시도
    bool success = Make_Load(board);
    while (!success) {
        init_Board(board);
        success = Make_Load(board);
    }

    do {
        system("cls");
        Print_Board(board, player);
        std::cout << "명령어를 입력하십시오 : ";
        std::cin >> command;
        if (player.x == SIZE - 1 && player.y == SIZE - 1) {
            std::cout << "도착하였습니다. 다시 시작하시려면 g를 입력하십시오";
        }
        switch (command) {
        case 'W': case 'w':
            if (player.y > 0 && board[player.x][player.y - 1] == 'O') {
                player.y--;
            }
            break;
        case 's': case 'S':
            if (player.y < 29 && board[player.x][player.y + 1] == 'O') {
                player.y++;
            }
            break;
        case 'a': case 'A':
            if (player.x > 0 && board[player.x - 1][player.y] == 'O') {
                player.x--;
            }
            break;
        case 'd': case 'D':
            if (player.x < 29 && board[player.x + 1][player.y] == 'O') {
                player.x++;
            }
            break;
        case 'r': // 플레이어 생성 및 초기화
            player = { 0, 0, true };
            break;
        case 'g': // 경로 및 게임 초기화
            player = { 0, 0, false };
            init_Board(board);
            success = Make_Load(board);
            while (!success) {
                init_Board(board);
                success = Make_Load(board);
            }
            break;
        default:
            break;
        }
        Sleep(500);
        std::cout << std::endl;
    } while (command != 'q');
}

void init_Board(char board[][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            board[i][j] = '*';
        }
    }
    int x{}, y{}, cnt{};
    do {
        x = rand() % 25 + 2;
        y = rand() % 25 + 2;
        if (board[x][y] == '#') {
            continue;
        }
        else {
            board[x][y] = '#';
            cnt++;
        }
    } while (cnt != 30);  // 장애물 30개 배치
}

void Print_Board(char board[][SIZE], Position player) {
    if (player.is_survive)
    {
        board[player.x][player.y] = '@';
    }
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            std::cout << board[j][i] << "  ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    board[player.x][player.y] = 'O';  // 플레이어 위치를 다시 표시
}

bool Make_Load(char board[][SIZE]) {
    int direction{}, Max_movement{ 5 }, canmove{}, failcount{};
    Position Load{ 0, 0 };
    board[Load.x][Load.y] = 'O';  // 시작점 설정

    while (Load.x != SIZE - 1 || Load.y != SIZE - 1) {
        Position prevLoad = Load;  // 현재 위치 저장
        Load.is_survive = true;
        direction = rand() % max_direct;  // 무작위 방향 선택
        canmove = rand() % Max_movement + 1;  // 무작위로 이동할 칸 수 결정

        if (Load.x == SIZE - 1) {
            direction = 2;  // 오른쪽으로만 이동
        }
        if (Load.y == SIZE - 1) {
            direction = 1;  // 아래로만 이동
        }

        while (canmove > 0 && Load.is_survive) {
            prevLoad = Load;  // 현재 위치 저장 (이전 위치로 돌아오기 위한 준비)
            switch (direction) {
            case 0: // 왼쪽
                if (Load.y > 0) Load.y--;
                else Load.is_survive = false;  // 경계 초과
                break;
            case 1: // 아래
                if (Load.x < SIZE - 1) Load.x++;
                else Load.is_survive = false;
                break;
            case 2: // 오른쪽
                if (Load.y < SIZE - 1) Load.y++;
                else Load.is_survive = false;
                break;
            case 3: // 위
                if (Load.x > 0) Load.x--;
                else Load.is_survive = false;
                break;
            }

            if (Load.is_survive) {
                is_Block(board, &Load, prevLoad, direction);
                canmove--;
            }
            if (!Load.is_survive) {
                failcount++;

                if (failcount > MAX_FAIL_COUNT) {
                    return false;  // 실패 시 false 반환
                }
                break;
            }
        }

        // 장애물에 부딪힌 경우, 새로운 방향을 설정하여 다시 시도
        if (!Load.is_survive) {
            direction = rand() % max_direct;  // 방향을 무작위로 바꿔 다른 방향으로 이동
        }
    }

    return true;  // 경로 생성 성공 시 true 반환
}

void is_Block(char board[][SIZE], Position* Load, Position prevLoad, int direction) {
    if (board[Load->x][Load->y] == '#' || board[Load->x][Load->y] == 'O') {
        // 장애물을 만나면 직전 위치로 돌아감
        Load->x = prevLoad.x;
        Load->y = prevLoad.y;
        Load->is_survive = false;  // 경로를 차단
    }
    else {
        board[Load->x][Load->y] = 'O';  // 경로 표시
    }
}
