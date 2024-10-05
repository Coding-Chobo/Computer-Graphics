#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <Windows.h>
#include <vector>
using namespace std;

void Print_text(string text[]);
void Change_Color_Cap(string text, int* count); // ������ �Լ� ����
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
        // ������ �� ���� �� �پ� �о ����ϸ鼭 ���� endl �߰�
        while (getline(file, line)) {
            strline[index++] = line;
            cout << line << endl; // �� ���� endl�� �߰��Ͽ� ���
        }
        file.close();
    }
    else {
        cerr << "������ �� �� �����ϴ�." << endl;
        return 1;
    }
    for (int i = 0; i < 10; i++)
    {
        temp[i] = strline[i];
    }




    do {
        cout << "��ɾ �Է��Ͻʽÿ� : ";
        cin >> command;

        switch (command) {
        case 'c':
            if (!is_cap)
            {
                for (int i = 0; i < 10; i++)
                {
                    Change_Color_Cap(strline[i], &cnt); // ��ü �ؽ�Ʈ�� �빮�� ���� ���� �Լ��� ó��
                }
                cout << "�� ���� �ܾ�� " << cnt << "�� �Դϴ�." << endl;
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
            cout << "�ٲٽ� ���� �� ���� �Է��Ͻʽÿ�. (ex :a b) : ";
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
            cout << "ǥ���Ͻ� �ܾ �Է��Ͻʽÿ�. (ex :Tour) : ";
            cin >> word;
            cnt = 0;
            for (int i = 0; i < 10; i++)
            {
                SearchByWord(strline[i], &cnt, word);
            }
            cout << endl << "Amount of Word : " << cnt << endl;
            break;
        case 'q':
            cout << "���α׷� ����" << endl;
            break;
        default:
            cout << "�߸��� ��ɾ��Դϴ�." << endl;
            break;
        }

    } while (command != 'q');

    return 0;
}

// �빮�ڷ� �����ϴ� �ܾ �ٸ� ������ ����ϴ� �Լ�
void Change_Color_Cap(string text, int* count) {
    stringstream ss(text);
    string word;
    // ������ �������� �ܾ �ϳ��� ����
    while (ss >> word) {
        // ù ��° ���ڰ� �빮������ Ȯ��
        if (isupper(word[0])) {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); // �ʷϻ�
            cout << word << " ";
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);  // �⺻ �������� ����
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
        int len = text[i].length(); // ��ü ���� ����
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
        reverse(word.begin(), word.end());  // �ܾ ������
        reversed_text += word + " ";        
    }

    text = reversed_text;
}
void Count_Word(string text) {
    int cnt{};
    stringstream ss(text);
    string word;
    string reversed_text = "";  // ������ �ܾ ������ ���ο� ���ڿ�
    while (ss >> word)
    {
        cout << word << " ";
        cnt++;
    }
    cout << "              Amount of words :  " << cnt << endl << endl;
}
void Change_alpha(string text[], char alphabet[]) {
    for (int i = 0; i < 10; i++) {
        replace(text[i].begin(), text[i].end(), alphabet[0], alphabet[1]);  // from ���ڸ� to ���ڷ� �ٲ�
    }
}
void Sorted_Sentense(string text[], int* judge) {
    vector<pair<int, string>> lines;  // �ܾ� ���� ���� �Բ� ������ ����

    // �� ���� �ܾ� ���� �� ��ü�� ���Ϳ� ����
    for (int i = 0; i < 10; i++) {
        lines.push_back({ CountWords(text[i]), text[i] });
    }

    if (*judge == 0) {
        // �������� ����
        sort(lines.begin(), lines.end());
        cout << "�������� ���� �Ϸ�." << endl;

        // ���ĵ� ����� ���� �迭�� �ٽ� ����
        for (int i = 0; i < 10; i++) {
            text[i] = lines[i].second;
        }
        (*judge)++;  // ���¸� ����
    }
    else if (*judge == 1) {
        // �������� ����
        sort(lines.begin(), lines.end(), greater<>());
        cout << "�������� ���� �Ϸ�." << endl;

        // ���ĵ� ����� ���� �迭�� �ٽ� ����
        for (int i = 0; i < 10; i++) {
            text[i] = lines[i].second;
        }
        (*judge)++;  // ���¸� ����
    }

    Print_text(text);  // ��� ���
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