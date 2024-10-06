#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <gl/glew.h> // �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <ctime>
#include <cstdlib> // rand() �Լ��� ����ϱ� ���� �߰�
#include <stdio.h>
#include <iomanip> // For debug output precision

#define Max 12
#define Max_vertex 40
#define ROW 1000
#define COL 1000

GLchar* vertexSource, * fragmentSource; // �ҽ��ڵ� ���� ����
GLuint vertexShader, fragmentShader; // ���̴� ��ü
GLuint shaderProgramID; // ���̴� ���α׷�
GLuint vao, vbo[2]; // VAO�� VBO

//----------------------------- ����ü ���� -------------------------------

//--------------------------- ���� ���� ---------------------------------
float xpos{}, ypos{}; // ���콺 Ŭ�� ��ġ ��ǥ
bool canmake{}; // ������ ������ �� �ִ��� ����
int currentblockcnt{}; // ���� ������ ���� ����
float radius{ 0.15f }; // ������ ������
GLfloat colors[Max][3][3]; // �� ������ ����
GLfloat vertex[Max_vertex][3];
int limit_figure[4];
int currentvertexcnt{};
//----------------------------- �Լ� ���� -------------------------------------
void init_color();
void make_vertexShaders();
void make_fragmentShaders();
void Make_triangle();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLint width, height;
void InitBuffer();
bool mode_line{};
char* filetobuf(const char* file);
GLvoid Keyboard(unsigned char key, int x, int y);
void Timer(int value); // Ÿ�̸� �ݹ� �Լ�
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void init_figure();
void MakeSquare(int point);
void UpdateVBO();
//---------------------------------------------------------------------

//�ﰢ�� ����
void Make_triangle() {
    radius = static_cast<float>(rand()) / RAND_MAX / 4 + 0.2f;
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
}

// ���� �ʱ�ȭ �Լ�
void init_color() {
    // �� ������ ���� ���� �ʱ�ȭ
    for (int i = 0; i < Max; ++i) {
        for (int j = 0; j < 3; ++j) {
            colors[i][j][0] = static_cast<float>(rand()) / RAND_MAX;
            colors[i][j][1] = static_cast<float>(rand()) / RAND_MAX;
            colors[i][j][2] = static_cast<float>(rand()) / RAND_MAX;
        }
    }
}
// ���� �ʱⰪ �Է�
void init_figure() {
    xpos = -0.5f;
    ypos = 0.5f;
    Make_triangle();
    xpos = 0.5f;
    ypos = 0.5f;
    Make_triangle();
    xpos = -0.5f;
    ypos = -0.5f;
    Make_triangle();
    xpos = 0.5f;
    ypos = -0.5f;
    Make_triangle();
    limit_figure[0]++;
    limit_figure[1]++;
    limit_figure[2]++;
    limit_figure[3]++;
    // VBO �����͸� ������Ʈ�Ͽ� GPU�� �ٽ� ����
    UpdateVBO();

    // ȭ���� �ٽ� �׸����� ��û
    glutPostRedisplay();
}

// ���α׷��� ���� �Լ�
void main(int argc, char** argv) {
    // ������ ����
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(ROW, COL);
    glutCreateWindow("Practical 1");

    srand(time(NULL));

    // GLEW �ʱ�ȭ
    glewExperimental = GL_TRUE;
    glewInit();
    init_color(); // ���� �ʱ�ȭ
    InitBuffer(); // ���� �ʱ�ȭ
    make_vertexShaders(); // ���ؽ� ���̴� ����
    make_fragmentShaders(); // �����׸�Ʈ ���̴� ����
    shaderProgramID = make_shaderProgram(); // ���̴� ���α׷� ����
    init_figure();
    // �ݹ� �Լ� ���
    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutMouseFunc(Mouse);
    glutKeyboardFunc(Keyboard);

    glutMainLoop(); // ���� ���� ����
}

// ������ �����ϰ� ���� ������ �����ϴ� �Լ�
void MakeSquare(int point) {
    if (currentblockcnt >= Max) return; // �ִ� ���� ������ �ʰ����� �ʵ��� ��


    switch (point) {



        break;

    default:
        break;
    }
    // VBO �����͸� ������Ʈ�Ͽ� GPU�� �ٽ� ����
    UpdateVBO();

    // ȭ���� �ٽ� �׸����� ��û
    glutPostRedisplay();
}

// VAO �� VBO �ʱ�ȭ
void InitBuffer() {
    glGenVertexArrays(1, &vao); // VAO ����
    glBindVertexArray(vao); // VAO ���ε�

    glGenBuffers(2, vbo); // 2���� VBO ����

    // ���� ��ǥ �����͸� VBO�� ����
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // ���� �����͸� VBO�� ����
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
}

// VBO �����͸� ������Ʈ�ϴ� �Լ�
void UpdateVBO() {
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex), vertex); // GPU�� ���ο� ���� ������ ����
}

// ȭ�� �׸��� �ݹ� �Լ�
GLvoid drawScene() {
    // ���� ����
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramID); // ���̴� ���α׷� ���

    glBindVertexArray(vao); // VAO ���ε�

    // ���� �׸���: �� ������ ���� ������ ���� �ٸ��� �׷��� ��
    int offset = 0;
    if (!mode_line)
    {
        for (int i = 0; i < currentvertexcnt; i += 3) {
            std::cout << "������ �׸��� �ֽ��ϴ�." << std::endl;
            glDrawArrays(GL_TRIANGLES, i, 3); // �ﰢ��: 3���� ����
        }
    }
    else
    {
        for (int i = 0; i < currentvertexcnt; i += 3) {
            glDrawArrays(GL_LINE_LOOP, i, 3);
        }
    }

    glutSwapBuffers(); // ȭ�鿡 ���
}

