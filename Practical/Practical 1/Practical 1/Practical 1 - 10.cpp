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

#define Max 5
#define Max_vertex  216 //432
#define ROW 1000
#define COL 1000
#define PI 3.141592653589793238
GLchar* vertexSource, * fragmentSource; // �ҽ��ڵ� ���� ����
GLuint vertexShader, fragmentShader; // ���̴� ��ü
GLuint shaderProgramID; // ���̴� ���α׷�
GLuint vao, vbo[2]; // VAO�� VBO

//----------------------------- ����ü ���� -------------------------------

//--------------------------- ���� ���� ---------------------------------
float xpos{ROW/2}, ypos{COL/2}; // ���콺 Ŭ�� ��ġ ��ǥ
int limitspiral{}; // ���� ������ ���� ����
float radius{ 0.15f }; // ������ ������
GLfloat colors[10 * Max_vertex][3][3]; // �� ������ ����
GLfloat vertex1[2 * Max_vertex][3]{};
GLfloat vertex2[2 * Max_vertex][3]{};
GLfloat vertex3[2 * Max_vertex][3]{};
GLfloat vertex4[2 * Max_vertex][3]{};
GLfloat vertex5[2 * Max_vertex][3]{};
int currentvertexcnt[Max]{};
int direction1[4]{};
int direction2[4]{};
float Sp_radius{};
GLfloat rect_sprialgap[4]{ 0.0f };
float theta{};
int step[Max]{};
bool reverse[Max]{};
bool canmake[5]{ true };
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

}

// ���� �ʱ�ȭ �Լ�
void init_color() {
    // �� ������ ���� ���� �ʱ�ȭ
    for (int i = 0; i < 10 * Max_vertex; ++i) {
        for (int j = 0; j < 3; ++j) {
            colors[i][j][0] = 1.0f;
            colors[i][j][1] = 1.0f;
            colors[i][j][2] = 1.0f;
        }
    }
}
// ���� �ʱⰪ �Է�
void init_figure() {
    
    for (int i = 0; i < Max; i++)
    {
        currentvertexcnt[i] = 0;
        reverse[i] = false;
    }
    ZeroMemory(vertex1, sizeof(vertex1));
    ZeroMemory(vertex2, sizeof(vertex2));
    ZeroMemory(vertex3, sizeof(vertex3));
    ZeroMemory(vertex4, sizeof(vertex4));
    ZeroMemory(vertex5, sizeof(vertex5));
    theta = 0.0f;
    Sp_radius = 0.0f;
    
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
    // �ݹ� �Լ� ���
    glutTimerFunc(50, Timer, 0);
    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutMouseFunc(Mouse);
    glutKeyboardFunc(Keyboard);
    init_figure();
    glutMainLoop(); // ���� ���� ����
}

// ������ �����ϰ� ���� ������ �����ϴ� �Լ�
void MakeSquare(int point) {
 

    
    // VBO �����͸� ������Ʈ�Ͽ� GPU�� �ٽ� ����
    UpdateVBO();

    // ȭ���� �ٽ� �׸����� ��û
    glutPostRedisplay();
}

// VAO �� VBO �ʱ�ȭ
void InitBuffer() {
    glGenVertexArrays(1, &vao);  // VAO ����
    glBindVertexArray(vao);  // VAO ���ε�

    glGenBuffers(2, vbo);  // 2���� VBO ����

    // ���� ��ǥ �����͸� VBO�� ����
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex1) + sizeof(vertex2) + sizeof(vertex3) + sizeof(vertex4) + sizeof(vertex5), vertex1, GL_DYNAMIC_DRAW);

    // ��� vertex �����͸� �ϳ��� VBO�� �����Ͽ� ����
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex1), vertex1);
    glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(vertex1),  sizeof(vertex2), vertex2);
    glBufferSubData(GL_ARRAY_BUFFER, 3 * (sizeof(vertex1) + sizeof(vertex2)), sizeof(vertex3), vertex3);
    glBufferSubData(GL_ARRAY_BUFFER, 3 * (sizeof(vertex1) + sizeof(vertex2) + sizeof(vertex3)), sizeof(vertex4), vertex4);
    glBufferSubData(GL_ARRAY_BUFFER, 3 * (sizeof(vertex1) + sizeof(vertex2) + sizeof(vertex3) + sizeof(vertex4)), sizeof(vertex5), vertex5);

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

    // �� VBO�� ���� �����Ͱ� �ùٸ��� ������Ʈ�ǰ� �ִ��� Ȯ��
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex1), vertex1);
    if (limitspiral >= 2) {
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertex1), sizeof(vertex2), vertex2);
    }
    if (limitspiral >= 3) {
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertex1) + sizeof(vertex2), sizeof(vertex3), vertex3);
    }
    if (limitspiral >= 4) {
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertex1) + sizeof(vertex2) + sizeof(vertex3), sizeof(vertex4), vertex4);
    }
    if (limitspiral >= 5) {
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertex1) + sizeof(vertex2) + sizeof(vertex3) + sizeof(vertex4), sizeof(vertex5), vertex5);
    }
}




