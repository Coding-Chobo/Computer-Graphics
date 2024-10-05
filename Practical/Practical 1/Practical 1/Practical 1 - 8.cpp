#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <gl/glew.h> // �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <ctime>
#include <cstdlib> // rand() �Լ��� ����ϱ� ���� �߰�
#include <stdio.h>
#include <iomanip> // For debug output precision

#define Max 10
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
int shape[Max];
int currentvertexcnt{};
//----------------------------- �Լ� ���� -------------------------------------
void init_color();
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLint width, height;
void InitBuffer();
char* filetobuf(const char* file);
GLvoid Keyboard(unsigned char key, int x, int y);
void Timer(int value); // Ÿ�̸� �ݹ� �Լ�
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void MakeSquare(int point);
void UpdateVBO();
void PrintFigureDebugInfo();
//---------------------------------------------------------------------

// ���� �ʱ�ȭ �Լ�
void init_color() {
    GLfloat presetColors[3][3] = {
        {1.0f, 0.0f, 0.0f},  // Red
        {0.0f, 1.0f, 0.0f},  // Green
        {0.0f, 0.0f, 1.0f}   // Blue
    };

    // �� ������ ���� ���� �ʱ�ȭ
    for (int i = 0; i < Max; ++i) {
        for (int j = 0; j < 3; ++j) {
            colors[i][j][0] = presetColors[i % 3][0]; // R value
            colors[i][j][1] = presetColors[i % 3][1]; // G value
            colors[i][j][2] = presetColors[i % 3][2]; // B value
        }
    }
}