// ������ �������� �ݹ� �Լ�
GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

// ���콺 Ŭ�� �̺�Ʈ �ݹ� �Լ�
void Mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        xpos = (static_cast<float>(x) / (ROW / 2.0f)) - 1.0f; // OpenGL ��ǥ�� ��ȯ
        ypos = 1.0f - (static_cast<float>(y) / (COL / 2.0f)); // OpenGL ��ǥ�� ��ȯ
        if (xpos < 0 && ypos > 0)
        {
            vertex[0][0] = xpos;
            vertex[0][1] = ypos + radius;
            vertex[0][2] = 0.0f;

            vertex[1][0] = xpos - radius * 0.85;
            vertex[1][1] = ypos - radius / 2;
            vertex[1][2] = 0.0f;

            vertex[2][0] = xpos + radius * 0.85;
            vertex[2][1] = ypos - radius / 2;
            vertex[2][2] = 0.0f;
            for (int j = 0; j < 3; j++)
            {
                colors[0][j][0] = static_cast<float>(rand()) / RAND_MAX;
                colors[0][j][1] = static_cast<float>(rand()) / RAND_MAX;
                colors[0][j][2] = static_cast<float>(rand()) / RAND_MAX;
            }
        }
        else if (xpos > 0 && ypos > 0) {
            vertex[3][0] = xpos;
            vertex[3][1] = ypos + radius;
            vertex[3][2] = 0.0f;

            vertex[4][0] = xpos - radius * 0.85;
            vertex[4][1] = ypos - radius / 2;
            vertex[4][2] = 0.0f;

            vertex[5][0] = xpos + radius * 0.85;
            vertex[5][1] = ypos - radius / 2;
            vertex[5][2] = 0.0f;
        }
        else if (xpos < 0 && ypos < 0) {
            vertex[6][0] = xpos;
            vertex[6][1] = ypos + radius;
            vertex[6][2] = 0.0f;

            vertex[7][0] = xpos - radius * 0.85;
            vertex[7][1] = ypos - radius / 2;
            vertex[7][2] = 0.0f;

            vertex[8][0] = xpos + radius * 0.85;
            vertex[8][1] = ypos - radius / 2;
            vertex[8][2] = 0.0f;
        }
        else if (xpos > 0 && ypos < 0) {
            vertex[9][0] = xpos;
            vertex[9][1] = ypos + radius;
            vertex[9][2] = 0.0f;

            vertex[10][0] = xpos - radius * 0.85;
            vertex[10][1] = ypos - radius / 2;
            vertex[10][2] = 0.0f;

            vertex[11][0] = xpos + radius * 0.85;
            vertex[11][1] = ypos - radius / 2;
            vertex[11][2] = 0.0f;
        }
    }
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        xpos = (static_cast<float>(x) / (ROW / 2.0f)) - 1.0f; // OpenGL ��ǥ�� ��ȯ
        ypos = 1.0f - (static_cast<float>(y) / (COL / 2.0f)); // OpenGL ��ǥ�� ��ȯ
        if (xpos < 0 && ypos > 0 && limit_figure[0] < 3)
        {
            Make_triangle();
            limit_figure[0]++;
        }
        else if (xpos > 0 && ypos > 0 && limit_figure[1] < 3)
        {
            Make_triangle();
            limit_figure[1]++;
        }
        else if (xpos < 0 && ypos < 0 && limit_figure[2] < 3)
        {
            Make_triangle();
            limit_figure[2]++;
        }
        else if (xpos > 0 && ypos < 0 && limit_figure[3] < 3)
        {
            Make_triangle();
            limit_figure[3]++;
        }





    }

    // VBO �����͸� ������Ʈ�Ͽ� GPU�� �ٽ� ����
    UpdateVBO();

    // ȭ���� �ٽ� �׸����� ��û
    glutPostRedisplay();
}

// Ű���� �Է� �̺�Ʈ �ݹ� �Լ�
GLvoid Keyboard(unsigned char key, int x, int y) {

    switch (key) {
    case 'a':
        mode_line = false;
        break;
    case 'b':
        mode_line = true;
        break;
    }


    // VBO �����͸� ������Ʈ�Ͽ� GPU�� �ٽ� ����
    UpdateVBO();

    // ȭ���� �ٽ� �׸����� ��û
    glutPostRedisplay();
}

// ���̴� ���α׷� ���� �Լ�
GLuint make_shaderProgram() {
    GLuint shaderID = glCreateProgram();
    glAttachShader(shaderID, vertexShader);
    glAttachShader(shaderID, fragmentShader);
    glLinkProgram(shaderID);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderID;
}

// ���ؽ� ���̴� ���� �Լ�
void make_vertexShaders() {
    vertexSource = filetobuf("vertex.vs");
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
    glCompileShader(vertexShader);
}

// �����׸�Ʈ ���̴� ���� �Լ�
void make_fragmentShaders() {
    fragmentSource = filetobuf("fragment.fs");
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
    glCompileShader(fragmentShader);
}

// ������ ���۷� �о���� �Լ�
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