// ȭ�� �׸��� �ݹ� �Լ�
GLvoid drawScene() {
    // ���� ���� �� ȭ�� Ŭ����
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramID);  // ���̴� ���α׷� ���

    glBindVertexArray(vao);  // VAO ���ε�

    // ���� �׸���: �� ������ ���� ������ ���� �ٸ��� �׷��� ��
    int offset = 0;

    if (!mode_line) {  // ������ �׸���
        for (int i = 0; i < limitspiral; i++) {
            offset += currentvertexcnt[i];  // ���� �����̷��� ���� ��ġ�� �������� ����
        }
        glDrawArrays(GL_POINTS, 1, 2  * offset);
    }
    else {  // ������ �׸���
        offset = 0;
        for (int i = 0; i < limitspiral; i++) {
             offset += currentvertexcnt[i];  // ���� �����̷��� ���� ��ġ�� �������� ����
        }
        glDrawArrays(GL_LINE_STRIP, 1, 2 * offset);  // �� �����̷��� ������ �׸���
    }
    std::cout << offset << std::endl;
    glutSwapBuffers();  // ȭ�鿡 ���
}


// ������ �������� �ݹ� �Լ�
GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

//Ÿ�̸� �Լ�
void Timer(int value) {
    float spiral_gap = 0.001f;

    switch (value)
    {
    case 0: // �� �����̷�
            
            
            //1�� �����̷�
            if (!reverse[0]) {

                if (theta < PI * 6) {
                    std::cout << currentvertexcnt[2] << std::endl;
                    vertex1[currentvertexcnt[0]][0] = vertex1[0][0] + Sp_radius * cos(theta);
                    vertex1[currentvertexcnt[0]][1] = vertex1[0][1] + Sp_radius * sin(theta);
                    theta += 0.15;
                    Sp_radius += spiral_gap;
                    currentvertexcnt[0]++;
                }
                else {
                    reverse[0] = true;
                    theta = PI * 5;
                }
            }
            else // ������ ���
            {
                if (theta > -PI) {
                    vertex1[currentvertexcnt[0]][0] = vertex1[0][0] + 0.25f + Sp_radius * cos(theta);
                    vertex1[currentvertexcnt[0]][1] = vertex1[0][1] + Sp_radius * sin(theta);
                    theta -= 0.15;
                    Sp_radius -= spiral_gap;
                    currentvertexcnt[0]++;
                }
            }
            //2�� �����̷�
            if (limitspiral >= 2)
            {
                if (!reverse[1]) {

                    if (theta < PI * 6) {
                        vertex2[currentvertexcnt[1]][0] = vertex2[0][0] + Sp_radius * cos(theta);
                        vertex2[currentvertexcnt[1]][1] = vertex2[0][1] + Sp_radius * sin(theta);
                        currentvertexcnt[1]++;
                    }
                    else {
                        reverse[1] = true;
                    }
                }
                else // ������ ���
                {
                    if (theta > -PI) {
                        vertex2[currentvertexcnt[1]][0] = vertex2[0][0] + 0.25f + Sp_radius * cos(theta);
                        vertex2[currentvertexcnt[1]][1] = vertex2[0][1] + Sp_radius * sin(theta);
                        currentvertexcnt[1]++;
                    }
                }
            }
            //3�� �����̷�
            if (limitspiral >= 3)
            {
                if (!reverse[2]) {

                    if (theta < PI * 6) {
                        vertex3[currentvertexcnt[2]][0] = vertex3[0][0] + Sp_radius * cos(theta);
                        vertex3[currentvertexcnt[2]][1] = vertex3[0][1] + Sp_radius * sin(theta);
                        currentvertexcnt[2]++;
                    }
                    else {
                        reverse[2] = true;
                    }
                }
                else // ������ ���
                {
                    if (theta > -PI) {
                        vertex3[currentvertexcnt[2]][0] = vertex3[0][0] + 0.25f + Sp_radius * cos(theta);
                        vertex3[currentvertexcnt[2]][1] = vertex3[0][1] + Sp_radius * sin(theta);
                        currentvertexcnt[2]++;
                    }
                }
            }
            //4�� �����̷�
            if (limitspiral >= 4)
            {
                if (!reverse[3]) {

                    if (theta < PI * 6) {
                        vertex4[currentvertexcnt[3]][0] = vertex4[0][0] + Sp_radius * cos(theta);
                        vertex4[currentvertexcnt[3]][1] = vertex4[0][1] + Sp_radius * sin(theta);
                        currentvertexcnt[3]++;
                    }
                    else {
                        reverse[3] = true;
                    }
                }
                else // ������ ���
                {
                    if (theta > -PI) {
                        vertex4[currentvertexcnt[3]][0] = vertex4[0][0] + 0.25f + Sp_radius * cos(theta);
                        vertex4[currentvertexcnt[3]][1] = vertex4[0][1] + Sp_radius * sin(theta);
                        currentvertexcnt[3]++;
                    }
                }
            }
            //5�� �����̷�
            if (limitspiral == 5)
            {
                if (!reverse[4]) {

                    if (theta < PI * 6) {
                        vertex5[currentvertexcnt[4]][0] = vertex5[0][0] + Sp_radius * cos(theta);
                        vertex5[currentvertexcnt[4]][1] = vertex5[0][1] + Sp_radius * sin(theta);
                        currentvertexcnt[4]++;
                    }
                    else {
                        reverse[4] = true;
                    }
                }
                else // ������ ���
                {
                    if (theta > -PI) {
                        vertex5[currentvertexcnt[4]][0] = vertex5[0][0] + 0.25f + Sp_radius * cos(theta);
                        vertex5[currentvertexcnt[4]][1] = vertex5[0][1] + Sp_radius * sin(theta);
                        currentvertexcnt[4]++;
                    }
                }
            }

        
        break;
    }
    // VBO �����͸� ������Ʈ�Ͽ� GPU�� �ٽ� ����
    UpdateVBO();

    // ȭ���� �ٽ� �׸����� ��û
    glutPostRedisplay();
    glutTimerFunc(10, Timer, value);
}

