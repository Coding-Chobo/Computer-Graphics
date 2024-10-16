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
#define ROW 1000
#define COL 1000
//----------------------------- 구조체 선언 -------------------------------
struct BoundingBox {
    float left;
    float top;
    float right;
    float bottom;
};
struct Vertex {
    float x, y, z;
    float r, g, b;
};
struct Index
{
    int v1;
    int v2;
    int v3;
};
struct Box {
    vector<Vertex>vertex;
    vector<Index>indexlist;
    GLuint VAO;
    GLuint VBO[2];
    GLuint EBO;
};
struct Figure {
    int shape;
    bool is_active{};
    BoundingBox checkbox{};
    int dir1;
    int dir2;
};

//----------------------------- 함수 선언 -------------------------------------

void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
void InitBuffer();
char* filetobuf(const char* file);
void UpdateVBO(Box box);
void UpdateVBO();

GLvoid Keyboard(unsigned char key, int x, int y);
void Timer(int value); // 타이머 콜백 함수
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
GLvoid drawScene();
GLvoid Reshape(int w, int h);


void init_color(int shape);
void init_figure();
void UpdateVBO();
void Make_cube(float x, float y, float z,float size);
