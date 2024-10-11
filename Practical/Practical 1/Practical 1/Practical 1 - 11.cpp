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
#define Max_vertex 30
#define ROW 1000
#define COL 1000

GLchar* vertexSource, * fragmentSource; // �ҽ��ڵ� ���� ����
GLuint vertexShader, fragmentShader; // ���̴� ��ü
GLuint shaderProgramID; // ���̴� ���α׷�
GLuint vao, vbo[2]; // VAO�� VBO

//----------------------------- ����ü ���� -------------------------------

//--------------------------- ���� ���� ---------------------------------
float xpos{}, ypos{}; // ���콺 Ŭ�� ��ġ ��ǥ
int currentblockcnt{}; // ���� ������ ���� ����
float radius{ 0.15f }; // ������ ������
GLfloat colors[Max_vertex][3][3]; // �� ������ ����
GLfloat vertex[Max_vertex][3];
int currentvertexcnt{};
int reverse[4]{};
int reverse_alone{};
int mode{};
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
void Makeshape();
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
    for (int i = 0; i < Max_vertex; ++i) {
        for (int j = 0; j < 3; ++j) {
            colors[i][j][0] = static_cast<float>(rand()) / RAND_MAX;
            colors[i][j][1] = static_cast<float>(rand()) / RAND_MAX;
            colors[i][j][2] = static_cast<float>(rand()) / RAND_MAX;
        }
    }
}
// ���� �ʱⰪ �Է�
void init_figure() {
    ZeroMemory(vertex, sizeof(vertex)); // ���� �迭�� 0���� �ʱ�ȭ
    currentvertexcnt = 0; // ���� ���� ���� �ʱ�ȭ

    // 1��и� - �ﰢ�� ����
    vertex[currentvertexcnt][0] = -1.0f / 2.0f;
    vertex[currentvertexcnt][1] = 2.0f / 3.0f;
    vertex[currentvertexcnt][2] = 0.0f;
    currentvertexcnt++;

    vertex[currentvertexcnt][0] = -2.0f / 3.0f;
    vertex[currentvertexcnt][1] = 1.0f / 3.0f;
    vertex[currentvertexcnt][2] = 0.0f;
    currentvertexcnt++;

    vertex[currentvertexcnt][0] = -2.0f / 3.0f;
    vertex[currentvertexcnt][1] = 1.0f / 3.0f;
    vertex[currentvertexcnt][2] = 0.0f;
    currentvertexcnt++;

    // 2��и� - �簢�� ����
    vertex[currentvertexcnt][0] = 1.0f / 4.0f;
    vertex[currentvertexcnt][1] = 1.0f / 4.0f;
    vertex[currentvertexcnt][2] = 0.0f;
    currentvertexcnt++;

    vertex[currentvertexcnt][0] = 3.0f / 4.0f;
    vertex[currentvertexcnt][1] = 1.0f / 4.0f;
    vertex[currentvertexcnt][2] = 0.0f;
    currentvertexcnt++;

    vertex[currentvertexcnt][0] = 1.0f / 2.0f;
    vertex[currentvertexcnt][1] = 3.0f / 4.0f;
    vertex[currentvertexcnt][2] = 0.0f;
    currentvertexcnt++;

    //3 ���簢���� ������ �ﰢ���� ������
    vertex[currentvertexcnt][0] = 1.0f / 2.0f;
    vertex[currentvertexcnt][1] = 3.0f / 4.0f;
    vertex[currentvertexcnt][2] = 0.0f;
    currentvertexcnt++;

    vertex[currentvertexcnt][0] = 1.0f / 2.0f;
    vertex[currentvertexcnt][1] = 3.0f / 4.0f;
    vertex[currentvertexcnt][2] = 0.0f;
    currentvertexcnt++;

    vertex[currentvertexcnt][0] = 3.0f / 4.0f;
    vertex[currentvertexcnt][1] = 1.0f / 4.0f;
    vertex[currentvertexcnt][2] = 0.0f;
    currentvertexcnt++;
     
    //4 ������ �ﰢ�� 3��
    vertex[currentvertexcnt][0] = - 2.0f / 3.0f;
    vertex[currentvertexcnt][1] = -3.0f / 4.0f;
    vertex[currentvertexcnt][2] = 0.0f;
    currentvertexcnt++;

    vertex[currentvertexcnt][0] = -1.0f / 3.0f;
    vertex[currentvertexcnt][1] = -3.0f / 4.0f;
    vertex[currentvertexcnt][2] = 0.0f;
    currentvertexcnt++;

    vertex[currentvertexcnt][0] = -2.0f / 3.0f;
    vertex[currentvertexcnt][1] = -1.0f / 2.0f;
    vertex[currentvertexcnt][2] = 0.0f;
    currentvertexcnt++;
    //5 �� �밢
    vertex[currentvertexcnt][0] = -2.0f / 3.0f;
    vertex[currentvertexcnt][1] = -1.0f / 2.0f;
    vertex[currentvertexcnt][2] = 0.0f;
    currentvertexcnt++;

    vertex[currentvertexcnt][0] = -1.0f / 3.0f;
    vertex[currentvertexcnt][1] = -1.0f / 2.0f;
    vertex[currentvertexcnt][2] = 0.0f;
    currentvertexcnt++;

    vertex[currentvertexcnt][0] = -1.0f / 3.0f;
    vertex[currentvertexcnt][1] = -3.0f / 4.0f;
    vertex[currentvertexcnt][2] = 0.0f;
    currentvertexcnt++;
    //6
    vertex[currentvertexcnt][0] = -1.0f / 3.0f;
    vertex[currentvertexcnt][1] = -1.0f / 2.0f;
    vertex[currentvertexcnt][2] = 0.0f;
    currentvertexcnt++;

    vertex[currentvertexcnt][0] = -2.0f / 3.0f;
    vertex[currentvertexcnt][1] = -1.0f / 2.0f;
    vertex[currentvertexcnt][2] = 0.0f;
    currentvertexcnt++;

    vertex[currentvertexcnt][0] = -1.0f / 2.0f;
    vertex[currentvertexcnt][1] = -1.0f / 2.0f;
    vertex[currentvertexcnt][2] = 0.0f;
    currentvertexcnt++;

    // 7������ -- > �� ���� ������
    vertex[currentvertexcnt][0] = 3.0f / 8.0f;
    vertex[currentvertexcnt][1] = -3.0f / 4.0f;
    vertex[currentvertexcnt][2] = 0.0f;
    currentvertexcnt++;

    vertex[currentvertexcnt][0] = 5.0f / 8.0f;
    vertex[currentvertexcnt][1] = -3.0f / 4.0f;
    vertex[currentvertexcnt][2] = 0.0f;
    currentvertexcnt++;

    vertex[currentvertexcnt][0] = 1.0f / 4.0f;
    vertex[currentvertexcnt][1] = -1.0f / 2.0f;
    vertex[currentvertexcnt][2] = 0.0f;
    currentvertexcnt++;
    //8 �� �밢
    vertex[currentvertexcnt][0] = 1.0f / 4.0f;
    vertex[currentvertexcnt][1] = -1.0f / 2.0f;
    vertex[currentvertexcnt][2] = 0.0f;
    currentvertexcnt++;

    vertex[currentvertexcnt][0] = 3.0f / 4.0f;
    vertex[currentvertexcnt][1] = -1.0f / 2.0f;
    vertex[currentvertexcnt][2] = 0.0f;
    currentvertexcnt++;

    vertex[currentvertexcnt][0] = 5.0f / 8.0f;
    vertex[currentvertexcnt][1] = -3.0f / 4.0f;
    vertex[currentvertexcnt][2] = 0.0f;
    currentvertexcnt++;
    //9
    vertex[currentvertexcnt][0] = 3.0f / 4.0f;
    vertex[currentvertexcnt][1] = -1.0f / 2.0f;
    vertex[currentvertexcnt][2] = 0.0f;
    currentvertexcnt++;

    vertex[currentvertexcnt][0] = 1.0f / 4.0f;
    vertex[currentvertexcnt][1] = -1.0f / 2.0f;
    vertex[currentvertexcnt][2] = 0.0f;
    currentvertexcnt++;

    vertex[currentvertexcnt][0] = 1.0f / 2.0f;
    vertex[currentvertexcnt][1] = -3.0f / 8.0f;
    vertex[currentvertexcnt][2] = 0.0f;
    currentvertexcnt++;

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

    glutTimerFunc(100, Timer, 0);
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
    switch (mode)
    {
    case 0:
        for (int i = 0; i < currentvertexcnt; i += 3) {
            glDrawArrays(GL_TRIANGLES, i, 3); // �ﰢ��: 3���� ����
        }
        break;
    case 1:
        for (int i = 0; i < 3; i += 3)
        {
            glDrawArrays(GL_TRIANGLES, i, 3); // �ﰢ��: 3���� ����
        }
        break;
    case 2:
        for (int i = 3; i < 9; i += 3)
        {
            glDrawArrays(GL_TRIANGLES, i, 3); // �ﰢ��: 3���� ����
        }
        break;
    case 3:
        for (int i = 9; i < 18; i += 3)
        {
            glDrawArrays(GL_TRIANGLES, i, 3); // �ﰢ��: 3���� ����
        }
        break;
    case 4:
        for (int i = 18; i < 27; i += 3)
        {
            glDrawArrays(GL_TRIANGLES, i, 3); // �ﰢ��: 3���� ����
        }
        break;
    default:
        break;
    }

        

    glutSwapBuffers(); // ȭ�鿡 ���
}

