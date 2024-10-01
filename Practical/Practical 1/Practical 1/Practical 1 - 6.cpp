#include <iostream>
#include <gl/glew.h> //--- �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <ctime>
#include <cstdlib> // rand() �Լ��� ����ϱ� ���� �߰�
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
void Timer(int value); // Ÿ�̸� �ݹ� �Լ�
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
float bgColor[3] = { 1.0f, 1.0f, 1.0f }; // ��� ���� 

float gap_x{};
float gap_y{};

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
    MakeSquare();
    glutDisplayFunc(drawScene); // ��� �Լ��� ����
    glutReshapeFunc(Reshape); // �ٽ� �׸��� �Լ� ����
    glutPassiveMotionFunc(passiveMotion);
    glutMotionFunc(Motion);
    glutMouseFunc(Mouse);
    glutKeyboardFunc(Keyboard);
    glutTimerFunc(50, Timer, 0);
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
        for (int i = MAX - 1; i >= 0; i--) {
            // �簢�� ���� Ŭ�� ���� Ȯ�� 
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
    glutPostRedisplay(); // ȭ�� ����
}
bool IsRectOverlap(Square rect1, Square rect2) {
    if (rect1.x2 <= rect2.x1 || rect1.x1 >= rect2.x2) {
        return false;
    }
    if (rect1.y2 <= rect2.y1 || rect1.y1 >= rect2.y2) {
        return false;
    }
    // �� �� ��ο��� ��ģ�ٸ� �浹�� �߻��� ��
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

    glutPostRedisplay(); // ȭ�� ����

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
                        // �¿�
                        switch (separeted_square[i][j].mode)
                        {
                        case 1: // ���� �̵�
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
                        case 0: // ���� �̵�
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
                        //����
                        switch (separeted_square[i][j].direction2)
                        {
                        case 2: // ���� �̵�
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
                        case 3: // �Ʒ��� �̵�
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
                        case 1: // ���� �̵�
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
                        case 0: // ���� �̵�
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

                        case 2: // ���� �̵�
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
                        case 3: // �Ʒ��� �̵�
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
                        case 1: // ���� �̵�
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
                        case 0: // ���� �̵�
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

                        case 2: // ���� �̵�
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
                        case 3: // �Ʒ��� �̵�
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
                    case 1: //-- �밢�� �
                        // �¿�
                        switch (separeted_square[i][j].mode)
                        {
                        case 1: // ���� �̵�
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
                        case 0: // ���� �̵�
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
                        //����
                        switch (separeted_square[i][j].direction2)
                        {
                        case 2: // ���� �̵�
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
                        case 3: // �Ʒ��� �̵�
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
    glutPostRedisplay(); // ȭ�� ����
    glutTimerFunc(100, Timer, value);
}