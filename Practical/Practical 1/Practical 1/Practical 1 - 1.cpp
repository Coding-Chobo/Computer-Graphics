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

bool timerRunning = false; // Ÿ�̸� ���¸� �����ϴ� ����

void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ���� 
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
    glutKeyboardFunc(Keyboard);
    glutMainLoop(); // �̺�Ʈ ó�� ����
}

GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ� 
{
    glViewport(0, 0, w, h);
}

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ� 
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // �������� ������� ����
    glClear(GL_COLOR_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
    glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}

void Timer(int value) {
    if (timerRunning) { // Ÿ�̸Ӱ� ���� ���� ��쿡�� ����
        // ������ ���� ����
        float r = static_cast<float>(rand()) / RAND_MAX;
        float g = static_cast<float>(rand()) / RAND_MAX;
        float b = static_cast<float>(rand()) / RAND_MAX;
        glClearColor(r, g, b, 1.0f); // ���� �� ����
        glClear(GL_COLOR_BUFFER_BIT);
        glutSwapBuffers(); // ȭ�� ����

        // 500ms���� Ÿ�̸Ӹ� �ٽ� ȣ�� (0.5��)
        glutTimerFunc(500, Timer, 0);
    }
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 'c':
        glClearColor(0.0f, 1.0f, 1.0f, 1.0f); // û�ϻ� (cyan)
        glClear(GL_COLOR_BUFFER_BIT); // ȭ���� û�ϻ����� ĥ�ϱ�
        glutSwapBuffers(); // ���� ��ü (ȭ�� ����)
        break;
    case 'm':
        glClearColor(1.0f, 0.0f, 1.0f, 1.0f); // ��ȫ�� (magenta)
        glClear(GL_COLOR_BUFFER_BIT);
        glutSwapBuffers(); // ���� ��ü (ȭ�� ����)
        break;
    case 'y':
        glClearColor(1.0f, 1.0f, 0.0f, 1.0f); // ����� (yellow)
        glClear(GL_COLOR_BUFFER_BIT);
        glutSwapBuffers(); // ���� ��ü (ȭ�� ����)
        break;
    case 'a':
    {
        // ������ �� ����
        float r = static_cast<float>(rand()) / RAND_MAX;
        float g = static_cast<float>(rand()) / RAND_MAX;
        float b = static_cast<float>(rand()) / RAND_MAX;
        glClearColor(r, g, b, 1.0f); // ���� �� ����
        glClear(GL_COLOR_BUFFER_BIT);
        glutSwapBuffers(); // ���� ��ü (ȭ�� ����)
    }
    break;
    case 'w':
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // ���
        glClear(GL_COLOR_BUFFER_BIT);
        glutSwapBuffers(); // ���� ��ü (ȭ�� ����)
        break;
    case 'k':
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // ������
        glClear(GL_COLOR_BUFFER_BIT);
        glutSwapBuffers(); // ���� ��ü (ȭ�� ����)
        break;
    case 't':
        if (!timerRunning) {
            timerRunning = true; // Ÿ�̸� ����
            glutTimerFunc(500, Timer, 0); // 500ms���� Timer �Լ� ȣ��
        }
        break;
    case 's':
        timerRunning = false; // Ÿ�̸� ����
        break;
    case 'q':
        exit(0); // ���α׷� ����
        break;
    }
}