// ������ �������� �ݹ� �Լ�
GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

//Ÿ�̸� �Լ�
void Timer(int value) {
    float moving_gap_t = 0.01f;
    float moving_gap_r = 0.005f;
    // �� --> �ﰢ��
    if (mode == 0|| mode == 1 )
    {
        if (!reverse[0])
        {
            if (vertex[2][0] < -1.0f / 3.0f)
            {
                vertex[2][0] += moving_gap_t;
            }
            else
            {
                reverse[0] = true;
            }
        }
        else
        {
            if (vertex[2][0] > -2.0f / 3.0f)
            {
                vertex[2][0] -= moving_gap_t;
            }
            else
            {
                reverse[0] = false;
            }
        }
    }
    // �ﰢ�� --> �簢��
    if (mode == 0 || mode == 2)
    {
        if (!reverse[1])
        {
            if (vertex[5][0] > 1.0f / 4.0f)
            {
                vertex[5][0] -= moving_gap_r;
                vertex[6][0] -= moving_gap_r;
                vertex[7][0] += moving_gap_r;
            }
            else
            {
                reverse[1] = true;
            }
        }
        else
        {
            if (vertex[5][0] < 1.0f / 2.0f)
            {
                vertex[5][0] += moving_gap_r;
                vertex[6][0] += moving_gap_r;
                vertex[7][0] -= moving_gap_r;
            }
            else
            {
                reverse[1] = false;
            }
        }
    }
    // �簢�� --> ������
    if (mode == 0 || mode == 3)
    {
        if (!reverse[2])
        {
            if (vertex[16][0] > -3.0f / 4.0f)
            {
                vertex[11][0] -= moving_gap_r;
                vertex[12][0] -= moving_gap_r;
                vertex[16][0] -= moving_gap_r;
            }
            if (vertex[15][0] < -1.0f / 4.0f)
            {
                vertex[15][0] += moving_gap_r;
                vertex[13][0] += moving_gap_r;
            }
            if (vertex[17][1] < -3.0f / 8.0f)
            {
                vertex[17][1] += moving_gap_r;
            }
            else
            {
                reverse[2] = true;
            }
        }
        else
        {
            if (vertex[16][0] < -2.0f / 3.0f)
            {
                vertex[11][0] += moving_gap_r;
                vertex[12][0] += moving_gap_r;
                vertex[16][0] += moving_gap_r;
            }
            if (vertex[15][0] > -1.0f / 3.0f)
            {
                vertex[15][0] -= moving_gap_r;
                vertex[13][0] -= moving_gap_r;
            }
            if (vertex[17][1] > -1.0f / 2.0f)
            {
                vertex[17][1] -= moving_gap_r;
            }
            else
            {
                reverse[2] = false;
            }
        }
    } 
    // ������ --> ��
    if (mode == 0 || mode == 4)
    {
        if (!reverse[3])
        {
            if (vertex[18][0] < 1.0f / 2.0f)
            {
                vertex[18][0] += moving_gap_r * 2 / 3;
            }
            else
            {
                vertex[18][0] = 0.5f;
            }
            if (vertex[19][0] > 0.505f)
            {
                vertex[19][0] -= moving_gap_r * 2 / 3;
                vertex[23][0] -= moving_gap_r * 2 / 3;
            }
            else
            {
                vertex[19][0] = 0.505f;
                vertex[23][0] = 0.505f;
            }
            if (vertex[20][0] < 1.0f / 2.0f)
            {
                vertex[20][0] += moving_gap_r;
                vertex[21][0] += moving_gap_r;
                vertex[25][0] += moving_gap_r;
            }
            if (vertex[22][0] > 0.505f)
            {
                vertex[22][0] -= moving_gap_r;
                vertex[24][0] -= moving_gap_r;
            }
            else
            {
                reverse[3] = true;
            }
        }
        else
        {
            if (vertex[18][0] > 3.0f / 8.0f)
            {
                vertex[18][0] -= moving_gap_r * 2 / 3;
            }
            if (vertex[19][0] < 5.0f / 8.0f)
            {
                vertex[19][0] += moving_gap_r * 2 / 3;
                vertex[23][0] += moving_gap_r * 2 / 3;
            }
            if (vertex[20][0] > 1.0f / 4.0f)
            {
                vertex[20][0] -= moving_gap_r;
                vertex[21][0] -= moving_gap_r;
                vertex[25][0] -= moving_gap_r;
            }
            if (vertex[22][0] < 3.0f / 4.0f)
            {
                vertex[22][0] += moving_gap_r;
                vertex[24][0] += moving_gap_r;
            }
            else
            {
                reverse[3] = false;
            }
        }
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
    for (int i = 0; i < Max; i++)
    {
        reverse[i] = false;
    }
    switch (key) {
    case 'l': case 'L': // �� --> �ﰢ��
        init_figure();
        mode = 1;
        break;
    case 't': case 'T': // �ﰢ�� --> �簢��
        init_figure();
        mode = 2;
        break;
    case 'r': case 'R': // �簢�� --> ������
        init_figure();
        mode = 3;
        break;

    case 'p': case 'P': // ������ --> ��
        init_figure();
        mode = 4;
        break;
    case 'a': case 'A': // ��� �׷����� ��
        init_figure();
        mode = 0;
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
