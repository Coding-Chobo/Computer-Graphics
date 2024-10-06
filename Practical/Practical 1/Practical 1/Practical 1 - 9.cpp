#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <gl/glew.h> // �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <ctime>
#include <cstdlib> // rand() �Լ��� ����ϱ� ���� �߰�
#include <stdio.h>
#include <iomanip> // For debug output precision
#include <math.h>

#define Max 4
#define Max_vertex 12
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
GLfloat prev[Max_vertex][3]{};
int currentvertexcnt{};
int direction1[4]{};
int direction2[4]{};
float Sp_radius{};
GLfloat rect_sprialgap[4]{ 0.0f };
int theta{};
bool mode[4]{};
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

//Ÿ�̸� �Լ�
void Timer(int value) {
    float moving_gap = 0.05f;
    float spiral_gap = 0.01f;

    switch (value)
    {
    case 1: // �ñ��
        if (mode[value - 1])
        {
            for (int i = 0; i < Max; i++)
            {
                switch (direction1[i])
                {
                case 1: // �·� �̵�
                    if (vertex[3 * i + 1][0] > -0.95f)
                    {
                        vertex[3 * i][0] -= moving_gap;
                        vertex[3 * i + 1][0] -= moving_gap;
                        vertex[3 * i + 2][0] -= moving_gap;
                    }
                    else
                    {
                        direction1[i] = 2;
                    }
                    break;
                case 2: // ��� �̵�
                    if (vertex[3 * i + 2][0] < 0.95f)
                    {
                        vertex[3 * i][0] += moving_gap;
                        vertex[3 * i + 1][0] += moving_gap;
                        vertex[3 * i + 2][0] += moving_gap;
                    }
                    else
                    {
                        direction1[i] = 1;
                    }
                    break;
                default:
                    break;
                }
            }
            for (int i = 0; i < Max; i++)
            {
                switch (direction2[i])
                {
                case 1:
                    if (vertex[3 * i][1] < 0.95f)
                    {
                        vertex[3 * i][1] += moving_gap;
                        vertex[3 * i + 1][1] += moving_gap;
                        vertex[3 * i + 2][1] += moving_gap;
                    }
                    else
                    {
                        direction2[i] = 2;
                    }
                    break;
                case 2:
                    if (vertex[3 * i + 1][1] > -0.95f)
                    {
                        vertex[3 * i][1] -= moving_gap;
                        vertex[3 * i + 1][1] -= moving_gap;
                        vertex[3 * i + 2][1] -= moving_gap;
                    }
                    else
                    {
                        direction2[i] = 1;
                    }
                    break;
                default:
                    break;
                }
            }
        }
        break;
    case 2: // �������
        if (mode[value - 1])
        {
            for (int i = 0; i < 4; i++)
            {
                switch (direction1[i])
                {
                case 1: // �·� �̵�
                    if (vertex[3 * i + 1][0] > -0.95f)
                    {
                        vertex[3 * i][0] -= moving_gap;
                        vertex[3 * i + 1][0] -= moving_gap;
                        vertex[3 * i + 2][0] -= moving_gap;
                    }
                    else if (vertex[3 * i + 1][1] > -0.95f)
                    {
                        vertex[3 * i][1] -= moving_gap;
                        vertex[3 * i + 1][1] -= moving_gap;
                        vertex[3 * i + 2][1] -= moving_gap;
                        direction1[i] = 2;
                    }
                    else
                    {
                        direction1[i] = 3;
                    }
                    break;
                case 2: // ��� �̵�
                    if (vertex[3 * i + 2][0] < 0.95f)
                    {
                        vertex[3 * i][0] += moving_gap;
                        vertex[3 * i + 1][0] += moving_gap;
                        vertex[3 * i + 2][0] += moving_gap;
                    }
                    else if (vertex[3 * i + 1][1] > -0.95f)
                    {
                        vertex[3 * i][1] -= moving_gap;
                        vertex[3 * i + 1][1] -= moving_gap;
                        vertex[3 * i + 2][1] -= moving_gap;
                        direction1[i] = 1;
                    }
                    else
                    {
                        direction1[i] = 3;
                    }
                    break;
                case 3: // ���� �̵�
                    if (vertex[3 * i][1] < 0.95f)
                    {
                        vertex[3 * i][1] += moving_gap;
                        vertex[3 * i + 1][1] += moving_gap;
                        vertex[3 * i + 2][1] += moving_gap;
                    }
                    else if (vertex[3 * i + 2][0] < 0.95f)
                    {
                        vertex[3 * i][0] += moving_gap;
                        vertex[3 * i + 1][0] += moving_gap;
                        vertex[3 * i + 2][0] += moving_gap;
                        direction1[i] = 4;
                    }
                    else
                    {
                        direction1[i] = 1;
                    }
                    break;
                case 4: //�Ʒ��� �̵�
                    if (vertex[3 * i + 1][1] > -0.95f)
                    {
                        vertex[3 * i][1] -= moving_gap;
                        vertex[3 * i + 1][1] -= moving_gap;
                        vertex[3 * i + 2][1] -= moving_gap;
                    }
                    else if (vertex[3 * i + 2][0] < 0.95f)
                    {
                        vertex[3 * i][0] += moving_gap;
                        vertex[3 * i + 1][0] += moving_gap;
                        vertex[3 * i + 2][0] += moving_gap;
                        direction1[i] = 3;
                    }
                    else
                    {
                        direction1[i] = 1;
                    }
                    break;
                default:
                    break;
                }
            }
        }
        break;
    case 3: // �׸� �����̷�
        if (mode[value - 1])
        {
            for (int i = 0; i < Max; i++)
            {
                switch (direction1[i])
                {
                case 1: // �·� �̵�
                    if (vertex[3 * i + 1][0] > -0.95f + rect_sprialgap[i])
                    {
                        vertex[3 * i][0] -= moving_gap;
                        vertex[3 * i + 1][0] -= moving_gap;
                        vertex[3 * i + 2][0] -= moving_gap;
                    }
                    else
                    {
                        direction1[i] = 4;
                    }
                    break;
                case 2: // ��� �̵�
                    if (vertex[3 * i + 2][0] < 0.95f - rect_sprialgap[i])
                    {
                        vertex[3 * i][0] += moving_gap;
                        vertex[3 * i + 1][0] += moving_gap;
                        vertex[3 * i + 2][0] += moving_gap;
                    }
                    else
                    {
                        direction1[i] = 3;
                    }
                    break;
                case 3: // ���� �̵�
                    if (vertex[3 * i][1] < 0.95f - rect_sprialgap[i])
                    {
                        vertex[3 * i][1] += moving_gap;
                        vertex[3 * i + 1][1] += moving_gap;
                        vertex[3 * i + 2][1] += moving_gap;
                    }
                    else
                    {
                        if (rect_sprialgap[i] < 0.9f)
                        {
                            rect_sprialgap[i] += 0.1f;
                        }
                        else
                        {
                            rect_sprialgap[i] = 0.0f;
                        }

                        direction1[i] = 1;
                    }
                    break;
                case 4: // �Ʒ��� �̵�
                    if (vertex[3 * i + 1][1] > -0.95f + rect_sprialgap[i])
                    {
                        vertex[3 * i][1] -= moving_gap;
                        vertex[3 * i + 1][1] -= moving_gap;
                        vertex[3 * i + 2][1] -= moving_gap;
                    }
                    else
                    {
                        direction1[i] = 2;
                    }
                    break;
                default:
                    break;
                }
            }

        }
        break;
    case 4: // �� �����̷�
        if (mode[value-1])
        {
            Sp_radius += spiral_gap;
            if (Sp_radius >= 0.5f)
            {
                Sp_radius = 0;
                theta = 0;
                for (int i = 0; i < Max_vertex; i++)
                {
                    for (int j = 0; j < 3; j++)
                    {
                        vertex[i][j] = prev[i][j];
                    }
                }
            }
            theta++;
            for (int i = 0; i < currentvertexcnt; i++)
            {
                vertex[i][0] += Sp_radius * cos(theta);
                vertex[i][1] += Sp_radius * sin(theta);
            }
        }
        break;
    default:
        break;
    }
    // VBO �����͸� ������Ʈ�Ͽ� GPU�� �ٽ� ����
    UpdateVBO();

    // ȭ���� �ٽ� �׸����� ��û
    glutPostRedisplay();
    glutTimerFunc(100, Timer, value);
}

