#include <iostream>
#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <ctime>
#include <cstdlib> // rand() 함수를 사용하기 위해 추가

#define COL 600
#define ROW 800
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
void Timer(int value); // 타이머 콜백 함수
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void initRect();
void passiveMotion(int x, int y);
bool timerRunning = false; // 타이머 상태를 저장하는 변수

// 사각형 좌표와 초기 좌표를 함께 저장 (최소 크기 및 최대 크기)
float square[4][4] = { {0.0f, 0.0f, 1.0f, 1.0f},
    {-1.0f, 0.0f, 0.0f, 1.0f},
    {-1.0f, -1.0f, 0.0f, 0.0f},
    {0.0f, -1.0f, 1.0f, 0.0f} };
float initialSquare[4][4] = { {0.0f, -1.0f, 1.0f, 0.0f}, {-1.0f, -1.0f, 0.0f, 0.0f},{-1.0f, 0.0f, 0.0f, 1.0f},{0.0f, 0.0f, 1.0f, 1.0f} };

float colors[4][3] = { {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 1.0, 0.0} }; // 사각형 색상
float bgColor[3] = { 1.0f, 1.0f, 0.5f }; // 배경 색상 

// 사각형 그리기 함수
void initRect() {
    for (int i = 0; i < 4; ++i) {
        glColor3f(colors[i][0], colors[i][1], colors[i][2]);
        glRectf(square[i][0], square[i][1], square[i][2], square[i][3]);
    }
}

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

    glutDisplayFunc(drawScene); // 출력 함수의 지정
    glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
    glutPassiveMotionFunc(passiveMotion);
    glutMouseFunc(Mouse);

    glutMainLoop(); // 이벤트 처리 시작
    return 0;
}

void passiveMotion(int x, int y) {
    // 좌표 변환
    float xpos = static_cast<float>(x) / (ROW / 2.0f) - 1.0f;
    float ypos = 1.0f - static_cast<float>(y) / (COL / 2.0f);
    std::cout << "x = " << x << " y = " << y << " -> xpos = " << xpos << " ypos = " << ypos << std::endl;
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
    initRect();
    glutSwapBuffers(); // 화면에 출력하기
}

void Mouse(int button, int state, int x, int y)
{
    // OpenGL 좌표로 변환
    float xpos = (static_cast<float>(x) / (ROW / 2.0f)) - 1.0f;
    float ypos = 1.0f - (static_cast<float>(y) / (COL / 2.0f));
    if (state == GLUT_DOWN) {
        bool inRectangle = false;

        if (button == GLUT_LEFT_BUTTON) {
            for (int i = 0; i < 4; i++) {
                if (xpos >= square[i][0] && xpos <= square[i][2] && ypos >= square[i][1] && ypos <= square[i][3]) {
                    inRectangle = true;
                    colors[i][0] = static_cast<float>(rand()) / RAND_MAX;
                    colors[i][1] = static_cast<float>(rand()) / RAND_MAX;
                    colors[i][2] = static_cast<float>(rand()) / RAND_MAX;
                }
            }
            if (!inRectangle) {
                bgColor[0] = static_cast<float>(rand()) / RAND_MAX;
                bgColor[1] = static_cast<float>(rand()) / RAND_MAX;
                bgColor[2] = static_cast<float>(rand()) / RAND_MAX;
            }
        }
        else if (button == GLUT_RIGHT_BUTTON) {
            for (int i = 0; i < 4; i++) {
                if (xpos >= square[i][0] && xpos <= square[i][2] && ypos >= square[i][1] && ypos <= square[i][3]) {
                    inRectangle = true;
                    // 축소 조건: 사각형 크기가 너무 작아지지 않도록 함
                    if (square[i][2] - square[i][0] > 0.2f) {
                        square[i][0] += 0.1f;
                        square[i][1] += 0.1f;
                        square[i][2] -= 0.1f;
                        square[i][3] -= 0.1f;
                    }
                }
            }
            if (!inRectangle) {
                if (xpos < 0.0f&& ypos > 0.0f)
                {
                    square[1][0] -= 0.1f;
                    square[1][1] -= 0.1f;
                    square[1][2] += 0.1f;
                    square[1][3] += 0.1f;
                }
                else if (xpos > 0.0f && ypos > 0.0f)
                {
                    square[0][0] -= 0.1f;
                    square[0][1] -= 0.1f;
                    square[0][2] += 0.1f;
                    square[0][3] += 0.1f;
                }
                else if (xpos < 0.0f && ypos < 0.0f)
                {
                    square[2][0] -= 0.1f;
                    square[2][1] -= 0.1f;
                    square[2][2] += 0.1f;
                    square[2][3] += 0.1f;
                }
                else if (xpos > 0.0f && ypos < 0.0f)
                {
                    square[3][0] -= 0.1f;
                    square[3][1] -= 0.1f;
                    square[3][2] += 0.1f;
                    square[3][3] += 0.1f;
                }
            }
        }
        glutPostRedisplay(); // 화면 갱신
    }
}

