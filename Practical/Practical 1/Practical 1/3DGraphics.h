#pragma once
//라이브러리 호출
#include <iostream>
#include <gl/glew.h> // 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <ctime>
#include <cstdlib> // rand() 함수를 사용하기 위해 추가
#include <stdio.h>
#include <iomanip> // For debug output precision
#include <math.h>
#include <vector>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>

//--------------------------------define--------------------------------
using std::vector;
#define Max 15
#define ROW 1000
#define COL 1000
//----------------------------- 구조체 선언 -------------------------------
struct Vertex {
    float x, y, z;
};
struct Color {
    float r, g, b;
};
struct Figure {
    int shape;
    bool is_active{};
    RECT checkbox{};
};

//----------------------------- 함수 선언 -------------------------------------

void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
void InitBuffer();
char* filetobuf(const char* file);

GLvoid Keyboard(unsigned char key, int x, int y);
void Timer(int value); // 타이머 콜백 함수
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
GLvoid drawScene();
GLvoid Reshape(int w, int h);

void init_color(int shape);
void init_figure();
void UpdateVBO();
void Make_Line(int mode);
void Make_triangle(int size);
void Make_Rect(int mode);
void Make_Penta(int mode);
