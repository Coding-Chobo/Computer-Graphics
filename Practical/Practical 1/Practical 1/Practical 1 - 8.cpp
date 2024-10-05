#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <gl/glew.h> // 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <ctime>
#include <cstdlib> // rand() 함수를 사용하기 위해 추가
#include <stdio.h>
#include <iomanip> // For debug output precision

#define Max 10
#define Max_vertex 40
#define ROW 1000
#define COL 1000

GLchar* vertexSource, * fragmentSource; // 소스코드 저장 변수
GLuint vertexShader, fragmentShader; // 세이더 객체
GLuint shaderProgramID; // 셰이더 프로그램
GLuint vao, vbo[2]; // VAO와 VBO

//----------------------------- 구조체 선언 -------------------------------

//--------------------------- 전역 변수 ---------------------------------
float xpos{}, ypos{}; // 마우스 클릭 위치 좌표
bool canmake{}; // 도형을 생성할 수 있는지 여부
int currentblockcnt{}; // 현재 생성된 도형 개수
float radius{ 0.15f }; // 도형의 반지름
GLfloat colors[Max][3][3]; // 각 도형의 색상
GLfloat vertex[Max_vertex][3];
int shape[Max];
int currentvertexcnt{};
//----------------------------- 함수 선언 -------------------------------------
void init_color();
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLint width, height;
void InitBuffer();
char* filetobuf(const char* file);
GLvoid Keyboard(unsigned char key, int x, int y);
void Timer(int value); // 타이머 콜백 함수
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void MakeSquare(int point);
void UpdateVBO();
void PrintFigureDebugInfo();
//---------------------------------------------------------------------

// 색상 초기화 함수
void init_color() {
    GLfloat presetColors[3][3] = {
        {1.0f, 0.0f, 0.0f},  // Red
        {0.0f, 1.0f, 0.0f},  // Green
        {0.0f, 0.0f, 1.0f}   // Blue
    };

    // 각 도형의 색상 값을 초기화
    for (int i = 0; i < Max; ++i) {
        for (int j = 0; j < 3; ++j) {
            colors[i][j][0] = presetColors[i % 3][0]; // R value
            colors[i][j][1] = presetColors[i % 3][1]; // G value
            colors[i][j][2] = presetColors[i % 3][2]; // B value
        }
    }
}

// 프로그램의 메인 함수
void main(int argc, char** argv) {
    // 윈도우 생성
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(ROW, COL);
    glutCreateWindow("Practical 1");

    // GLEW 초기화
    glewExperimental = GL_TRUE;
    glewInit();

    init_color(); // 색상 초기화
    InitBuffer(); // 버퍼 초기화
    make_vertexShaders(); // 버텍스 셰이더 생성
    make_fragmentShaders(); // 프래그먼트 셰이더 생성
    shaderProgramID = make_shaderProgram(); // 셰이더 프로그램 생성

    // 콜백 함수 등록
    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutMouseFunc(Mouse);
    glutKeyboardFunc(Keyboard);

    glutMainLoop(); // 메인 루프 진입
}