// ���콺 Ŭ�� �̺�Ʈ �ݹ� �Լ�
void Mouse(int button, int state, int x, int y) {

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        Sp_radius = 0.0f;
        theta = 0.0f;
        xpos = (static_cast<float>(x) / (ROW / 2.0f)) - 1.0f; // OpenGL ��ǥ�� ��ȯ
        ypos = 1.0f - (static_cast<float>(y) / (COL / 2.0f)); // OpenGL ��ǥ�� ��ȯ
        limitspiral = 1;

        vertex1[0][0] = xpos;
        vertex1[0][1] = ypos;
        vertex1[0][2] = 0.0f;
        init_figure();
    }

    // VBO �����͸� ������Ʈ�Ͽ� GPU�� �ٽ� ����
    UpdateVBO();

    // ȭ���� �ٽ� �׸����� ��û
    glutPostRedisplay();
}
void init_rnd_position(GLfloat vertex[2 * Max_vertex][3]) {
    
    if (rand() % 2)
    {
        vertex[0][0] = static_cast<float>(rand()) / RAND_MAX;
    }
    else
    {
        vertex[0][0] = -static_cast<float>(rand()) / RAND_MAX;
    }
    if (rand() % 2)
    {
        vertex[0][1] = static_cast<float>(rand()) / RAND_MAX;
    }
    else
    {
        vertex[0][1] = -static_cast<float>(rand()) / RAND_MAX;
    }

}
// Ű���� �Է� �̺�Ʈ �ݹ� �Լ�
GLvoid Keyboard(unsigned char key, int x, int y) {

    switch (key) {
    case 'p':case 'P':
        mode_line = false;
        break;
    case 'l': case'L':
        mode_line = true;
        break;
    case '1':
        init_figure();
        InitBuffer();
        limitspiral = 1;
        init_rnd_position(vertex1);
        break;
    case '2':
        init_figure();
        InitBuffer();
        limitspiral = 2;
        init_rnd_position(vertex1);
        init_rnd_position(vertex2);
        break;
    case '3':
        init_figure();
        InitBuffer();
        limitspiral = 3;
        init_rnd_position(vertex1);
        init_rnd_position(vertex2);
        init_rnd_position(vertex3);
        break;
    case '4':
        init_figure();
        InitBuffer();
        limitspiral = 4;
        init_rnd_position(vertex1);
        init_rnd_position(vertex2);
        init_rnd_position(vertex3);
        init_rnd_position(vertex4);
        break;
    case '5':
        init_figure();
        InitBuffer();
        limitspiral = 5;
        init_rnd_position(vertex1);
        init_rnd_position(vertex2);
        init_rnd_position(vertex3);
        init_rnd_position(vertex4);
        init_rnd_position(vertex5);
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
