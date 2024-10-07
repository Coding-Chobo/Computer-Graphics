#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <gl/glew.h> // 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <ctime>
#include <cstdlib> // rand() 함수를 사용하기 위해 추가
#include <stdio.h>
#include <iomanip> // For debug output precision
#include <math.h>

#define Max 5
#define Max_vertex  216
#define ROW 1000
#define COL 1000

GLchar* vertexSource, * fragmentSource; // 소스코드 저장 변수
GLuint vertexShader, fragmentShader; // 세이더 객체
GLuint shaderProgramID; // 셰이더 프로그램
GLuint vao, vbo[2]; // VAO와 VBO

//----------------------------- 구조체 선언 -------------------------------

//--------------------------- 전역 변수 ---------------------------------
float xpos{ROW/2}, ypos{COL/2}; // 마우스 클릭 위치 좌표
int limitspiral{}; // 현재 생성된 도형 개수
float radius{ 0.15f }; // 도형의 반지름
GLfloat colors[Max][3][3]; // 각 도형의 색상
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
bool canmake{ true };
//----------------------------- 함수 선언 -------------------------------------
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
void Timer(int value); // 타이머 콜백 함수
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void init_figure();
void MakeSquare(int point);
void UpdateVBO();
//---------------------------------------------------------------------

//삼각형 생성
void Make_triangle() {

}

// 색상 초기화 함수
void init_color() {
    // 각 도형의 색상 값을 초기화
    for (int i = 0; i < Max; ++i) {
        for (int j = 0; j < 3; ++j) {
            colors[i][j][0] = static_cast<float>(rand()) / RAND_MAX;
            colors[i][j][1] = static_cast<float>(rand()) / RAND_MAX;
            colors[i][j][2] = static_cast<float>(rand()) / RAND_MAX;
        }
    }
}
// 도형 초기값 입력
void init_figure() {
    
    for (int i = 0; i < Max; i++)
    {
        currentvertexcnt[i] = 0;
    }
    canmake = true;
    // VBO 데이터를 업데이트하여 GPU에 다시 전송
    UpdateVBO();

    // 화면을 다시 그리도록 요청
    glutPostRedisplay();
}

// 프로그램의 메인 함수
void main(int argc, char** argv) {
    // 윈도우 생성
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(ROW, COL);
    glutCreateWindow("Practical 1");

    srand(time(NULL));

    // GLEW 초기화
    glewExperimental = GL_TRUE;
    glewInit();
    init_color(); // 색상 초기화
    InitBuffer(); // 버퍼 초기화
    make_vertexShaders(); // 버텍스 셰이더 생성
    make_fragmentShaders(); // 프래그먼트 셰이더 생성
    shaderProgramID = make_shaderProgram(); // 셰이더 프로그램 생성
    // 콜백 함수 등록
    glutTimerFunc(300, Timer, 0);
    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutMouseFunc(Mouse);
    glutKeyboardFunc(Keyboard);
    init_figure();
    glutMainLoop(); // 메인 루프 진입
}

// 도형을 생성하고 정점 정보를 설정하는 함수
void MakeSquare(int point) {
 

    
    // VBO 데이터를 업데이트하여 GPU에 다시 전송
    UpdateVBO();

    // 화면을 다시 그리도록 요청
    glutPostRedisplay();
}

// VAO 및 VBO 초기화
void InitBuffer() {
    glGenVertexArrays(1, &vao); // VAO 생성
    glBindVertexArray(vao); // VAO 바인딩

    glGenBuffers(2, vbo); // 2개의 VBO 생성

    // 정점 좌표 데이터를 VBO에 복사
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, currentvertexcnt[0] * sizeof(vertex1), vertex1, GL_DYNAMIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex2), vertex2, GL_DYNAMIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex3), vertex3, GL_DYNAMIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex4), vertex4, GL_DYNAMIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex5), vertex5, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // 색상 데이터를 VBO에 복사
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
}

// VBO 데이터를 업데이트하는 함수
void UpdateVBO() {
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

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



// 화면 그리기 콜백 함수
GLvoid drawScene() {
    // 배경색 설정
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramID); // 셰이더 프로그램 사용

    glBindVertexArray(vao); // VAO 바인딩

    // 도형 그리기: 각 도형의 정점 개수에 따라 다르게 그려야 함
    int offset = 0;
    if (!mode_line)
    {
        for (int i = 0; i < limitspiral; i++)
        {
                glDrawArrays(GL_POINTS, i, currentvertexcnt[i]);
        }
    }
    else
    {
        for (int i = 0; i < Max; i++)
        {
            for (int j = 0; j < currentvertexcnt[i]; j++)
            {
                glDrawArrays(GL_LINES, i, 2);
            }
        }
    }

    glutSwapBuffers(); // 화면에 출력
}

