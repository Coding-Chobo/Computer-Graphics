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
#include <optional>
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
struct Coordinate {
    vector<glm::vec3>vertex;
    vector<glm::vec3>color;
    vector<unsigned int>indexlist;
};
struct Object {
    vector<glm::vec3>vertex;
    vector<glm::vec3>color;
    vector<Index>indexlist;
    Translate transform;
    Translate scaling;
    Translate rotation;
    Coordinate path;
    float dx, dy;
    GLint flight_time;
    bool dir{}; // false�� ����
    bool is_sliced{}; // true�� �߸�
};

void read_newline(char* str) {
    char* pos;
    if ((pos = strchr(str, '\n')) != NULL)
        *pos = '\0';
}
Object Make_figure_L(float size, unsigned int mode);
Object Make_figure_R(float size, unsigned int mode);


GLvoid InitBuffer();
GLvoid Make_Matrix(Object obj);
GLvoid Make_Matrix(Coordinate obj);
GLvoid init_Matrix();

GLvoid UpdateVBO(Coordinate object);
GLvoid UpdateVBO(Object cube);

GLvoid Keyboard(unsigned char key, int x, int y);
void Timer(int value); // Ÿ�̸� �ݹ� �Լ�
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void SpecialKeyboard(int key, int x, int y);

GLint CreateShader(const char* file, int type);
GLvoid CreateShaderProgram();
GLvoid Render(GLvoid);
GLvoid Reshape(int w, int h);
void mainLoop();

bool CheckCollisionWithPolygon(const Object& obj, glm::vec3 lineStart, glm::vec3 lineEnd, Translate& cp1, Translate& cp2);
bool CheckLineIntersection(glm::vec3 p1, glm::vec3 p2, glm::vec3 q1, glm::vec3 q2);
bool IsPointInTriangle(glm::vec3 pt, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2);
bool GetIntersectionPoint(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& q1, const glm::vec3& q2, Translate& intersection);


void init_figure();
void read_obj_file(const char* filename, Object& model);
