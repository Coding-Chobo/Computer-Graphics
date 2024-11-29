#pragma once
//���̺귯�� ȣ��
#include <iostream>
#include <fstream>
#include <sstream>
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
struct XYZ
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
};
struct FreeCamera
{
    GLfloat x, y, z;
    GLfloat at_x, at_y, at_z;
    glm::vec3 forward{}, up{}, right{};
};
class FixedCamera
{    
public:
    GLfloat *x, *y, *z;
    glm::vec3 forward{}, up{}, right{};
};
struct Light
{
    XYZ position{};
};
struct Index {
    unsigned int v1;  // ���� �ε���
    unsigned int v2; // �ؽ�ó �ε���
    unsigned int v3;  // ���� �ε���
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
    vector<glm::vec3> vertex;       // ���� ����Ʈ
    vector<glm::vec3> color;        // ���� ����Ʈ
    vector<glm::vec3> normal;       // ���� ����Ʈ
    vector<glm::vec2> texture;     // �ؽ�ó ��ǥ ����Ʈ
    vector<Index> indexlist;        // ����/�ؽ�ó/���� �ε��� ����Ʈ

    XYZ transform{};
    XYZ scaling{};
    XYZ rotation{};
    XYZ *parent_transform{};
    vector<XYZ> last_transform{};
    vector<XYZ> last_rotation{};
    XYZ tps_position{};
    BoundingBox hitbox{};
    float power{};
    float sensitivity{};
    glm::vec3 direction{};
    bool is_jump{};
    bool is_Land{};
    int flight_time{};
    float orbit_theta{};
    float dir_x{};
    float dir_z{};
    int facecnt{};
    void UpdateVBO();
    void Make_Matrix();
    void Make_Matrix_parent();
    void Draw_object();
    void Draw_object_parent();
    void Draw_object_element();
    void Draw_object_parent_element();
    void UpdateBoundingBox();
    Object();

};
Object::Object() {
    transform = { 0.0f,0.0f,0.0f };
    rotation = { 0.0f,0.0f,0.0f };
    scaling = { 1.0f,1.0f ,1.0f };
    sensitivity = 4.0;
}

struct Coordinate {
    vector<glm::vec3>vertex;
    vector<glm::vec3>color;
    vector<unsigned int>indexlist;
    float radius{};
};
//----------------------------------------------------------
void read_newline(char* str) {
    char* pos;
    if ((pos = strchr(str, '\n')) != NULL)
        *pos = '\0';
}

void init_figure();
//ť�� �����׸���
void Make_Cube(float size, Object& obj);
void Make_Tetra(float size, Object& obj);
void Make_Board(Object& obj, float size);
//ī�޶�
void init_camera();
void Update_camera(float angle_xz, float angle_y);
void Update_camera(float angle_xy);

//���� ����
void checkFrameBuffer();
GLvoid InitBuffer();

//25��
GLvoid UpdateVBO(Coordinate object);
void Orbit_Path(Coordinate path, Object planet);
GLvoid init_Matrix();
void Draw_Coordinate(Coordinate obj);



void Rotate_vertex(float angle);

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