#include <iostream>
#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <ctime>
#include <cstdlib> // rand() 함수를 사용하기 위해 추가
#include <stdio.h>
#define COL 1000
#define ROW 1000
#define MAX 30

struct  Square
{
    float x1{};
    float y1{};
    float x2{};
    float y2{};
    bool is_surv{};
};


GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);

void MakeSquare();
void Timer(int value); // 타이머 콜백 함수
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void drawRect();
bool IsRectOverlap(Square rect1, Square rect2);
void passiveMotion(int x, int y);
bool timerRunning = false; // 타이머 상태를 저장하는 변수
int currentBlcokcnt{};
int selectedBlock{ MAX };

float previousMouseX, previousMouseY;

Square square[MAX]{};
Square eraser{};
float colors[MAX][3]{};
float bgColor[3] = { 1.0f, 1.0f, 1.0f }; // 배경 색상 

float gap_x{};
float gap_y{};

int main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정 
{
    srand(time(NULL));
    //--- 윈도우 생성하기
    glutInit(&argc, argv); // glut 초기화
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
    glutInitWindowPosition(100, 100); // 윈도우의 위치 지정
    glutInitWindowSize(ROW, COL); // 윈도우의 크기 지정
    glutCreateWindow("Example1"); // 윈도우 생성

    //--- GLEW 초기화하기
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { // glew 초기화
        std::cerr << "Unable to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }
    else {
        std::cout << "GLEW Initialized\n";
    }
    MakeSquare();
    glutDisplayFunc(drawScene); // 출력 함수의 지정
    glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
    glutPassiveMotionFunc(passiveMotion);
    glutMotionFunc(Motion);
    glutMouseFunc(Mouse);
    glutKeyboardFunc(Keyboard);
    glutMainLoop(); // 이벤트 처리 시작
    return 0;
}

void passiveMotion(int x, int y) {
    // 좌표 변환
    float xpos = static_cast<float>(x) / (ROW / 2.0f) - 1.0f;
    float ypos = 1.0f - static_cast<float>(y) / (COL / 2.0f);
    std::cout << "x = " << x << " y = " << y << " -> xpos = " << xpos << " ypos = " << ypos << std::endl;
    std::cout << currentBlcokcnt << std::endl;
}

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수 
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수 
{
    glClearColor(bgColor[0], bgColor[1], bgColor[2], 1.0f); // 배경색을 설정
    glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
    drawRect();
    glutSwapBuffers(); // 화면에 출력하기
}

void Mouse(int button, int state, int x, int y)
{
    // OpenGL 좌표로 변환 (화면의 크기를 고려)
    float xpos = (static_cast<float>(x) / (ROW / 2.0f)) - 1.0f;
    float ypos = 1.0f - (static_cast<float>(y) / (COL / 2.0f));
    eraser.is_surv = true;
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        for (int i = MAX - 1; i >= 0; i--) {
            // 사각형 내부 클릭 여부 확인 
            if (square[i].is_surv) {
                if ((xpos > square[i].x1) && (xpos < square[i].x2) && (ypos > square[i].y1) && (ypos < square[i].y2)) {
                    selectedBlock = i;
                    square[i].x1 -= ((square[i].x2 - square[i].x1) / 2);
                    square[i].x2 += ((square[i].x2 - square[i].x1) / 2);
                    square[i].y1 -= ((square[i].y2 - square[i].y1) / 2);
                    square[i].y2 += ((square[i].y2 - square[i].y1) / 2);
                    previousMouseX = xpos;
                    previousMouseY = ypos;
                    eraser.is_surv = false;
                    break;
                }
            }
        }
        if (eraser.is_surv)
        {
            previousMouseX = xpos;
            previousMouseY = ypos;
            eraser.x1 = xpos - gap_x;
            eraser.x2 = xpos + gap_x;
            eraser.y1 = ypos - gap_y;
            eraser.y2 = ypos + gap_y;
        }
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        square[selectedBlock].is_surv = false;
        eraser.is_surv = false;
        gap_x = (square[selectedBlock].x2 - square[selectedBlock].x1) / 2;
        gap_y = (square[selectedBlock].y2 - square[selectedBlock].y1) / 2;
        selectedBlock = MAX;
    }
    glutPostRedisplay(); // 화면 갱신
}
bool IsRectOverlap(Square rect1, Square rect2) {
    if (rect1.x2 <= rect2.x1 || rect1.x1 >= rect2.x2) {
        return false;
    }
    if (rect1.y2 <= rect2.y1 || rect1.y1 >= rect2.y2) {
        return false;
    }
    // 두 축 모두에서 겹친다면 충돌이 발생한 것
    return true;
}
void drawRect() {
    for (int i = 0; i < MAX; ++i) {
        if (square[i].is_surv)
        {
            glColor3f(colors[i][0], colors[i][1], colors[i][2]);
            glRectf(square[i].x1, square[i].y1, square[i].x2, square[i].y2);
        }
    }
    if (eraser.is_surv)
    {
        glColor3f(0.0f,0.0f,0.0f);
        glRectf(eraser.x1, eraser.y1, eraser.x2, eraser.y2);
    }
}
void Motion(int x, int y) {
    std::cout << selectedBlock << std::endl;
    if (selectedBlock < MAX) { // 선택된 블록이 있을 때만 동작
        // OpenGL 좌표로 변환
        float xpos = static_cast<float>(x) / (ROW / 2.0f) - 1.0f;
        float ypos = 1.0f - static_cast<float>(y) / (COL / 2.0f);

        // 마우스 이동량 계산
        float deltaX = xpos - previousMouseX;
        float deltaY = ypos - previousMouseY;

        // 선택된 블록 이동
        square[selectedBlock].x1 += deltaX;
        square[selectedBlock].y1 += deltaY;
        square[selectedBlock].x2 += deltaX;
        square[selectedBlock].y2 += deltaY;
        for (int i = 0; i < MAX; i++)
        {
            if (IsRectOverlap(square[i], square[selectedBlock]) && i != selectedBlock && square[i].is_surv)
            {
                square[i].is_surv = false;
                colors[selectedBlock][0] = colors[i][0];
                colors[selectedBlock][1] = colors[i][1];
                colors[selectedBlock][2] = colors[i][2];
            }
        }
        // 마우스 위치 업데이트
c
    }
    else if (eraser.is_surv)
    {
        // OpenGL 좌표로 변환
        float xpos = static_cast<float>(x) / (ROW / 2.0f) - 1.0f;
        float ypos = 1.0f - static_cast<float>(y) / (COL / 2.0f);

        // 마우스 이동량 계산
        float deltaX = xpos - previousMouseX;
        float deltaY = ypos - previousMouseY;

        // 선택된 블록 이동
        eraser.x1 += deltaX;
        eraser.y1 += deltaY;
        eraser.x2 += deltaX;
        eraser.y2 += deltaY;
        for (int i = 0; i < MAX; i++)
        {
            if (IsRectOverlap(square[i], eraser) && square[i].is_surv)
            {
                square[i].is_surv = false;
            }
        }
        // 마우스 위치 업데이트
        previousMouseX = xpos;
        previousMouseY = ypos;

        glutPostRedisplay(); // 화면 갱신
    }
}
GLvoid Keyboard(unsigned char key, int x, int y)
{
    switch (key) {

    case 'r':
    {
        MakeSquare();
        glutSwapBuffers(); // 버퍼 교체 (화면 갱신)
        glutPostRedisplay(); // 화면 갱신
    }
    break;
    }
}
void MakeSquare() {
    for (int i = 0; i < MAX; i++)
    {    
        
        int decide_sign = rand() % 2;
        if (decide_sign == 0)
        {
            square[i].x1 = static_cast<float>(rand()) / RAND_MAX;
        }
        else
        {
            square[i].x1 = (static_cast<float>(rand()) / RAND_MAX) * (-1);
        }
        decide_sign = rand() % 2;
        if (decide_sign == 0)
        {
            square[i].y1 = static_cast<float>(rand()) / RAND_MAX;
        }
        else
        {
            square[i].y1 = (static_cast<float>(rand()) / RAND_MAX) * (-1);
        }

        square[i].x2 = square[i].x1 + static_cast<float>(rand()) / RAND_MAX / 6;
        square[i].y2 = square[i].y1 + static_cast<float>(rand()) / RAND_MAX / 6;
        colors[i][0] = static_cast<float>(rand()) / RAND_MAX;
        colors[i][1] = static_cast<float>(rand()) / RAND_MAX;
        colors[i][2] = static_cast<float>(rand()) / RAND_MAX;
        square[i].is_surv = true;
    }
}