// 윈도우 리사이즈 콜백 함수
GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

//타이머 함수
void Timer(int value) {
    float spiral_gap = 0.001f;

    switch (value)
    {
    case 0: // 원 스파이럴
            

            switch (limitspiral)
            {
            case 1:

                if (canmake)
                {
                    for (int i = 0; i < limitspiral; i++) {
                        if (!reverse[i]) {

                            if (currentvertexcnt[i] < Max_vertex) {
                                std::cout << currentvertexcnt[0] << std::endl;
                                vertex1[currentvertexcnt[i]][0] = vertex1[0][0] + Sp_radius * cos(theta);
                                vertex1[currentvertexcnt[i]][1] = vertex1[0][1] + Sp_radius * sin(theta);
                                theta += 0.15;
                                Sp_radius += spiral_gap;
                                currentvertexcnt[i]++;
                            }
                            else {
                                reverse[i] = true;
                            }
                        }
                        else // 리버스 모드
                        {
                            if (currentvertexcnt[i] < 2 * Max_vertex) {
                                vertex1[currentvertexcnt[i]][0] = vertex1[0][0] + 2 + Sp_radius * cos(theta);
                                vertex1[currentvertexcnt[i]][1] = vertex1[0][1] + Sp_radius * sin(theta);
                                theta -= 0.15;
                                Sp_radius -= spiral_gap;
                                currentvertexcnt[i]++;
                            }
                            else {
                                canmake = false;
                            }
                        }
                    }
                }

                break;
            case 2:

                break;
            case 3:

                break;
            case 4:

                break;
            case 5:

                break;
            default:
                break;
            }

        
        break;
    }
    // VBO 데이터를 업데이트하여 GPU에 다시 전송
    UpdateVBO();

    // 화면을 다시 그리도록 요청
    glutPostRedisplay();
    glutTimerFunc(100, Timer, value);
}

// 마우스 클릭 이벤트 콜백 함수
void Mouse(int button, int state, int x, int y) {

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        glutTimerFunc(100, Timer, 0);
        Sp_radius = 0.0f;
        theta = 0.0f;
        xpos = (static_cast<float>(x) / (ROW / 2.0f)) - 1.0f; // OpenGL 좌표로 변환
        ypos = 1.0f - (static_cast<float>(y) / (COL / 2.0f)); // OpenGL 좌표로 변환
        limitspiral = 1;

        vertex1[0][0] = xpos;
        vertex1[0][1] = ypos;
        vertex1[0][2] = 0.0f;
        init_figure();
    }

    // VBO 데이터를 업데이트하여 GPU에 다시 전송
    UpdateVBO();

    // 화면을 다시 그리도록 요청
    glutPostRedisplay();
}

// 키보드 입력 이벤트 콜백 함수
GLvoid Keyboard(unsigned char key, int x, int y) {

    switch (key) {
    case 'p':case 'P':
        mode_line = false;
        break;
    case 'l': case'L':
        mode_line = true;
        break;
    case '1':

        break;
    case '2':

        break;
    case '3':

        break;
    case '4':

        break;
    case '5':

        break;
    }


    // VBO 데이터를 업데이트하여 GPU에 다시 전송
    UpdateVBO();

    // 화면을 다시 그리도록 요청
    glutPostRedisplay();
}

// 셰이더 프로그램 생성 함수
GLuint make_shaderProgram() {
    GLuint shaderID = glCreateProgram();
    glAttachShader(shaderID, vertexShader);
    glAttachShader(shaderID, fragmentShader);
    glLinkProgram(shaderID);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderID;
}

// 버텍스 셰이더 생성 함수
void make_vertexShaders() {
    vertexSource = filetobuf("vertex.vs");
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
    glCompileShader(vertexShader);
}

// 프래그먼트 셰이더 생성 함수
void make_fragmentShaders() {
    fragmentSource = filetobuf("fragment.fs");
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
    glCompileShader(fragmentShader);
}

// 파일을 버퍼로 읽어오는 함수
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