// 도형을 생성하고 정점 정보를 설정하는 함수
void MakeSquare(int point) {
    if (currentblockcnt >= Max) return; // 최대 도형 개수를 초과하지 않도록 함

    shape[currentblockcnt] = point;
    switch (point) {
    case 1: // 작은 사각형(점)
        vertex[currentvertexcnt][0] = xpos - radius / 3;
        vertex[currentvertexcnt][1] = ypos - radius / 3;
        vertex[currentvertexcnt][2] = 0.0f;
        currentvertexcnt++;
        vertex[currentvertexcnt][0] = xpos - radius / 3;
        vertex[currentvertexcnt][1] = ypos + radius / 3;
        vertex[currentvertexcnt][2] = 0.0f;
        currentvertexcnt++;
        vertex[currentvertexcnt][0] = xpos + radius / 3;
        vertex[currentvertexcnt][1] = ypos + radius / 3;
        vertex[currentvertexcnt][2] = 0.0f;
        currentvertexcnt++;
        vertex[currentvertexcnt][0] = xpos + radius / 3;
        vertex[currentvertexcnt][1] = ypos - radius / 3;
        vertex[currentvertexcnt][2] = 0.0f;
        currentvertexcnt++;
        break;

    case 2: // 선
        vertex[currentvertexcnt][0] = xpos - radius;
        vertex[currentvertexcnt][1] = ypos - radius;
        vertex[currentvertexcnt][2] = 0.0f;
        currentvertexcnt++;
        vertex[currentvertexcnt][0] = xpos + radius;
        vertex[currentvertexcnt][1] = ypos + radius;
        vertex[currentvertexcnt][2] = 0.0f;
        currentvertexcnt++;
        break;

    case 3: // 삼각형
        vertex[currentvertexcnt][0] = xpos;
        vertex[currentvertexcnt][1] = ypos + radius;
        vertex[currentvertexcnt][2] = 0.0f;
        currentvertexcnt++;
        vertex[currentvertexcnt][0] = xpos - radius * 0.85;
        vertex[currentvertexcnt][1] = ypos - radius / 2;
        vertex[currentvertexcnt][2] = 0.0f;
        currentvertexcnt++;
        vertex[currentvertexcnt][0] = xpos + radius * 0.85;
        vertex[currentvertexcnt][1] = ypos - radius / 2;
        vertex[currentvertexcnt][2] = 0.0f;
        currentvertexcnt++;
        break;

    case 4: // 사각형
        vertex[currentvertexcnt][0] = xpos - radius;
        vertex[currentvertexcnt][1] = ypos - radius;
        vertex[currentvertexcnt][2] = 0.0f;
        currentvertexcnt++;
        vertex[currentvertexcnt][0] = xpos - radius;
        vertex[currentvertexcnt][1] = ypos + radius;
        vertex[currentvertexcnt][2] = 0.0f;
        currentvertexcnt++;
        vertex[currentvertexcnt][0] = xpos + radius;
        vertex[currentvertexcnt][1] = ypos + radius;
        vertex[currentvertexcnt][2] = 0.0f;
        currentvertexcnt++;
        vertex[currentvertexcnt][0] = xpos + radius;
        vertex[currentvertexcnt][1] = ypos - radius;
        vertex[currentvertexcnt][2] = 0.0f;
        currentvertexcnt++;
        break;

    default:
        break;
    }

    currentblockcnt++; // 도형 개수 증가

    // VBO 데이터를 업데이트하여 GPU에 다시 전송
    UpdateVBO();

    // 화면을 다시 그리도록 요청
    glutPostRedisplay();
}

// VAO 및 VBO 초기화
void InitBuffer() {
    glGenVertexArrays(1, &vao); // VAO 생성
    glBindVertexArray(vao); // VAO 바인딩

    glGenBuffers(2, vbo); // 2개의 VBO 생성

    // 정점 좌표 데이터를 VBO에 복사
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // 색상 데이터를 VBO에 복사
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
}

// VBO 데이터를 업데이트하는 함수
void UpdateVBO() {
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex), vertex); // GPU에 새로운 정점 데이터 전송
}

// 화면 그리기 콜백 함수
GLvoid drawScene() {
    // 배경색 설정
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramID); // 셰이더 프로그램 사용

    glBindVertexArray(vao); // VAO 바인딩

    // 도형 그리기: 각 도형의 정점 개수에 따라 다르게 그려야 함
    int offset = 0;
    for (int i = 0; i < currentblockcnt; ++i) {
        if (shape[i] == 4) {
            glDrawArrays(GL_TRIANGLE_FAN, offset, 4); // 사각형: 4개의 정점
            offset += 4;
        }
        else if (shape[i] == 2) {
            glDrawArrays(GL_LINES, offset, 2); // 선: 2개의 정점
            offset += 2;
        }
        else if (shape[i] == 3) {
            glDrawArrays(GL_TRIANGLES, offset, 3); // 삼각형: 3개의 정점
            offset += 3;
        }
        else if (shape[i] == 1) {
            glDrawArrays(GL_TRIANGLE_FAN, offset, 4); // 작은 사각형: 4개의 정점
            offset += 4;
        }
    }

    glutSwapBuffers(); // 화면에 출력
}

// 윈도우 리사이즈 콜백 함수
GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

// 마우스 클릭 이벤트 콜백 함수
void Mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        canmake = true;
        xpos = (static_cast<float>(x) / (ROW / 2.0f)) - 1.0f; // OpenGL 좌표로 변환
        ypos = 1.0f - (static_cast<float>(y) / (COL / 2.0f)); // OpenGL 좌표로 변환
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        canmake = false;
    }
}

