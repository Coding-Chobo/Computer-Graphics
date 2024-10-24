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
//----------------------------- 구조체 선언 -------------------------------
struct Camera_Info
{
    GLfloat x, y, z;
    GLfloat at_x, at_y, at_z;
};
struct Index
{
    unsigned int v1;
    unsigned int v2;
    unsigned int v3;
};
struct Translate
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
};
struct Object {
    vector<glm::vec3>vertex;
    vector<glm::vec3>color;
    vector<Index>indexlist;
    Translate transform;
    Translate scaling;
    Translate rotation;
};
struct Coordinate {
    vector<glm::vec3>vertex;
    vector<glm::vec3>color;
    vector<unsigned int>indexlist;
};
void read_newline(char* str) {
    char* pos;
    if ((pos = strchr(str, '\n')) != NULL)
        *pos = '\0';
}
void Make_Cube(float x, float y, float z, float size, Object& obj);
void Make_Tetra(float x, float y, float z, float size, Object& obj);
void Make_Corn(float x, float y, float z, float size, Object& obj);
void Make_Spiral(float x, float y, float z, Coordinate& obj);
void Draw_Coordinate(Coordinate obj);
void Draw_Cube(float x, float y, float z, float size, Object obj);
void Draw_Tetra(float x, float y, float z, float size, Object obj);
GLvoid InitBuffer();
GLvoid Make_Matrix();

GLvoid Make_Matrix(float& orbit_scale,Object obj);
GLvoid Make_Matrix(Object obj);
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
void mainLoop();
void SpecialKeyboard(int key, int x, int y);
void init_figure();
void read_obj_file(const char* filename, Object& model);
void swap_figure(Object& obj1, Object& obj2);