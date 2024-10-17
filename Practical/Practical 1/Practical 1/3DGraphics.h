#pragma once
//���̺귯�� ȣ��
#include <iostream>
#include <gl/glew.h> // �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <ctime>
#include <cstdlib> // rand() �Լ��� ����ϱ� ���� �߰�
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
//----------------------------- ����ü ���� -------------------------------
struct BoundingBox {
    float left;
    float top;
    float right;
    float bottom;
};
struct Vertex {
    float x, y, z;
};
struct Color {
    float r, g, b;
};
struct Figure {
    int shape;
    bool is_active{};
    BoundingBox checkbox{};
    int dir1;
    int dir2;
};

//----------------------------- �Լ� ���� -------------------------------------

void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
void InitBuffer();
char* filetobuf(const char* file);

GLvoid Keyboard(unsigned char key, int x, int y);
void Timer(int value); // Ÿ�̸� �ݹ� �Լ�
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
GLvoid drawScene();
GLvoid Reshape(int w, int h);

bool IsRectOverlap(BoundingBox rect1, BoundingBox rect2);
void init_color(int shape);
void init_figure();
void UpdateVBO();
void Make_Line(int mode);
void Make_triangle(int size);
void Make_Rect(int mode);
void Make_Penta(int mode);
