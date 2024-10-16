#pragma once
//라이브러리 호출
#include <iostream>
#include <gl/glew.h>
#include <vector>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
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
struct Index
{
    int v1;
    int v2;
    int v3;
};
struct Object {
    vector<glm::vec3>vertex;
    vector<glm::vec3>color;
    vector<Index>indexlist;
    GLuint vao;
    GLuint vbo[2];
    GLuint EBO;
};
struct Coordinate {
    vector<glm::vec3>vertex;
    vector<glm::vec3>color;
    vector<unsigned int>indexlist;
    GLuint vao;
    GLuint vbo[2];
    GLuint EBO;
};
//----------------------------- 함수 선언 -------------------------------------
GLvoid Render(GLvoid);
GLvoid Reshape(int w, int h);
void DrawCube(float x, float y, float z, float size, Object cube);
void Draw_Coordinate(Coordinate obj);
GLint CreateShader(const char* file, int type);
GLvoid CreateShaderProgram();
GLvoid InitBuffer();
GLvoid Make_Matrix();
GLvoid UpdateVBO(Coordinate object);
GLvoid UpdateVBO(Object cube);