// 키보드 입력 이벤트 콜백 함수
GLvoid Keyboard(unsigned char key, int x, int y) {
    if (canmake) {
        switch (key) {
        case 'p': MakeSquare(1); break; // 작은 사각형(점) 생성
        case 'l': MakeSquare(2); break; // 선 생성
        case 't': MakeSquare(3); break; // 삼각형 생성
        case 'r': MakeSquare(4); break; // 사각형 생성
        }
    }
    int edited_vertex{};
    int select_figure{};
    if (currentblockcnt > 0)
    {
        switch (key)
        {
        case 'w':
            edited_vertex = 0;
            select_figure = rand() % currentblockcnt;  // 바뀌는 도형 인덱스
            for (int i = 0; i < select_figure; i++)
            {
                if (shape[i] == 1)
                {
                    edited_vertex += shape[i] * 4;
                }
                else
                {
                    edited_vertex += shape[i];
                }

            }
            if (shape[select_figure] == 1)
            {
                for (int i = 0; i < 4; i++) // 바뀌는 도형의 버텍스 수
                {
                    vertex[edited_vertex + i][1] += radius;
                }
            }
            else
            {
                for (int i = 0; i < shape[select_figure]; i++) // 바뀌는 도형의 버텍스 수
                {
                    vertex[edited_vertex + i][1] += radius;
                }
            }
            break;
        case 'a':
            edited_vertex = 0;
            select_figure = rand() % currentblockcnt;  // 바뀌는 도형 인덱스
            for (int i = 0; i < select_figure; i++)
            {
                if (shape[i] == 1)
                {
                    edited_vertex += shape[i] * 4;
                }
                else
                {
                    edited_vertex += shape[i];
                }
            }
            if (shape[select_figure] == 1)
            {
                for (int i = 0; i < 4; i++) // 바뀌는 도형의 버텍스 수
                {
                    vertex[edited_vertex + i][0] -= radius;
                }
            }
            else
            {
                for (int i = 0; i < shape[select_figure]; i++) // 바뀌는 도형의 버텍스 수
                {
                    vertex[edited_vertex + i][0] -= radius;
                }
            }
            break;
        case 's':
            edited_vertex = 0;
            select_figure = rand() % currentblockcnt;  // 바뀌는 도형 인덱스
            for (int i = 0; i < select_figure; i++)
            {
                if (shape[i] == 1)
                {
                    edited_vertex += shape[i] * 4;
                }
                else
                {
                    edited_vertex += shape[i];
                }
            }
            if (shape[select_figure] == 1)
            {
                for (int i = 0; i < 4; i++) // 바뀌는 도형의 버텍스 수
                {
                    vertex[edited_vertex + i][1] -= radius;
                }
            }
            else
            {
                for (int i = 0; i < shape[select_figure]; i++) // 바뀌는 도형의 버텍스 수
                {
                    vertex[edited_vertex + i][1] -= radius;
                }
            }
            break;
        case 'd':
            edited_vertex = 0;
            select_figure = rand() % currentblockcnt;  // 바뀌는 도형 인덱스
            for (int i = 0; i < select_figure; i++)
            {
                if (shape[i] == 1)
                {
                    edited_vertex += shape[i] * 4;
                }
                else
                {
                    edited_vertex += shape[i];
                }
            }
            if (shape[select_figure] == 1)
            {
                for (int i = 0; i < 4; i++) // 바뀌는 도형의 버텍스 수
                {
                    vertex[edited_vertex + i][0] += radius;
                }
            }
            else
            {
                for (int i = 0; i < shape[select_figure]; i++) // 바뀌는 도형의 버텍스 수
                {
                    vertex[edited_vertex + i][0] += radius;
                }
            }

            break;
        case 'c':
            currentblockcnt = 0;
            currentvertexcnt = 0;
            ZeroMemory(vertex, 100);

        default:
            break;

        }
    }

    // VBO 데이터를 업데이트하여 GPU에 다시 전송
    UpdateVBO();

    // 화면을 다시 그리도록 요청
    glutPostRedisplay();
}

// 셰이더 프로그램 생성 함수
GLuint make_shaderProgram() {
    GLuint shaderID = glCreateProgram();
    glAttachShader(shaderID, vertexShader);
    glAttachShader(shaderID, fragmentShader);
    glLinkProgram(shaderID);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderID;
}

// 버텍스 셰이더 생성 함수
void make_vertexShaders() {
    vertexSource = filetobuf("vertex.vs");
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
    glCompileShader(vertexShader);
}

// 프래그먼트 셰이더 생성 함수
void make_fragmentShaders() {
    fragmentSource = filetobuf("fragment.fs");
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
    glCompileShader(fragmentShader);
}

// 파일을 버퍼로 읽어오는 함수
char* filetobuf(const char* file) {
    FILE* fptr;
    long length;
    char* buf;

    fptr = fopen(file, "rb");
    if (!fptr) return NULL;

    fseek(fptr, 0, SEEK_END);
    length = ftell(fptr);
    buf = (char*)malloc(length + 1);
    fseek(fptr, 0, SEEK_SET);
    fread(buf, length, 1, fptr);
    fclose(fptr);

    buf[length] = 0;
    return buf;
}
