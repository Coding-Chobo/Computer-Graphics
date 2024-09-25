#include <iostream>
#include <gl/glew.h> //--- �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <ctime>
#include <cstdlib> // rand() �Լ��� ����ϱ� ���� �߰�

#define COL 600
#define ROW 800
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
void Timer(int value); // Ÿ�̸� �ݹ� �Լ�
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void initRect();
void passiveMotion(int x, int y);
bool timerRunning = false; // Ÿ�̸� ���¸� �����ϴ� ����

// �簢�� ��ǥ�� �ʱ� ��ǥ�� �Բ� ���� (�ּ� ũ�� �� �ִ� ũ��)
float square[4][4] = { {0.0f, 0.0f, 1.0f, 1.0f},
    {-1.0f, 0.0f, 0.0f, 1.0f},
    {-1.0f, -1.0f, 0.0f, 0.0f},
    {0.0f, -1.0f, 1.0f, 0.0f} };
float initialSquare[4][4] = { {0.0f, -1.0f, 1.0f, 0.0f}, {-1.0f, -1.0f, 0.0f, 0.0f},{-1.0f, 0.0f, 0.0f, 1.0f},{0.0f, 0.0f, 1.0f, 1.0f} };

float colors[4][3] = { {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 1.0, 0.0} }; // �簢�� ����
float bgColor[3] = { 1.0f, 1.0f, 0.5f }; // ��� ���� 

// �簢�� �׸��� �Լ�
void initRect() {
    for (int i = 0; i < 4; ++i) {
        glColor3f(colors[i][0], colors[i][1], colors[i][2]);
        glRectf(square[i][0], square[i][1], square[i][2], square[i][3]);
    }
}

int main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ���� 
{
    srand(time(NULL));
    //--- ������ �����ϱ�
    glutInit(&argc, argv); // glut �ʱ�ȭ
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // ���÷��� ��� ����
    glutInitWindowPosition(100, 100); // �������� ��ġ ����
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
    glutMouseFunc(Mouse);

    glutMainLoop(); // �̺�Ʈ ó�� ����
    return 0;
}

void passiveMotion(int x, int y) {
    // ��ǥ ��ȯ
    float xpos = static_cast<float>(x) / (ROW / 2.0f) - 1.0f;
    float ypos = 1.0f - static_cast<float>(y) / (COL / 2.0f);
    std::cout << "x = " << x << " y = " << y << " -> xpos = " << xpos << " ypos = " << ypos << std::endl;
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
    initRect();
    glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}

void Mouse(int button, int state, int x, int y)
{
    // OpenGL ��ǥ�� ��ȯ
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
                    // ��� ����: �簢�� ũ�Ⱑ �ʹ� �۾����� �ʵ��� ��
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
        glutPostRedisplay(); // ȭ�� ����
    }
}

