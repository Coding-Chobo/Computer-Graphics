#include <iostream>
#include <gl/glew.h> //--- �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <ctime>
#include <cstdlib> // rand() �Լ��� ����ϱ� ���� �߰�

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
void Timer(int value); // Ÿ�̸� �ݹ� �Լ�
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void drawRect();
void ResetRect();
void passiveMotion(int x, int y);
bool timerRunning = false; // Ÿ�̸� ���¸� �����ϴ� ����
int timer_mode{};
int currentBlcokcnt{};
SQUARE square[MAX] = {};
SQUARE temp_square[MAX] = {};
float colors[MAX][3] = { {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 1.0, 0.0} }; // �簢�� ����
float bgColor[3] = { 0.2f, 0.2f, 0.2f }; // ��� ���� 


int main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ���� 
{
    srand(time(NULL));
    //--- ������ �����ϱ�
    glutInit(&argc, argv); // glut �ʱ�ȭ
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // ���÷��� ��� ����
    glutInitWindowPosition(0, 0); // �������� ��ġ ����
    glutInitWindowSize(ROW, COL); // �������� ũ�� ����
    glutCreateWindow("Example1"); // ������ ����

    //--- GLEW �ʱ�ȭ�ϱ�
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { // glew �ʱ�ȭ
        std::cerr << "Unable to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }
    else {
        std::cout << "GLEW Initialized\n";
    }

    glutDisplayFunc(drawScene); // ��� �Լ��� ����
    glutReshapeFunc(Reshape); // �ٽ� �׸��� �Լ� ����
    glutPassiveMotionFunc(passiveMotion);
    glutMotionFunc(Motion);
    glutMouseFunc(Mouse);
    glutKeyboardFunc(Keyboard);
    glutMainLoop(); // �̺�Ʈ ó�� ����
    return 0;
}

void passiveMotion(int x, int y) {
    // ��ǥ ��ȯ
    float xpos = static_cast<float>(x) / (ROW / 2.0f) - 1.0f;
    float ypos = 1.0f - static_cast<float>(y) / (COL / 2.0f);
    std::cout << "x = " << x << " y = " << y << " -> xpos = " << xpos << " ypos = " << currentBlcokcnt << std::endl;
}

GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ� 
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
}

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ� 
{
    glClearColor(bgColor[0], bgColor[1], bgColor[2], 1.0f); // ������ ����
    glClear(GL_COLOR_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
    drawRect();
    glutSwapBuffers(); // ȭ�鿡 ����ϱ�
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
    if (timerRunning) { // Ÿ�̸Ӱ� ���� ���� ��쿡�� ����
        float moving_gap = { 0.05f };
        if (timer_mode == 1)
        {
            for (int i = 0; i < currentBlcokcnt; i++)
            {
                switch (square[i].direction)
                {
                case 0: // ���� �̵�
                    if (square[i].x + square[i].xgap + moving_gap < 1.0f)
                    {
                        square[i].x += moving_gap;
                    }
                    else
                    {
                        square[i].direction = 1;
                    }
                    break;
                case 1: // ���� �̵�
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
                case 0: // �Ʒ��� �̵�
                    if (square[i].y - square[i].ygap - moving_gap > -1.0f)
                    {
                        square[i].y -= moving_gap;
                    }
                    else
                    {
                        square[i].direction2 = 1;
                    }
                    break;
                case 1: // ���� �̵�
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
                case 0: // ���� �̵�
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
                case 1: // �Ʒ��� �̵�
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
                case 2: // ���� �̵�
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
                case 3: // ���� �̵�
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
        } //-- ������� ������
        glutPostRedisplay(); // ȭ�� ����
        glutTimerFunc(100, Timer, value);
    }
}
void Mouse(int button, int state, int x, int y)
{
    // OpenGL ��ǥ�� ��ȯ
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
    glutPostRedisplay(); // ȭ�� ����
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
    glutPostRedisplay(); // ȭ�� ����
}