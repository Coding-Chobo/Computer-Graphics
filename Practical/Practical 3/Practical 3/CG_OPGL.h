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
struct FreeCamera
{
    GLfloat x, y, z;
    GLfloat at_x, at_y, at_z;
    glm::vec3 forward{}, up{}, right{};
    void rotation_camera_vector_xz(float angle);
};
class FixedCamera
{    
public:
    GLfloat *x, *y, *z;
    glm::vec3 forward{}, up{}, right{};
    void rotation_camera_vector_xz(float angle);
};
struct Index
{
    unsigned int v1;
    unsigned int v2;
    unsigned int v3;
};
struct XYZ
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
};
struct BoundingBox {
    XYZ left_front_top; // min x, max y, max z 
    XYZ left_front_bottom;// min x, min y, max z 
    XYZ right_front_top;// max x, max y, max z 
    XYZ right_front_bottom;// max x, min y, max z 
    XYZ left_back_top; // min x, max y, min z 
    XYZ left_back_bottom;// min x, min y, min z 
    XYZ right_back_top;// max x, max y, min z
    XYZ right_back_bottom;// max x, min y, min z 
};

class Object {
public:
    vector<glm::vec3>vertex;
    vector<glm::vec3>color;
    vector<Index>indexlist;
    XYZ transform{};
    XYZ scaling{};
    XYZ rotation{};
    XYZ tps_position{};
    BoundingBox hitbox{};
    float speed{};
    float sensitivity{};
    glm::vec3 direction{};
    bool is_jump{};
    bool is_move{};
    int flight_time{};
    float dir_x{};
    float dir_z{};
    glm::mat4 Matrix;
    void UpdateVBO();
    void Make_Matrix();
    void Draw_object();
    void UpdateBoundingBox();
    Object();
};
Object::Object() {
    Matrix = glm::mat4(1.0f);
    transform = { 0.0f,0.0f,0.0f };
    rotation = { 0.0f,0.0f,0.0f };
    scaling = { 1.0f,1.0f ,1.0f };
    sensitivity = 4.0;
}

struct Coordinate {
    vector<glm::vec3>vertex;
    vector<glm::vec3>color;
    vector<unsigned int>indexlist;
};
//----------------------------------------------------------
void read_newline(char* str) {
    char* pos;
    if ((pos = strchr(str, '\n')) != NULL)
        *pos = '\0';
}

void init_figure();
//ť�� �����׸���
void Make_cube_top(Object& obj, float size);
void Make_cube_bottom(Object& obj, float size);
void Make_cube_Left(Object& obj, float size);
void Make_cube_Right(Object& obj, float size);
void Make_cube_front(Object& obj, float size);
void Make_cube_back(Object& obj, float size);
void Make_cube_front_left(Object& obj, float size);
void Make_cube_front_right(Object& obj, float size);

void Make_Block(Object& obj, float size);

void Draw_Coordinate(Coordinate obj);

//ī�޶�
void init_camera();
void Update_camera(float angle_xz, float angle_y);

//���� ����
void checkFrameBuffer();
GLvoid InitBuffer();

void is_walkable();
bool is_crash(Object& objA, Object& objB);

void AddColors(Object& fig, float r, float g, float b);
void AddColors_Indexlist(Object& fig, float r, float g, float b);

GLvoid Keyboard(unsigned char key, int x, int y);
void Timer(int value); // Ÿ�̸� �ݹ� �Լ�
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void passiveMotion(int x, int y);
void SpecialKeyboard(int key, int x, int y);
GLvoid KeyUp(unsigned char key, int x, int y);

GLint CreateShader(const char* file, int type);
GLvoid CreateShaderProgram();
GLvoid Render(GLvoid);
GLvoid Reshape(int w, int h);
void mainLoop();

void read_obj_file(const char* filename, Object& model);