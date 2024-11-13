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
    //GLfloat* x, * y, * z;
    GLfloat at_x, at_y, at_z;

    glm::vec3 u{}, v{}, n{};
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
    BoundingBox hitbox{};
    Camera_Info camera{};
    float speed{};
    float dir_x{};
    float dir_z{};
    bool is_jump{};
    int flight_time{};
    glm::mat4 Matrix;
    void UpdateVBO();
    //void Update_Camera();
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
    //camera.x, camera.y, camera.z = &transform.x, & transform.y, & transform.z;
    //camera.u = { 0.0, 0.0, 1.0f };
    //camera.v = { 0.0, 1.0f, 0.0 };
    //camera.n = { 1.0f, 0.0, 0.0 };

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

// 실습 17번
//큐브 나눠그리기
void Make_cube_top(Object& obj, float size);
void Make_cube_bottom(Object& obj, float size);
void Make_cube_Left(Object& obj, float size);
void Make_cube_Right(Object& obj, float size);
void Make_cube_front(Object& obj, float size);
void Make_cube_back(Object& obj, float size);
void Make_cube_front_left(Object& obj, float size);
void Make_cube_front_right(Object& obj, float size);

//실습 20번
void init_camera();
void Make_Block(Object& obj, float size);

//
void checkFrameBuffer();

void is_walkable();
void Draw_Coordinate(Coordinate obj);
GLvoid InitBuffer();
GLvoid Satellite_Matrix(float& orbit_angle, Object obj, Object planet);

void AddColors(Object& fig, float r, float g, float b);
void AddColors_Indexlist(Object& fig, float r, float g, float b);

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