// ���콺 Ŭ�� �̺�Ʈ �ݹ� �Լ�
void Mouse(int button, int state, int x, int y) {
    
    // VBO �����͸� ������Ʈ�Ͽ� GPU�� �ٽ� ����
    UpdateVBO();

    // ȭ���� �ٽ� �׸����� ��û
    glutPostRedisplay();
}

// Ű���� �Է� �̺�Ʈ �ݹ� �Լ�
GLvoid Keyboard(unsigned char key, int x, int y) {

    std::cout << "play timer\n";
    switch (key) {
    case '1':

        for (int i = 0; i < Max; i++)
        {
            mode[i]= false;
            direction1[i] = i % 2 + 1;
            direction2[i] = i % 2 + 1;
        }
        mode[0] = true;
        glutTimerFunc(100, Timer, 1);
        break;
    case '2':
        for (int i = 0; i < Max; i++)
        {
            mode[i] = false;
            direction1[i] = i + 1;
        }
        mode[1] = true;
        glutTimerFunc(100, Timer, 2);
        break;
    case '3':
        for (int i = 0; i < Max; i++)
        {
            direction1[i] = 1;
            mode[i] = false;
        }
        mode[2] = true;
        glutTimerFunc(100, Timer, 3);
        break;
    case '4':
        theta = 0;
        Sp_radius = 0.0f;
        for (int i = 0; i < Max_vertex; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                prev[i][j] = vertex[i][j];
            }
        }
        mode[3] = true;
        glutTimerFunc(100, Timer, 4);
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
