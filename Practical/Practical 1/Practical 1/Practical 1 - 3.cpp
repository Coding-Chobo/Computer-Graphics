#include <iostream>
#include <gl/glew.h> //--- �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <ctime>
#include <cstdlib> // rand() �Լ��� ����ϱ� ���� �߰�
#include <stdio.h>
#define COL 600
#define ROW 800
#define MAX 10
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
void Timer(int value); // Ÿ�̸� �ݹ� �Լ�
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void drawRect();
void passiveMotion(int x, int y);
bool timerRunning = false; // Ÿ�̸� ���¸� �����ϴ� ����
int currentBlcokcnt{};
int selectedBlock{MAX};

float previousMouseX, previousMouseY;

float square[MAX][4] = {};
float colors[MAX][3] = { {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 1.0, 0.0} }; // �簢�� ����
float bgColor[3] = { 1.0f, 1.0f, 0.5f }; // ��� ���� 

void drawRect() {
    for (int i = 0; i < currentBlcokcnt; ++i) {
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
    std::cout << "x = " << x << " y = " << y << " -> xpos = " << xpos << " ypos = " << ypos << std::endl;
    std::cout << currentBlcokcnt << std::endl;
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

void Mouse(int button, int state, int x, int y)
{
    // OpenGL ��ǥ�� ��ȯ (ȭ���� ũ�⸦ ���)
    float xpos = (static_cast<float>(x) / (ROW / 2.0f)) - 1.0f;
    float ypos = 1.0f - (static_cast<float>(y) / (COL / 2.0f));
    
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        for (int i = currentBlcokcnt - 1; i >= 0; i--) {
            // �簢�� ���� Ŭ�� ���� Ȯ�� 
            if ((xpos > square[i][0]) && (xpos < square[i][2]) && (ypos > square[i][1]) && (ypos < square[i][3])) {
                selectedBlock = i;
                previousMouseX = xpos;
                previousMouseY = ypos;
                break;
            }
        }
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        bool inRectangle = false;
        for (int i = currentBlcokcnt - 1; i >= 0; i--) {
            // �簢�� ���� Ŭ�� ���� Ȯ�� 
            if ((xpos > square[i][0]) && (xpos < square[i][2]) && (ypos > square[i][1]) && (ypos < square[i][3])&&(selectedBlock != i)) {
                square[i][0] = (square[i][0] < square[selectedBlock][0]) ? square[i][0] : square[selectedBlock][0];
                square[i][2] = (square[i][2] > square[selectedBlock][2]) ? square[i][2] : square[selectedBlock][2];
                square[i][1] = (square[i][1] < square[selectedBlock][1]) ? square[i][1] : square[selectedBlock][1];
                square[i][3] = (square[i][3] > square[selectedBlock][3]) ? square[i][3] : square[selectedBlock][3];
                colors[i][0] = static_cast<float>(rand()) / RAND_MAX;
                colors[i][1] = static_cast<float>(rand()) / RAND_MAX;
                colors[i][2] = static_cast<float>(rand()) / RAND_MAX;
                inRectangle = true;
                break;
            }
        }
        if (inRectangle)
        {
            if (currentBlcokcnt == selectedBlock)
            {
                square[currentBlcokcnt][0] = {};
                square[currentBlcokcnt][1] = {};
                square[currentBlcokcnt][2] = {};
                square[currentBlcokcnt][3] = {};
                currentBlcokcnt--;
            }
            else
            {
                for (int i = selectedBlock; i < currentBlcokcnt - 1; i++)
                {
                    square[i][0] = square[i + 1][0];
                    square[i][1] = square[i + 1][1];
                    square[i][2] = square[i + 1][2];
                    square[i][3] = square[i + 1][3];
                }
                currentBlcokcnt--;
            }
        }
        
        
        selectedBlock = MAX;
    }
    glutPostRedisplay(); // ȭ�� ����
}

void Motion(int x, int y) {
    std::cout << selectedBlock << std::endl;
    if (selectedBlock < MAX) { // ���õ� ����� ���� ���� ����
        // OpenGL ��ǥ�� ��ȯ
        float xpos = static_cast<float>(x) / (ROW / 2.0f) - 1.0f;
        float ypos = 1.0f - static_cast<float>(y) / (COL / 2.0f);

        // ���콺 �̵��� ���
        float deltaX = xpos - previousMouseX;
        float deltaY = ypos - previousMouseY;

        // ���õ� ��� �̵�
        square[selectedBlock][0] += deltaX;
        square[selectedBlock][1] += deltaY;
        square[selectedBlock][2] += deltaX;
        square[selectedBlock][3] += deltaY;

        // ���콺 ��ġ ������Ʈ
        previousMouseX = xpos;
        previousMouseY = ypos;

        glutPostRedisplay(); // ȭ�� ����
    }
}
GLvoid Keyboard(unsigned char key, int x, int y)
{
    switch (key) {

    case 'a':
    {
        if (currentBlcokcnt < MAX)
        {
            for (int i = 0; i < 4; i++)
            {
                int decide_sign = rand() % 2;
                if (decide_sign == 0)
                {
                    square[currentBlcokcnt][i] = static_cast<float>(rand()) / RAND_MAX;
                }
                else
                {   
                    square[currentBlcokcnt][i] = (static_cast<float>(rand()) / RAND_MAX) * (-1);
                }
            }
            if (square[currentBlcokcnt][0] > square[currentBlcokcnt][2])
            {
                float temp = square[currentBlcokcnt][0];
                square[currentBlcokcnt][0] = square[currentBlcokcnt][2];
                square[currentBlcokcnt][2] = temp;
            }
            if (square[currentBlcokcnt][1] > square[currentBlcokcnt][3])
            {
                float temp = square[currentBlcokcnt][1];
                square[currentBlcokcnt][1] = square[currentBlcokcnt][3];
                square[currentBlcokcnt][3] = temp;
            }
            colors[currentBlcokcnt][0] = static_cast<float>(rand()) / RAND_MAX;
            colors[currentBlcokcnt][1] = static_cast<float>(rand()) / RAND_MAX;
            colors[currentBlcokcnt][2] = static_cast<float>(rand()) / RAND_MAX;
            currentBlcokcnt++;
        }
        glutSwapBuffers(); // ���� ��ü (ȭ�� ����)
        glutPostRedisplay(); // ȭ�� ����
    }
    break;
    }
}
