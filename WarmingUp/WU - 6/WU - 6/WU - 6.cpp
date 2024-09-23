#include <iostream>
#include <string>
#include <time.h>
#include <windows.h>

#define max_direct 4
#define SIZE 30
#define MAX_FAIL_COUNT 50  // �ִ� ���� Ƚ�� ����

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

    // ��� ������ ������ ��� ��õ�
    bool success = Make_Load(board);
    while (!success) {
        init_Board(board);
        success = Make_Load(board);
    }

    do {
        system("cls");
        Print_Board(board, player);
        std::cout << "��ɾ �Է��Ͻʽÿ� : ";
        std::cin >> command;
        if (player.x == SIZE - 1 && player.y == SIZE - 1) {
            std::cout << "�����Ͽ����ϴ�. �ٽ� �����Ͻ÷��� g�� �Է��Ͻʽÿ�";
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
        case 'r': // �÷��̾� ���� �� �ʱ�ȭ
            player = { 0, 0, true };
            break;
        case 'g': // ��� �� ���� �ʱ�ȭ
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
    } while (cnt != 30);  // ��ֹ� 30�� ��ġ
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
    board[player.x][player.y] = 'O';  // �÷��̾� ��ġ�� �ٽ� ǥ��
}

bool Make_Load(char board[][SIZE]) {
    int direction{}, Max_movement{ 5 }, canmove{}, failcount{};
    Position Load{ 0, 0 };
    board[Load.x][Load.y] = 'O';  // ������ ����

    while (Load.x != SIZE - 1 || Load.y != SIZE - 1) {
        Position prevLoad = Load;  // ���� ��ġ ����
        Load.is_survive = true;
        direction = rand() % max_direct;  // ������ ���� ����
        canmove = rand() % Max_movement + 1;  // �������� �̵��� ĭ �� ����

        if (Load.x == SIZE - 1) {
            direction = 2;  // ���������θ� �̵�
        }
        if (Load.y == SIZE - 1) {
            direction = 1;  // �Ʒ��θ� �̵�
        }

        while (canmove > 0 && Load.is_survive) {
            prevLoad = Load;  // ���� ��ġ ���� (���� ��ġ�� ���ƿ��� ���� �غ�)
            switch (direction) {
            case 0: // ����
                if (Load.y > 0) Load.y--;
                else Load.is_survive = false;  // ��� �ʰ�
                break;
            case 1: // �Ʒ�
                if (Load.x < SIZE - 1) Load.x++;
                else Load.is_survive = false;
                break;
            case 2: // ������
                if (Load.y < SIZE - 1) Load.y++;
                else Load.is_survive = false;
                break;
            case 3: // ��
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
                    return false;  // ���� �� false ��ȯ
                }
                break;
            }
        }

        // ��ֹ��� �ε��� ���, ���ο� ������ �����Ͽ� �ٽ� �õ�
        if (!Load.is_survive) {
            direction = rand() % max_direct;  // ������ �������� �ٲ� �ٸ� �������� �̵�
        }
    }

    return true;  // ��� ���� ���� �� true ��ȯ
}

void is_Block(char board[][SIZE], Position* Load, Position prevLoad, int direction) {
    if (board[Load->x][Load->y] == '#' || board[Load->x][Load->y] == 'O') {
        // ��ֹ��� ������ ���� ��ġ�� ���ư�
        Load->x = prevLoad.x;
        Load->y = prevLoad.y;
        Load->is_survive = false;  // ��θ� ����
    }
    else {
        board[Load->x][Load->y] = 'O';  // ��� ǥ��
    }
}
