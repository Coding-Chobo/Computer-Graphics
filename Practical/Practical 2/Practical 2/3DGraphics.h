#pragma once
//���̺귯�� ȣ��
#include <iostream>
#include <gl/glew.h>
#include <GL/gl.h>   // OpenGL �⺻ ���
#include <GL/glu.h>  // OpenGL ��ƿ��Ƽ ���
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
//----------------------------- ����ü ���� -------------------------------
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
struct Gauge
{
    float x;
    float y;
    float z;
};
struct Object {
    vector<glm::vec3>vertex;
    vector<glm::vec3>color;
    vector<Index>indexlist;
    GLuint vao;
    GLuint vbo[2];
    GLuint EBO;
    Gauge transform;
    Gauge scaling;
    Gauge rotation;
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
void Make_Cube(float x, float y, float z, float size, Object& obj);
void Make_Tetra(float x, float y, float z, float size, Object& obj);
void Make_Corn(float x, float y, float z, float size, Object& obj);
void Draw_Coordinate(Coordinate obj);
void Draw_Cube(float x, float y, float z, float size, Object obj);
void Draw_Tetra(float x, float y, float z, float size, Object obj);
GLvoid InitBuffer();
GLvoid Make_Matrix();
GLvoid Make_Matrix(float& orbit_angle, float self_scale_x, float self_scale_y, float self_scale_z, float self_rotate_x, float self_rotate_y, float self_rotate_z, float self_trans_x, float self_trans_y, float self_trans_z);
GLvoid init_Matrix();
GLvoid UpdateVBO(Coordinate object);
GLvoid UpdateVBO(Object cube);

GLvoid Keyboard(unsigned char key, int x, int y);
void Timer(int value); // Ÿ�̸� �ݹ� �Լ�
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