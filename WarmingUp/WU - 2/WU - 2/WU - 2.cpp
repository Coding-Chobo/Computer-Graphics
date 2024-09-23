#include <iostream>
#include <string>
#include <fstream>
#include <Windows.h>
using namespace std;
void ReverseLine(string line);






int main() {
	ifstream file;
	string line;
	char command;
	if (file.is_open())
	{
		while (getline(file,line))
		{
			cout << line << endl;
		}	
		file.close();
	}
	else {
		cerr << "파일이 없습니다" << endl;
	}
	cout << "명령어를 입력하십시오 : ";
	cin >> command;
	switch (command)
	{
	case 'c':
		

		break;
	case 'd':

		break;
	case 'e':

		break;
	case 'f':

		break;
	case 'g':

		break;
	case 'h':

		break;
	case 'r':

		break;
	case 's':

		break;
	case 'q':

		break;
	

	default:
		break;
	}






	return 0;
}
void ReverseLine(string line) {
	string temp;
	for (int i = line.length(); i >= 0; i--)
	{
			
	}
}