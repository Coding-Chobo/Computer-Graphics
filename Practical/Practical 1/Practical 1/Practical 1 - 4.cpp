#include <iostream>
#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <ctime>
#include <cstdlib> // rand() 함수를 사용하기 위해 추가

#define COL 800
#define ROW 800
#define MAX 5
struct SQUARE {
    float x{};
    float y{};
    float xgap{};
    float ygap{};
    int direction{};
    int direction2{};
};
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
void Timer(int value); // 타이머 콜백 함수
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void drawRect();
void ResetRect();
void passiveMotion(int x, int y);
bool timerRunning = false; // 타이머 상태를 저장하는 변수
int timer_mode{};
int currentBlcokcnt{};
SQUARE square[MAX] = {};
SQUARE temp_square[MAX] = {};
float colors[MAX][3] = { {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 1.0, 0.0} }; // 사각형 색상
float bgColor[3] = { 0.2f, 0.2f, 0.2f }; // 배경 색상 


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
    std::cout << "x = " << x << " y = " << y << " -> xpos = " << xpos << " ypos = " << currentBlcokcnt << std::endl;
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
void drawRect() {
    for (int i = 0; i < currentBlcokcnt; ++i) {
        glColor3f(colors[i][0], colors[i][1], colors[i][2]);
        glRectf(square[i].x - square[i].xgap, square[i].y - square[i].ygap, square[i].x + square[i].xgap, square[i].y + square[i].ygap);
    }
}
void ResetRect() {
    for (int i = 0; i < MAX; i++)
    {
        square[i] = {};
        temp_square[i] = {};
    }
    currentBlcokcnt = 0;
}
void Motion(int x, int y) {
}
void Timer(int value) {
    if (timerRunning) { // 타이머가 실행 중인 경우에만 실행
        float moving_gap = { 0.05f };
        if (timer_mode == 1)
        {
            for (int i = 0; i < currentBlcokcnt; i++)
            {
                switch (square[i].direction)
                {
                case 0: // 우측 이동
                    if (square[i].x + square[i].xgap + moving_gap < 1.0f)
                    {
                        square[i].x += moving_gap;
                    }
                    else
                    {
                        square[i].direction = 1;
                    }
                    break;
                case 1: // 좌측 이동
                    if (square[i].x - square[i].xgap - moving_gap > -1.0f)
                    {
                        square[i].x -= moving_gap;
                    }
                    else
                    {
                        square[i].direction = 0;
                    }
                    break;
                default:
                    break;
                }

                switch (square[i].direction2)
                {
                case 0: // 아래로 이동
                    if (square[i].y - square[i].ygap - moving_gap > -1.0f)
                    {
                        square[i].y -= moving_gap;
                    }
                    else
                    {
                        square[i].direction2 = 1;
                    }
                    break;
                case 1: // 위로 이동
                    if (square[i].y + square[i].ygap + moving_gap < 1.0f)
                    {
                        square[i].y += moving_gap;
                    }
                    else
                    {
                        square[i].direction2 = 0;
                    }
                    break;
                default:
                    break;
                }
            }
        }
        else if (timer_mode == 2)
        {
            for (int i = 0; i < currentBlcokcnt; i++)
            {
                switch (square[i].direction)
                {
                case 0: // 우측 이동
                    if (square[i].x + square[i].xgap + moving_gap < 1.0f)
                    {
                        square[i].x += moving_gap;
                    }
                    else
                    {
                        if (square[i].y - square[i].ygap - 2 * moving_gap > -1.0f) {
                            square[i].y -= 2 * moving_gap;
                        }
                        square[i].direction = 2;
                    }
                    break;
                case 1: // 아래로 이동
                    if (square[i].y - square[i].ygap - moving_gap > -1.0f)
                    {
                        square[i].y -= moving_gap;
                    }
                    else 
                    {
                        if (square[i].x + square[i].xgap + 2 * moving_gap < 1.0f) {
                            square[i].x += 2 * moving_gap;
                        }
                        square[i].direction = 3;
                    }
                    break;
                case 2: // 좌측 이동
                    if (square[i].x - square[i].xgap - moving_gap > -1.0f)
                    {
                        square[i].x -= moving_gap;
                    }
                    else if (square[i].y - square[i].ygap - 2 * moving_gap > -1.0f)
                    {
                        square[i].y -= 2 * moving_gap;
                        square[i].direction = 0;
                    }
                    else
                    {
                        square[i].direction = 3;
                    }
                    break;
                case 3: // 위로 이동
                    if (square[i].y + square[i].ygap + moving_gap < 1.0f)
                    {
                        square[i].y += moving_gap;
                    }
                    else if (square[i].x + square[i].xgap + 2 * moving_gap < 1.0f)
                    {
                        square[i].x += 2 * moving_gap;
                        square[i].direction = 1;
                    }
                    else
                    {
                        square[i].direction = 2;
                    }
                    break;
                default:
                    break;
                }
            }
        } //-- 지그재그 움직임
        glutPostRedisplay(); // 화면 갱신
        glutTimerFunc(100, Timer, value);
    }
}
void Mouse(int button, int state, int x, int y)
{
    // OpenGL 좌표로 변환
    float xpos = (static_cast<float>(x) / (ROW / 2.0f)) - 1.0f;
    float ypos = 1.0f - (static_cast<float>(y) / (COL / 2.0f));
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (currentBlcokcnt < 5)
        {
            square[currentBlcokcnt].x = xpos;
            square[currentBlcokcnt].y = ypos;
            square[currentBlcokcnt].xgap = 0.1f;
            square[currentBlcokcnt].ygap = 0.1f;
            colors[currentBlcokcnt][0] = static_cast<float>(rand()) / RAND_MAX;
            colors[currentBlcokcnt][1] = static_cast<float>(rand()) / RAND_MAX;
            colors[currentBlcokcnt][2] = static_cast<float>(rand()) / RAND_MAX;
            temp_square[currentBlcokcnt] = square[currentBlcokcnt];
            currentBlcokcnt++;
        }
    }
    glutPostRedisplay(); // 화면 갱신
}
GLvoid Keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case '1':
        timerRunning = true;
        timer_mode = 1;
        for (int i = 0; i < currentBlcokcnt; i++)
        {
            square[i].direction = rand() % 2;
            square[i].direction2 = rand() % 2;
        }
        glutTimerFunc(50, Timer, 0);
    break;
    case '2':
        timerRunning = true;
        timer_mode = 2;
        for (int i = 0; i < currentBlcokcnt; i++)
        {
            square[i].direction = rand() % 4;
        }
        glutTimerFunc(50, Timer, 0);
        break;
    case '3':
        for (int i = 0; i < currentBlcokcnt; i++)
        {
            square[i].xgap = (static_cast<float>(rand()) / RAND_MAX) / 4;
            square[i].ygap = (static_cast<float>(rand()) / RAND_MAX) / 4;
        }

        break;
    case '4':
        for (int i = 0; i < currentBlcokcnt; i++)
        {
            colors[i][0] = static_cast<float>(rand()) / RAND_MAX;
            colors[i][1] = static_cast<float>(rand()) / RAND_MAX;
            colors[i][2] = static_cast<float>(rand()) / RAND_MAX;
        }
        break;
    case 's':
        timerRunning = false;
        timer_mode = 0;
        break;
    case 'm':
        for (int i = 0; i < currentBlcokcnt; i++)
        {
            square[i] = temp_square[i];
        }

        break;
    case 'r':
        ResetRect();
        timerRunning = false;
        timer_mode = 0;
        break;
    case 'q':
        exit(0);

        break;
    }
    glutPostRedisplay(); // 화면 갱신
}