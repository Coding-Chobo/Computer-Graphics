#include <iostream>
#include <string> 

#define SIZE 30
void init_board(char board[][SIZE]);
void Print_board(char board[][SIZE]);
int main() {
	char board[SIZE][SIZE]{};
	bool start{};
	int Start_index[2][2]{};
	char command;
	init_board(board);
	Print_board(board);
	do
	{
		if (!start)
		{
			std::cout << "사각형의 시작 위치를 지정하십시오. : ";
			std::cin >> Start_index[0][0] >> Start_index[0][1] >> Start_index[1][0] >> Start_index[1][1];
			Print_board(board);
			start = true;
		}
		Print_board(board);
		std::cout << "명령어를 입력하십시오.(x,y,s,i,j,a,m,n,r,q) : ";
		std::cin >> command;








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
void Print_board(char board[][SIZE]) {
	for (int i = 0; i < SIZE; i++)
	{

	}
}