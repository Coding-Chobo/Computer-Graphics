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
#include <time.h>
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
    vector<glm::vec3>follow_vertex;
    float dx, dy;
    GLint flight_time;
    bool dir{}; // false�� ����
    bool is_sliced{}; // true�� �߸�
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
Object Make_figure_L(float size, unsigned int mode);
Object Make_figure_R(float size, unsigned int mode);

void Draw_Coordinate(Coordinate obj);
GLvoid InitBuffer();
GLvoid Make_Matrix(float& orbit_scale, Object obj);
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