// ���α׷��� ���� �Լ�
void main(int argc, char** argv) {
    // ������ ����
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(ROW, COL);
    glutCreateWindow("Practical 1");

    // GLEW �ʱ�ȭ
    glewExperimental = GL_TRUE;
    glewInit();

    init_color(); // ���� �ʱ�ȭ
    InitBuffer(); // ���� �ʱ�ȭ
    make_vertexShaders(); // ���ؽ� ���̴� ����
    make_fragmentShaders(); // �����׸�Ʈ ���̴� ����
    shaderProgramID = make_shaderProgram(); // ���̴� ���α׷� ����

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

    shape[currentblockcnt] = point;
    switch (point) {
    case 1: // ���� �簢��(��)
        vertex[currentvertexcnt][0] = xpos - radius / 3;
        vertex[currentvertexcnt][1] = ypos - radius / 3;
        vertex[currentvertexcnt][2] = 0.0f;
        currentvertexcnt++;
        vertex[currentvertexcnt][0] = xpos - radius / 3;
        vertex[currentvertexcnt][1] = ypos + radius / 3;
        vertex[currentvertexcnt][2] = 0.0f;
        currentvertexcnt++;
        vertex[currentvertexcnt][0] = xpos + radius / 3;
        vertex[currentvertexcnt][1] = ypos + radius / 3;
        vertex[currentvertexcnt][2] = 0.0f;
        currentvertexcnt++;
        vertex[currentvertexcnt][0] = xpos + radius / 3;
        vertex[currentvertexcnt][1] = ypos - radius / 3;
        vertex[currentvertexcnt][2] = 0.0f;
        currentvertexcnt++;
        break;

    case 2: // ��
        vertex[currentvertexcnt][0] = xpos - radius;
        vertex[currentvertexcnt][1] = ypos - radius;
        vertex[currentvertexcnt][2] = 0.0f;
        currentvertexcnt++;
        vertex[currentvertexcnt][0] = xpos + radius;
        vertex[currentvertexcnt][1] = ypos + radius;
        vertex[currentvertexcnt][2] = 0.0f;
        currentvertexcnt++;
        break;

    case 3: // �ﰢ��
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
        break;

    case 4: // �簢��
        vertex[currentvertexcnt][0] = xpos - radius;
        vertex[currentvertexcnt][1] = ypos - radius;
        vertex[currentvertexcnt][2] = 0.0f;
        currentvertexcnt++;
        vertex[currentvertexcnt][0] = xpos - radius;
        vertex[currentvertexcnt][1] = ypos + radius;
        vertex[currentvertexcnt][2] = 0.0f;
        currentvertexcnt++;
        vertex[currentvertexcnt][0] = xpos + radius;
        vertex[currentvertexcnt][1] = ypos + radius;
        vertex[currentvertexcnt][2] = 0.0f;
        currentvertexcnt++;
        vertex[currentvertexcnt][0] = xpos + radius;
        vertex[currentvertexcnt][1] = ypos - radius;
        vertex[currentvertexcnt][2] = 0.0f;
        currentvertexcnt++;
        break;

    default:
        break;
    }

    currentblockcnt++; // ���� ���� ����

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
    for (int i = 0; i < currentblockcnt; ++i) {
        if (shape[i] == 4) {
            glDrawArrays(GL_TRIANGLE_FAN, offset, 4); // �簢��: 4���� ����
            offset += 4;
        }
        else if (shape[i] == 2) {
            glDrawArrays(GL_LINES, offset, 2); // ��: 2���� ����
            offset += 2;
        }
        else if (shape[i] == 3) {
            glDrawArrays(GL_TRIANGLES, offset, 3); // �ﰢ��: 3���� ����
            offset += 3;
        }
        else if (shape[i] == 1) {
            glDrawArrays(GL_TRIANGLE_FAN, offset, 4); // ���� �簢��: 4���� ����
            offset += 4;
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
        canmake = true;
        xpos = (static_cast<float>(x) / (ROW / 2.0f)) - 1.0f; // OpenGL ��ǥ�� ��ȯ
        ypos = 1.0f - (static_cast<float>(y) / (COL / 2.0f)); // OpenGL ��ǥ�� ��ȯ
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        canmake = false;
    }
}

// Ű���� �Է� �̺�Ʈ �ݹ� �Լ�
GLvoid Keyboard(unsigned char key, int x, int y) {
    if (canmake) {
        switch (key) {
        case 'p': MakeSquare(1); break; // ���� �簢��(��) ����
        case 'l': MakeSquare(2); break; // �� ����
        case 't': MakeSquare(3); break; // �ﰢ�� ����
        case 'r': MakeSquare(4); break; // �簢�� ����
        }
    }
    int edited_vertex{};
    int select_figure{};
    if (currentblockcnt > 0)
    {
        switch (key)
        {
        case 'w':
            edited_vertex = 0;
            select_figure = rand() % currentblockcnt;  // �ٲ�� ���� �ε���
            for (int i = 0; i < select_figure; i++)
            {
                if (shape[i] == 1)
                {
                    edited_vertex += shape[i] * 4;
                }
                else
                {
                    edited_vertex += shape[i];
                }

            }
            if (shape[select_figure] == 1)
            {
                for (int i = 0; i < 4; i++) // �ٲ�� ������ ���ؽ� ��
                {
                    vertex[edited_vertex + i][1] += radius;
                }
            }
            else
            {
                for (int i = 0; i < shape[select_figure]; i++) // �ٲ�� ������ ���ؽ� ��
                {
                    vertex[edited_vertex + i][1] += radius;
                }
            }
            break;
        case 'a':
            edited_vertex = 0;
            select_figure = rand() % currentblockcnt;  // �ٲ�� ���� �ε���
            for (int i = 0; i < select_figure; i++)
            {
                if (shape[i] == 1)
                {
                    edited_vertex += shape[i] * 4;
                }
                else
                {
                    edited_vertex += shape[i];
                }
            }
            if (shape[select_figure] == 1)
            {
                for (int i = 0; i < 4; i++) // �ٲ�� ������ ���ؽ� ��
                {
                    vertex[edited_vertex + i][0] -= radius;
                }
            }
            else
            {
                for (int i = 0; i < shape[select_figure]; i++) // �ٲ�� ������ ���ؽ� ��
                {
                    vertex[edited_vertex + i][0] -= radius;
                }
            }
            break;
        case 's':
            edited_vertex = 0;
            select_figure = rand() % currentblockcnt;  // �ٲ�� ���� �ε���
            for (int i = 0; i < select_figure; i++)
            {
                if (shape[i] == 1)
                {
                    edited_vertex += shape[i] * 4;
                }
                else
                {
                    edited_vertex += shape[i];
                }
            }
            if (shape[select_figure] == 1)
            {
                for (int i = 0; i < 4; i++) // �ٲ�� ������ ���ؽ� ��
                {
                    vertex[edited_vertex + i][1] -= radius;
                }
            }
            else
            {
                for (int i = 0; i < shape[select_figure]; i++) // �ٲ�� ������ ���ؽ� ��
                {
                    vertex[edited_vertex + i][1] -= radius;
                }
            }
            break;
        case 'd':
            edited_vertex = 0;
            select_figure = rand() % currentblockcnt;  // �ٲ�� ���� �ε���
            for (int i = 0; i < select_figure; i++)
            {
                if (shape[i] == 1)
                {
                    edited_vertex += shape[i] * 4;
                }
                else
                {
                    edited_vertex += shape[i];
                }
            }
            if (shape[select_figure] == 1)
            {
                for (int i = 0; i < 4; i++) // �ٲ�� ������ ���ؽ� ��
                {
                    vertex[edited_vertex + i][0] += radius;
                }
            }
            else
            {
                for (int i = 0; i < shape[select_figure]; i++) // �ٲ�� ������ ���ؽ� ��
                {
                    vertex[edited_vertex + i][0] += radius;
                }
            }

            break;
        case 'c':
            currentblockcnt = 0;
            currentvertexcnt = 0;
            ZeroMemory(vertex, 100);

        default:
            break;

        }
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
