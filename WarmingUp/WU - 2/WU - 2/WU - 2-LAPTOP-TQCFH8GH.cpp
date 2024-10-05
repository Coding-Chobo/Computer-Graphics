#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <Windows.h>
#include <vector>
using namespace std;

void Print_text(string text[]);
void Change_Color_Cap(string text, int* count); // 수정된 함수 원형
void ReverseLine(string text[]);
void Insert_initial(string text[]);
void Reverse_Word(string& text);
void Count_Word(string text);
void Change_alpha(string text[], char alphabet[]);
void Sorted_Sentense(string text[], int* judge);
void SearchByWord(string text, int* count, char t[]);
int CountWords(string text);
int main() {
    string filename = "test.txt";
    ifstream file(filename);
    char command{}, word[20]{};
    string line;
    string strline[10];
    string temp[10];
    bool is_cap = false, is_insert = false;
    int index{}, cnt{};
    char alphabet[2];
    int judge{};

    if (file.is_open()) {
        // 파일의 각 줄을 한 줄씩 읽어서 출력하면서 끝에 endl 추가
        while (getline(file, line)) {
            strline[index++] = line;
            cout << line << endl; // 줄 끝에 endl을 추가하여 출력
        }
        file.close();
    }
    else {
        cerr << "파일을 열 수 없습니다." << endl;
        return 1;
    }
    for (int i = 0; i < 10; i++)
    {
        temp[i] = strline[i];
    }




    do {
        cout << "명령어를 입력하십시오 : ";
        cin >> command;

        switch (command) {
        case 'c':
            if (!is_cap)
            {
                for (int i = 0; i < 10; i++)
                {
                    Change_Color_Cap(strline[i], &cnt); // 전체 텍스트를 대문자 색상 변경 함수로 처리
                }
                cout << "총 나온 단어는 " << cnt << "개 입니다." << endl;
                is_cap = true;
            }
            else
            {
                cout << endl;
                Print_text(strline);
                is_cap = false;
            }

            break;
        case 'd':
            ReverseLine(strline);
            Print_text(strline);
            break;
        case 'e':
            if (!is_insert)
            {
                Insert_initial(strline);
                Print_text(strline);
                is_insert = true;
            }
            else
            {
                for (int i = 0; i < 10; i++)
                {
                    strline[i] = temp[i];
                }
                Print_text(strline);
                is_insert = false;
            }

            break;
        case 'f':
            for (int i = 0; i < 10; i++)
            {
                Reverse_Word(strline[i]);
            }
            Print_text(strline);
            break;
        case 'g':
            cout << "바꾸실 문자 두 개를 입력하십시오. (ex :a b) : ";
            cin >> alphabet[0] >> alphabet[1];

            Change_alpha(strline, alphabet);
            Print_text(strline);

            break;
        case 'h':
            for (int i = 0; i < 10; i++)
            {
                Count_Word(strline[i]);
            }
            break;
        case 'r':
            if (judge < 2)
            {
                Sorted_Sentense(strline, &judge);
            }
            else
            {
                for (int i = 0; i < 10; i++) {
                    strline[i] = temp[i];
                }
                judge = 0;
                Print_text(strline);
            }

            break;
        case 's':
            cout << "표시하실 단어를 입력하십시오. (ex :Tour) : ";
            cin >> word;
            cnt = 0;
            for (int i = 0; i < 10; i++)
            {
                SearchByWord(strline[i], &cnt, word);
            }
            cout << endl << "Amount of Word : " << cnt << endl;
            break;
        case 'q':
            cout << "프로그램 종료" << endl;
            break;
        default:
            cout << "잘못된 명령어입니다." << endl;
            break;
        }

    } while (command != 'q');

    return 0;
}

// 대문자로 시작하는 단어를 다른 색으로 출력하는 함수
void Change_Color_Cap(string text, int* count) {
    stringstream ss(text);
    string word;
    // 공백을 기준으로 단어를 하나씩 추출
    while (ss >> word) {
        // 첫 번째 글자가 대문자인지 확인
        if (isupper(word[0])) {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // 초록색
            cout << word << " ";
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);  // 기본 색상으로 복원
            (*count)++;
        }
        else {
            cout << word << " ";
        }
    }
    cout << endl;
}

void Print_text(string text[]) {
    for (int i = 0; i < 10; i++)
    {
        cout << text[i] << endl;
    }
    cout << endl;
}
void ReverseLine(string text[]) {
    for (int i = 0; i < 10; i++)
    {
        reverse(text[i].begin(), text[i].end());
    }
}
void Insert_initial(string text[]) {
    for (int i = 0; i < 10; i++) {
        int len = text[i].length(); // 전체 길이 측정
        for (int j = 3; j < len; j += 3 + 2) { 
            text[i].insert(j, "@@");
            len += 2;  
        }
    }
}
void Reverse_Word(string& text) { 
    stringstream ss(text);
    string word;
    string reversed_text = "";
    while (ss >> word) {
        reverse(word.begin(), word.end());  // 단어를 뒤집음
        reversed_text += word + " ";        
    }

    text = reversed_text;
}
void Count_Word(string text) {
    int cnt{};
    stringstream ss(text);
    string word;
    string reversed_text = "";  // 뒤집힌 단어를 저장할 새로운 문자열
    while (ss >> word)
    {
        cout << word << " ";
        cnt++;
    }
    cout << "              Amount of words :  " << cnt << endl << endl;
}
void Change_alpha(string text[], char alphabet[]) {
    for (int i = 0; i < 10; i++) {
        replace(text[i].begin(), text[i].end(), alphabet[0], alphabet[1]);  // from 문자를 to 문자로 바꿈
    }
}
void Sorted_Sentense(string text[], int* judge) {
    vector<pair<int, string>> lines;  // 단어 수와 줄을 함께 저장할 벡터

    // 각 줄의 단어 수와 줄 자체를 벡터에 저장
    for (int i = 0; i < 10; i++) {
        lines.push_back({ CountWords(text[i]), text[i] });
    }

    if (*judge == 0) {
        // 오름차순 정렬
        sort(lines.begin(), lines.end());
        cout << "오름차순 정렬 완료." << endl;

        // 정렬된 결과를 원본 배열에 다시 저장
        for (int i = 0; i < 10; i++) {
            text[i] = lines[i].second;
        }
        (*judge)++;  // 상태를 변경
    }
    else if (*judge == 1) {
        // 내림차순 정렬
        sort(lines.begin(), lines.end(), greater<>());
        cout << "내림차순 정렬 완료." << endl;

        // 정렬된 결과를 원본 배열에 다시 저장
        for (int i = 0; i < 10; i++) {
            text[i] = lines[i].second;
        }
        (*judge)++;  // 상태를 변경
    }

    Print_text(text);  // 결과 출력
}
void SearchByWord(string text, int* cnt, char t[]) {
    stringstream ss(text);
    string word;
    string searchWord = t;
    transform(searchWord.begin(), searchWord.end(), searchWord.begin(), ::tolower);
    while (ss >> word)
    {
        string lowerWord = word;
        transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);
        if (lowerWord == searchWord)
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
            cout << word << " ";
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            (*cnt)++;
        }
        else
        {
            cout << word << " ";
        }

    }
    cout << endl;
}
int CountWords(const string text) {
    stringstream ss(text);
    string word;
    int count = 0;
    while (ss >> word) {
        count++;
    }
    return count;
}