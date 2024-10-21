#pragma once
//라이브러리 호출
#include <iostream>
#include <gl/glew.h>
#include <GL/gl.h>   // OpenGL 기본 헤더
#include <GL/glu.h>  // OpenGL 유틸리티 헤더
#include <vector>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    unsigned int v1;
    unsigned int v2;
    unsigned int v3;
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
void read_newline(char* str) {
    char* pos;
    if ((pos = strchr(str, '\n')) != NULL)
        *pos = '\0';
}

    //------------`
void Draw_Cube(float x, float y, float z, float size, Object obj);
void Draw_Tetra(float x, float y, float z, float size, Object obj);
void Draw_Coordinate(Coordinate obj);

GLvoid InitBuffer();
GLvoid Make_Matrix();
GLvoid init_Matrix();
GLvoid UpdateVBO(Coordinate object);
GLvoid UpdateVBO(Object cube);

GLvoid Keyboard(unsigned char key, int x, int y);
void Timer(int value); // 타이머 콜백 함수
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
GLint CreateShader(const char* file, int type);
GLvoid CreateShaderProgram();
GLvoid Render(GLvoid);
GLvoid Reshape(int w, int h);
void SpecialKeyboard(int key, int x, int y);
void init_figure();
void read_obj_file(const char* filename, Object& model);