#include <iostream>
#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <ctime>
#include <cstdlib> // rand() 함수를 사용하기 위해 추가
#include <stdio.h>
#define COL 1000
#define ROW 1000
#define MAX 8

struct  Square
{
    float x1{};
    float y1{};
    float x2{};
    float y2{};
    bool is_surv{};
    int mode{};
    int direction2{};
};


GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
void Timer(int value);
void MakeSquare();
void Timer(int value); // 타이머 콜백 함수
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void drawRect();
bool IsRectOverlap(Square rect1, Square rect2);
void passiveMotion(int x, int y);
int currentBlcokcnt{};
int selectedBlock{ MAX };

float previousMouseX, previousMouseY;

Square square[MAX]{};
Square separeted_square[MAX][8];
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
    glutInitWindowPosition(0, 0); // 윈도우의 위치 지정
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
    glutTimerFunc(50, Timer, 0);
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

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        for (int i = MAX - 1; i >= 0; i--) {
            // 사각형 내부 클릭 여부 확인 
            if (square[i].is_surv) {
                if ((xpos > square[i].x1) && (xpos < square[i].x2) && (ypos > square[i].y1) && (ypos < square[i].y2)) {
                    square[i].mode = rand() % 4;
                    if (square[i].mode == 3)
                    {
                        for (int j = 0; j < 8; j++)
                        {
                            separeted_square[i][j].is_surv = true;
                        }
                        separeted_square[i][0].x1 = square[i].x1;
                        separeted_square[i][0].x2 = xpos;
                        separeted_square[i][0].y1 = ypos;
                        separeted_square[i][0].y2 = square[i].y2;


                        separeted_square[i][1].x1 = xpos;
                        separeted_square[i][1].x2 = square[i].x2;
                        separeted_square[i][1].y1 = ypos;
                        separeted_square[i][1].y2 = square[i].y2;


                        separeted_square[i][2].x1 = square[i].x1;
                        separeted_square[i][2].x2 = xpos;
                        separeted_square[i][2].y1 = square[i].y1;
                        separeted_square[i][2].y2 = ypos;


                        separeted_square[i][3].x1 = xpos;
                        separeted_square[i][3].x2 = square[i].x2;
                        separeted_square[i][3].y1 = square[i].y1;
                        separeted_square[i][3].y2 = ypos;

                        separeted_square[i][4].x1 = square[i].x1;
                        separeted_square[i][4].x2 = xpos;
                        separeted_square[i][4].y1 = ypos;
                        separeted_square[i][4].y2 = square[i].y2;


                        separeted_square[i][5].x1 = xpos;
                        separeted_square[i][5].x2 = square[i].x2;
                        separeted_square[i][5].y1 = ypos;
                        separeted_square[i][5].y2 = square[i].y2;


                        separeted_square[i][6].x1 = square[i].x1;
                        separeted_square[i][6].x2 = xpos;
                        separeted_square[i][6].y1 = square[i].y1;
                        separeted_square[i][6].y2 = ypos;


                        separeted_square[i][7].x1 = xpos;
                        separeted_square[i][7].x2 = square[i].x2;
                        separeted_square[i][7].y1 = square[i].y1;
                        separeted_square[i][7].y2 = ypos;

                        separeted_square[i][0].mode = 0;
                        separeted_square[i][0].direction2 = 2;
                        separeted_square[i][1].mode = 1;
                        separeted_square[i][1].direction2 = 2;
                        separeted_square[i][2].mode = 0;
                        separeted_square[i][2].direction2 = 3;
                        separeted_square[i][3].mode = 1;
                        separeted_square[i][3].direction2 = 3;

                        for (int j = 4; j < 8; j++)
                        {
                            separeted_square[i][j].mode = j-4;
                        }
                    }
                    else
                    {
                        for (int j = 0; j < 4; j++)
                        {
                            separeted_square[i][j].is_surv = true;
                        }
                        if (square[i].mode == 1)
                        {
                            separeted_square[i][0].mode = 0;
                            separeted_square[i][0].direction2 = 2;
                            separeted_square[i][1].mode = 1;
                            separeted_square[i][1].direction2 = 2;
                            separeted_square[i][2].mode = 0;
                            separeted_square[i][2].direction2 = 3;
                            separeted_square[i][3].mode = 1;
                            separeted_square[i][3].direction2 = 3;
                        }
                        else if (square[i].mode == 0)
                        {
                            for (int j = 0; j < 4; j++)
                            {
                                separeted_square[i][j].mode = j;
                            }
                        }
                        else
                        {
                            int dcmode = rand() % 4;
                            for (int j = 0; j < 4; j++)
                            {
                                separeted_square[i][j].mode = dcmode;
                            }
                        }
                        separeted_square[i][0].x1 = square[i].x1;
                        separeted_square[i][0].x2 = xpos;
                        separeted_square[i][0].y1 = ypos;
                        separeted_square[i][0].y2 = square[i].y2;

                        
                        separeted_square[i][1].x1 = xpos;
                        separeted_square[i][1].x2 = square[i].x2;
                        separeted_square[i][1].y1 = ypos;
                        separeted_square[i][1].y2 = square[i].y2;


                        separeted_square[i][2].x1 = square[i].x1;
                        separeted_square[i][2].x2 = xpos;
                        separeted_square[i][2].y1 = square[i].y1;
                        separeted_square[i][2].y2 = ypos;


                        separeted_square[i][3].x1 = xpos;
                        separeted_square[i][3].x2 = square[i].x2;
                        separeted_square[i][3].y1 = square[i].y1;
                        separeted_square[i][3].y2 = ypos;

                    }
                    square[i].is_surv = false;
                    break;
                }
            }
        }
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
        if (square[i].mode == 3)
        {
            for (int j = 0; j < 8; j++)
            {
                if (separeted_square[i][j].is_surv)
                {
                    glColor3f(colors[i][0], colors[i][1], colors[i][2]);
                    glRectf(separeted_square[i][j].x1, separeted_square[i][j].y1, separeted_square[i][j].x2, separeted_square[i][j].y2);
                }
            }
        }
        else
        {
            for (int j = 0; j < 4; j++)
            {
                if (separeted_square[i][j].is_surv)
                {
                    glColor3f(colors[i][0], colors[i][1], colors[i][2]);
                    glRectf(separeted_square[i][j].x1, separeted_square[i][j].y1, separeted_square[i][j].x2, separeted_square[i][j].y2);
                }
            }
        }
    }

}
void Motion(int x, int y) {
    std::cout << selectedBlock << std::endl;

    glutPostRedisplay(); // 화면 갱신

}
GLvoid Keyboard(unsigned char key, int x, int y)
{

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

        square[i].x2 = square[i].x1 + static_cast<float>(rand()) / RAND_MAX + 0.1f;
        square[i].y2 = square[i].y1 + static_cast<float>(rand()) / RAND_MAX + 0.1f;
        colors[i][0] = static_cast<float>(rand()) / RAND_MAX;
        colors[i][1] = static_cast<float>(rand()) / RAND_MAX;
        colors[i][2] = static_cast<float>(rand()) / RAND_MAX;
        square[i].is_surv = true;
    }
}
void Timer(int value) {
    float moving_gap = 0.05f;
    for (int i = 0; i < MAX; i++)
    {
        if (square[i].mode == 3)
        {
            for (int j = 0; j < 8; j++) {
                if (separeted_square[i][j].is_surv)
                {
                    if (j < 4)
                    {
                        // 좌우
                        switch (separeted_square[i][j].mode)
                        {
                        case 1: // 우측 이동
                            if (separeted_square[i][j].x2 + moving_gap < 1.0f)
                            {
                                separeted_square[i][j].x1 += moving_gap;
                                separeted_square[i][j].x2 += moving_gap;
                            }
                            else
                            {
                                separeted_square[i][j].mode = 0;
                            }
                            break;
                        case 0: // 좌측 이동
                            if (separeted_square[i][j].x1 - moving_gap > -1.0f)
                            {
                                separeted_square[i][j].x1 -= moving_gap;
                                separeted_square[i][j].x2 -= moving_gap;
                            }
                            else
                            {
                                separeted_square[i][j].mode = 1;
                            }
                            break;
                        }
                        //상하
                        switch (separeted_square[i][j].direction2)
                        {
                        case 2: // 위로 이동
                            if (separeted_square[i][j].y2 + moving_gap < 1.0f)
                            {
                                separeted_square[i][j].y1 += moving_gap;
                                separeted_square[i][j].y2 += moving_gap;
                            }
                            else
                            {
                                separeted_square[i][j].direction2 = 3;
                            }
                            break;
                        case 3: // 아래로 이동
                            if (separeted_square[i][j].y1 - moving_gap > -1.0f)
                            {
                                separeted_square[i][j].y1 -= moving_gap;
                                separeted_square[i][j].y2 -= moving_gap;
                            }
                            else
                            {
                                separeted_square[i][j].direction2 = 2;
                            }
                        }
                    }
                    else
                    {
                        switch (separeted_square[i][j].mode) {
                        case 1: // 우측 이동
                            if (separeted_square[i][j].x2 + moving_gap < 1.0f)
                            {
                                separeted_square[i][j].x1 += moving_gap;
                                separeted_square[i][j].x2 += moving_gap;
                            }
                            else
                            {
                                separeted_square[i][j].mode = 0;
                            }
                            break;
                        case 0: // 좌측 이동
                            if (separeted_square[i][j].x1 - moving_gap > -1.0f)
                            {
                                separeted_square[i][j].x1 -= moving_gap;
                                separeted_square[i][j].x2 -= moving_gap;
                            }
                            else
                            {
                                separeted_square[i][j].mode = 1;
                            }
                            break;

                        case 2: // 위로 이동
                            if (separeted_square[i][j].y2 + moving_gap < 1.0f)
                            {
                                separeted_square[i][j].y1 += moving_gap;
                                separeted_square[i][j].y2 += moving_gap;
                            }
                            else
                            {
                                separeted_square[i][j].mode = 3;
                            }
                            break;
                        case 3: // 아래로 이동
                            if (separeted_square[i][j].y1 - moving_gap > -1.0f)
                            {
                                separeted_square[i][j].y1 -= moving_gap;
                                separeted_square[i][j].y2 -= moving_gap;
                            }
                            else
                            {
                                separeted_square[i][j].mode = 2;
                            }
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            for (int j = 0; j < 4; j++) {
                if (separeted_square[i][j].is_surv)
                {
                    if (separeted_square[i][j].x2 - separeted_square[i][j].x1 < 0.02f || separeted_square[i][j].y2 - separeted_square[i][j].y1 < 0.02f)
                    {
                        separeted_square[i][j].is_surv = false;
                    }
                    else
                    {
                        separeted_square[i][j].x1 += 0.004;
                        separeted_square[i][j].x2 -= 0.004;
                        separeted_square[i][j].y1 += 0.004;
                        separeted_square[i][j].y2 -= 0.004; 
                    }


                    switch (square[i].mode)
                    {
                    case 0: case 2:
                        switch (separeted_square[i][j].mode) {
                        case 1: // 우측 이동
                            if (separeted_square[i][j].x2 + moving_gap < 1.0f)
                            {
                                separeted_square[i][j].x1 += moving_gap;
                                separeted_square[i][j].x2 += moving_gap;
                            }
                            else
                            {
                                separeted_square[i][j].mode = 0;
                            }
                            break;
                        case 0: // 좌측 이동
                            if (separeted_square[i][j].x1 - moving_gap > -1.0f)
                            {
                                separeted_square[i][j].x1 -= moving_gap;
                                separeted_square[i][j].x2 -= moving_gap;
                            }
                            else
                            {
                                separeted_square[i][j].mode = 1;
                            }
                            break;

                        case 2: // 위로 이동
                            if (separeted_square[i][j].y2 + moving_gap < 1.0f)
                            {
                                separeted_square[i][j].y1 += moving_gap;
                                separeted_square[i][j].y2 += moving_gap;
                            }
                            else
                            {
                                separeted_square[i][j].mode = 3;
                            }
                            break;
                        case 3: // 아래로 이동
                            if (separeted_square[i][j].y1 - moving_gap > -1.0f)
                            {
                                separeted_square[i][j].y1 -= moving_gap;
                                separeted_square[i][j].y2 -= moving_gap;
                            }
                            else
                            {
                                separeted_square[i][j].mode = 2;
                            }
                            break;
                        }
                        break;
                    case 1: //-- 대각선 운동
                        // 좌우
                        switch (separeted_square[i][j].mode)
                        {
                        case 1: // 우측 이동
                            if (separeted_square[i][j].x2 + moving_gap < 1.0f)
                            {
                                separeted_square[i][j].x1 += moving_gap;
                                separeted_square[i][j].x2 += moving_gap;
                            }
                            else
                            {
                                separeted_square[i][j].mode = 0;
                            }
                            break;
                        case 0: // 좌측 이동
                            if (separeted_square[i][j].x1 - moving_gap > -1.0f)
                            {
                                separeted_square[i][j].x1 -= moving_gap;
                                separeted_square[i][j].x2 -= moving_gap;
                            }
                            else
                            {
                                separeted_square[i][j].mode = 1;
                            }
                            break;
                        }
                        //상하
                        switch (separeted_square[i][j].direction2)
                        {
                        case 2: // 위로 이동
                            if (separeted_square[i][j].y2 + moving_gap < 1.0f)
                            {
                                separeted_square[i][j].y1 += moving_gap;
                                separeted_square[i][j].y2 += moving_gap;
                            }
                            else
                            {
                                separeted_square[i][j].direction2 = 3;
                            }
                            break;
                        case 3: // 아래로 이동
                            if (separeted_square[i][j].y1 - moving_gap > -1.0f)
                            {
                                separeted_square[i][j].y1 -= moving_gap;
                                separeted_square[i][j].y2 -= moving_gap;
                            }
                            else
                            {
                                separeted_square[i][j].direction2 = 2;
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
    glutPostRedisplay(); // 화면 갱신
    glutTimerFunc(100, Timer, value);
}