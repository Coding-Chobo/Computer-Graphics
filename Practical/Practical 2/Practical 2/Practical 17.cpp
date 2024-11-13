#define _CRT_SECURE_NO_WARNINGS
#include "3DGraphics.h"

#define WIDTH 1200
#define HEIGHT 900
#define PI 3.1415926535897932384620588419716939
#define MAX_LINE_LENGTH 1024
GLchar* vertexSource, * fragmentSource; // 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //세이더 객체
GLuint shaderProgramID; // 세이더 프로그램

//그려질 오브젝트 선언
Object cube_top{}, cube_bottom{}, cube_right{}, cube_left{}, cube_front{}, cube_back{};
Object quad_pyra_bottom{}, quad_pyra_right{}, quad_pyra_left{}, quad_pyra_front{}, quad_pyra_back{};
Object objfile{};
Coordinate Coordinate_system{};
Camera_Info camera{};

GLuint vao;
GLuint vbo[2];
GLuint EBO;

//실습에 필요한 변수들
unsigned int shape{ 0 };
unsigned int which_face{ 0 };
float theta = 0.0f;
float size = 0.2f;
float far_ = 50.0f;
float spiral_size{};
float radius = 0.0f;
glm::vec3 goal[4];
glm::vec3 goal2[4];
GLfloat orbit_angle_y{ 0.0f };  // 공전 각도
bool mode_swab{ false };
bool mode_spin{ false };
bool mode_topspin{ false };
bool mode_animate[6]{ 0 };
bool is_open{ false };
bool is_open2{ false };
int dir_y{};
int dir_s{};
//-----------------------------------------------------------------------
void main(int argc, char** argv)
{
	srand(time(NULL));
	// 윈도우 생성
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("OpenGL Practical");

	// GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();
	// 은면제거
	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	InitBuffer();
	CreateShaderProgram();
	//도형 정보 초기화
	
	init_figure(size);

	//리콜 함수
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeyboard);
	glutDisplayFunc(Render);
	glutIdleFunc(mainLoop); // 메인 루프
	glutMainLoop(); //이벤트 루프 진입
}
//그리는 함수
GLvoid Render() {
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	// 좌표계 그리기
	init_Matrix();
	Draw_Coordinate(Coordinate_system);

	//큐브 그리기
	if (shape == 0)
	{
		Make_Matrix(orbit_angle_y, cube_top);
		UpdateVBO(cube_top);
		glDrawElements(GL_TRIANGLES, 3 * cube_top.indexlist.size(), GL_UNSIGNED_INT, 0);
		Make_Matrix(orbit_angle_y, cube_bottom);
		UpdateVBO(cube_bottom);
		glDrawElements(GL_TRIANGLES, 3 * cube_bottom.indexlist.size(), GL_UNSIGNED_INT, 0);
		Make_Matrix(orbit_angle_y, cube_right);
		UpdateVBO(cube_right);
		glDrawElements(GL_TRIANGLES, 3 * cube_right.indexlist.size(), GL_UNSIGNED_INT, 0);
		Make_Matrix(orbit_angle_y, cube_left);
		UpdateVBO(cube_left);
		glDrawElements(GL_TRIANGLES, 3 * cube_left.indexlist.size(), GL_UNSIGNED_INT, 0);
		Make_Matrix(orbit_angle_y, cube_front);
		UpdateVBO(cube_front);
		glDrawElements(GL_TRIANGLES, 3 * cube_front.indexlist.size(), GL_UNSIGNED_INT, 0);
		Make_Matrix(orbit_angle_y, cube_back);
		UpdateVBO(cube_back);
		glDrawElements(GL_TRIANGLES, 3 * cube_back.indexlist.size(), GL_UNSIGNED_INT, 0);
	}
	else if (shape == 1)
	{
		Make_Matrix(orbit_angle_y, quad_pyra_bottom);
		UpdateVBO(quad_pyra_bottom);
		glDrawElements(GL_TRIANGLES, 3 * quad_pyra_bottom.indexlist.size(), GL_UNSIGNED_INT, 0);
		Make_Matrix(orbit_angle_y, quad_pyra_right);
		UpdateVBO(quad_pyra_right);
		glDrawElements(GL_TRIANGLES, 3 * quad_pyra_right.indexlist.size(), GL_UNSIGNED_INT, 0);
		Make_Matrix(orbit_angle_y, quad_pyra_left);
		UpdateVBO(quad_pyra_left);
		glDrawElements(GL_TRIANGLES, 3 * quad_pyra_left.indexlist.size(), GL_UNSIGNED_INT, 0);
		Make_Matrix(orbit_angle_y, quad_pyra_front);
		UpdateVBO(quad_pyra_front);
		glDrawElements(GL_TRIANGLES, 3 * quad_pyra_front.indexlist.size(), GL_UNSIGNED_INT, 0);
		Make_Matrix(orbit_angle_y, quad_pyra_back);
		UpdateVBO(quad_pyra_back);
		glDrawElements(GL_TRIANGLES, 3 * quad_pyra_back.indexlist.size(), GL_UNSIGNED_INT, 0);
	}
	
	//obj파일 그리기
	Make_Matrix(orbit_angle_y, objfile);
	UpdateVBO(objfile);
	glDrawElements(GL_TRIANGLES, 3 * objfile.indexlist.size(), GL_UNSIGNED_INT, 0);



	glutSwapBuffers();
}

GLvoid InitBuffer()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(2, vbo);
	glGenBuffers(1, &EBO);
}

GLvoid init_Matrix() {
	// 모델 location 인덱스 저장
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "trans");
	// 변환 관련 변수를 정의합니다.
	glm::mat4 modelMatrix = glm::mat4(1.0f); // 모델 변환
	glm::mat4 viewMatrix = glm::mat4(1.0f);  // 뷰 변환 (카메라 변환)
	glm::mat4 projectionMatrix = glm::mat4(1.0f); // 투영 변환 (프로젝션 변환)

	// 모델 변환
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	// 카메라 설정: 카메라 위치, 카메라가 바라보는 위치, 월드 업 벡터
	viewMatrix = glm::lookAt(
		glm::vec3(camera.x, camera.y, camera.z), // 카메라 위치
		glm::vec3(camera.at_x, camera.at_y, camera.at_z), // 카메라가 바라보는 지점
		glm::vec3(0.0f, 1.0f, 0.0f)  // 월드 업 벡터
	);

	// 투영 설정: FOV, 종횡비, near, far 클립 거리
	projectionMatrix = glm::perspective(
		glm::radians(45.0f), // 시야각
		(float)WIDTH / (float)HEIGHT, // 종횡비
		0.1f, 50.0f // 클립 평면 (near, far)
	);


	glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix; // MVP 변환


	// 모델 변환 적용
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

}

void Object::Make_Matrix() {
	// 변환 관련 변수를 정의합니다.
	this->Matrix = glm::translate(this->Matrix, glm::vec3(this->transform.x, this->transform.y, this->transform.z));  // 공전 거리 적용
	if (this->transform.y != cube_top.transform.y && shape == 0)
	{
		this->Matrix = glm::translate(this->Matrix, glm::vec3(0.0f, -0.2f, 0.0f));
	}

	// 제자리 회전: 공전된 위치에서 객체를 제자리에서 회전시킵니다.
	this->Matrix = glm::rotate(this->Matrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));  // X축 회전
	this->Matrix = glm::rotate(this->Matrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));  // Y축 회전
	this->Matrix = glm::rotate(this->Matrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));  // Z축 회전
	if (this->transform.y != cube_top.transform.y && shape == 0)
	{
		this->Matrix = glm::translate(this->Matrix, glm::vec3(0.0f, 0.2f, 0.0f));
	}
	// 크기 변환
	Matrix = glm::scale(this->Matrix, glm::vec3(scaling.x, scaling.y, scaling.z));
}

GLvoid Make_Matrix(float& orbit_angle, Object obj) {

	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "trans");
	// 변환 관련 변수를 정의합니다.
	glm::mat4 modelMatrix = glm::mat4(1.0f);  // 모델 변환
	glm::mat4 viewMatrix = glm::mat4(1.0f);   // 뷰 변환 (카메라 변환)
	glm::mat4 projectionMatrix = glm::mat4(1.0f); // 투영 변환 (프로젝션 변환)
	obj.Make_Matrix();


	modelMatrix = glm::rotate(modelMatrix, glm::radians(30.f), glm::vec3(1.0f, 0.0f, 0.0f));  // X축 회전
	modelMatrix = glm::rotate(modelMatrix, glm::radians(30.f), glm::vec3(0.0f, 1.0f, 0.0f));  // Y축 회전
	
	// 공전 변환: 객체가 원점을 기준으로 회전하게 합니다.
	modelMatrix = glm::rotate(modelMatrix, glm::radians(orbit_angle), glm::vec3(0.0f, 1.0f, 0.0f));  // Y축 공전
	modelMatrix = modelMatrix * obj.Matrix;


	// 카메라 설정
	viewMatrix = glm::lookAt(
		glm::vec3(camera.x, camera.y, camera.z), // 카메라 위치
		glm::vec3(camera.at_x, camera.at_y, camera.at_z), // 카메라가 바라보는 지점
		glm::vec3(0.0f, 1.0f, 0.0f)  // 월드 업 벡터
	);

	// 투영 설정
	projectionMatrix = glm::perspective(
		glm::radians(90.0f), // 시야각
		(float)WIDTH / (float)HEIGHT, // 종횡비
		0.1f, far_ // 클립 평면 (near, far)
	);

	// 최종 변환 행렬(MVP)을 계산하여 GPU로 전달
	glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
}

void Timer(int value) {

}
void Mouse(int button, int state, int x, int y) {

}
void Motion(int x, int y) {

}
void SpecialKeyboard(int key, int x, int y)
{
	switch (key)
	{

	default:
		break;
	}
}
GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key)
	{
		
	case 't':
		if (shape == 0)
		{
			mode_animate[0] = !mode_animate[0];
		}
		break;
	case 'f':
		if (shape == 0)
		{
			mode_animate[1] = !mode_animate[1];
		}
		break;
	case 's':
		if (shape == 0)
		{
			mode_animate[2] = !mode_animate[2];
		}
		break;
	case 'b':
		if (shape == 0)
		{
			mode_animate[3] = !mode_animate[3];
		}
		break;
		
		
	case 'o':
		if (shape == 1) {
			mode_animate[4] = true;
			mode_animate[5] = false;
			is_open = !is_open;
		}
		break;
	case 'R':
		if (shape == 1) {
			mode_animate[4] = false;
			mode_animate[5] = true;
			is_open2 = !is_open2;
		}
		break;
	
	case 'h':
		glEnable(GL_CULL_FACE);
		break;
	case 'H':
		glDisable(GL_CULL_FACE);
	case 'c':
		mode_swab = !mode_swab;
		break;
	case 'y':
		mode_spin = !mode_spin;
		break;
	case 'p':
		if (far_ >20)
		{
			far_ -= 5.0f;
		}
		break;
	case '1':
		shape = 1;
		break;
	case '0':
		shape = 0;
		break;
	default:
		break;
	}
}

void mainLoop() {
	float rotate_gap = 0.10f;
	float moving_gap = 0.001f;
	if (mode_spin)
	{
		orbit_angle_y += rotate_gap;
	}
	if (mode_animate[0])
	{
		cube_top.rotation.x += rotate_gap;
	}
	if (mode_animate[1]) // 앞
	{
		if (cube_front.rotation.x < 90.0f)
		{
			cube_front.rotation.x += rotate_gap;
		}
	}
	else
	{
		if (cube_front.rotation.x > 0.0f)
		{
			cube_front.rotation.x -= rotate_gap;
		}
		else
		{
			cube_front.rotation.x = 0.0f;
		}
	}
	if (mode_animate[2])// 옆
	{
		if (cube_right.transform.y < 0.5f)
		{
			cube_right.transform.y += moving_gap;
			cube_left.transform.y += moving_gap;
		}
	}
	else
	{
		if (cube_right.transform.y > 0.0f)
		{
			cube_right.transform.y -= moving_gap;
			cube_left.transform.y -= moving_gap;
		}
		else
		{
			cube_left.transform.y = 0;
			cube_right.transform.y = 0;
		}
	}
	if (mode_animate[3]) // 뒤
	{
		if (cube_back.scaling.x > 0)
		{
			cube_back.scaling.x -= moving_gap;
			cube_back.scaling.y -= moving_gap;
			cube_back.scaling.z -= moving_gap;
		}
	}
	else
	{
		if (cube_back.scaling.x < 1.0f)
		{
			cube_back.scaling.x += moving_gap;
			cube_back.scaling.y += moving_gap;
			cube_back.scaling.z += moving_gap;
		}
	}

	if (mode_animate[4])
	{
		if (quad_pyra_front.rotation.x < 270.0f&&is_open)
		{
			std::cout << "open" << std::endl;
			quad_pyra_back.rotation.x -= rotate_gap;
			quad_pyra_front.rotation.x += rotate_gap;
			quad_pyra_left.rotation.z += rotate_gap;
			quad_pyra_right.rotation.z -= rotate_gap;
		}
		else if (!is_open)
		{
			if (quad_pyra_front.rotation.x > -30.0f)
			{
				std::cout << "close" << std::endl;
				quad_pyra_back.rotation.x += rotate_gap;
				quad_pyra_front.rotation.x -= rotate_gap;
				quad_pyra_left.rotation.z -= rotate_gap;
				quad_pyra_right.rotation.z += rotate_gap;
			}
		}
	}
	if (mode_animate[5])
	{
		if (is_open2)
		{
			switch (which_face)
			{
			case 0: // front
				if (quad_pyra_front.rotation.x < 90.0f)
				{
					quad_pyra_front.rotation.x += rotate_gap;
				}
				else
				{
					which_face++;
				}
				break;
			case 1://left
				if (quad_pyra_left.rotation.z < 90.0f)
				{
					quad_pyra_left.rotation.z += rotate_gap;

				}
				else
				{
					which_face++;
				}
				break;
			case 2://back
				if (quad_pyra_back.rotation.x > -90.0f)
				{
					quad_pyra_back.rotation.x -= rotate_gap;
				}
				else
				{
					which_face++;
				}
				break;
			case 3://right
				if (quad_pyra_right.rotation.z > -90.0f)
				{
					quad_pyra_right.rotation.z -= rotate_gap;
				}
				break;
			default:
				break;
			}
		}
		else 
		{
			switch (which_face)
			{
			case 0: // front
				if (quad_pyra_front.rotation.x > -30.0f)
				{
					quad_pyra_front.rotation.x -= rotate_gap;
				}
				break;
			case 1://left
				if (quad_pyra_left.rotation.z > -30.0f)
				{
					quad_pyra_left.rotation.z -= rotate_gap;

				}
				else
				{
					which_face--;
				}
				break;
			case 2://back
				if (quad_pyra_back.rotation.x < 30.0f)
				{
					quad_pyra_back.rotation.x += rotate_gap;
				}
				else
				{
					which_face--;
				}
				break;
			case 3://right
				if (quad_pyra_right.rotation.z < 30.0f)
				{
					quad_pyra_right.rotation.z += rotate_gap;
				}
				else
				{
					which_face--;
				}
				break;
			default:
				break;
			}
		}
	}
	glutPostRedisplay(); // 다시 그리기 요청
}

void init_figure(float size) {
	//---------------------------obj파일-----------------------------
	objfile.vertex.clear();
	objfile.color.clear();
	objfile.indexlist.clear();
	read_obj_file("cat_ldc.obj", objfile);
	//-----------------월드 변환 좌표 입력--------------------------
	camera.x = 0.0f, camera.y = 0.0f, camera.z = 2.0f;
	camera.at_x = 0.0f, camera.at_y = 0.0f, camera.at_z = 0.0f;
	//---------------------------cube-----------------------------
	Make_cube_top(cube_top, size);
	Make_cube_bottom(cube_bottom, size);
	Make_cube_Left(cube_left, size);
	Make_cube_Right(cube_right, size);
	Make_cube_front(cube_front, size);
	Make_cube_back(cube_back, size);
	//-------------------------Quad_Pyra---------------------------
	Make_quad_pyra_bottom(quad_pyra_bottom, size);
	Make_quad_pyra_Left(quad_pyra_left,size);
	Make_quad_pyra_Right(quad_pyra_right,  size);
	Make_quad_pyra_front(quad_pyra_front,  size);
	Make_quad_pyra_back(quad_pyra_back,  size);
}

void Draw_Coordinate(Coordinate obj) {
	obj.vertex.clear();
	obj.color.clear();
	obj.indexlist.clear();

	// 정점 추가
	obj.vertex.emplace_back(glm::vec3{ -5.0f, 0.0f, 0.0f });  // X축
	obj.vertex.emplace_back(glm::vec3{ 5.0f, 0.0f, 0.0f });  // X축
	obj.vertex.emplace_back(glm::vec3{ 0.0f, -5.0f, 0.0f });  // Y축
	obj.vertex.emplace_back(glm::vec3{ 0.0f, 5.0f, 0.0f });  // Y축
	obj.vertex.emplace_back(glm::vec3{ 0.0f, 0.0f, -5.0f });  // Z축
	obj.vertex.emplace_back(glm::vec3{ 0.0f, 0.0f, 5.0f });  // Z축

	// 색상 추가 (각 축의 색상 설정)
	obj.color.emplace_back(glm::vec3{ 0.0f, 0.0f, 0.0f });  // X축: 빨강
	obj.color.emplace_back(glm::vec3{ 1.0f, 0.0f, 0.0f });  // X축: 빨강
	obj.color.emplace_back(glm::vec3{ 0.0f, 0.0f, 0.0f });  // Y축: 파랑
	obj.color.emplace_back(glm::vec3{ 0.0f, 0.0f, 1.0f });  // Y축: 파랑
	obj.color.emplace_back(glm::vec3{ 0.0f, 0.0f, 0.0f });  // Z축: 초록
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 0.0f });  // Z축: 초록


	// 인덱스 추가
	obj.indexlist.emplace_back(0);  // X축
	obj.indexlist.emplace_back(1);

	obj.indexlist.emplace_back(2);  // Y축
	obj.indexlist.emplace_back(3);

	obj.indexlist.emplace_back(4);  // Z축
	obj.indexlist.emplace_back(5);

	// VBO 업데이트
	UpdateVBO(obj);

	// 인덱스의 개수는 좌표계의 라인이 3개이므로 총 6개
	glDrawElements(GL_LINES, obj.indexlist.size(), GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}
//큐브 나눠 만들기
void Make_cube_top(Object& obj, float size) {
	obj.vertex.emplace_back(glm::vec3{ size,0.0f ,size });
	obj.vertex.emplace_back(glm::vec3{ size,0.0f ,-size });
	obj.vertex.emplace_back(glm::vec3{ -size,0.0f ,-size });
	obj.vertex.emplace_back(glm::vec3{ -size,0.0f ,size });
	for (unsigned int i = 1; i < obj.vertex.size()-1; i++)
	{
		obj.indexlist.emplace_back(Index{ 0,i,i + 1 });
	}
	obj.color.emplace_back(glm::vec3{ 1.0f,0.0f ,0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f,1.0f ,0.0f });
	obj.color.emplace_back(glm::vec3{ 1.0f,0.0f ,0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f,0.0f ,1.0f });
	obj.transform = { 0.0f ,size ,0.0f };
	obj.scaling = { 1.0f ,1.0f ,1.0f };
	obj.rotation = { 0.0f ,0.0f ,0.0f };
}
void Make_cube_bottom(Object& obj, float size) {
	obj.vertex.emplace_back(glm::vec3{ size,0.0f ,size });
	obj.vertex.emplace_back(glm::vec3{ -size,0.0f ,size });
	obj.vertex.emplace_back(glm::vec3{ -size,0.0f ,-size });
	obj.vertex.emplace_back(glm::vec3{ size,0.0f ,-size });
	for (unsigned int i = 1; i < obj.vertex.size() - 1; i++)
	{
		obj.indexlist.emplace_back(Index{ 0,i,i + 1 });
	}
	obj.color.emplace_back(glm::vec3{ 1.0f,0.0f ,0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f,1.0f ,0.0f });
	obj.color.emplace_back(glm::vec3{ 1.0f,0.0f ,0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f,0.0f ,1.0f });
	obj.transform = { 0.0f ,-size ,0.0f };
	obj.scaling = { 1.0f ,1.0f ,1.0f };
	obj.rotation = { 0.0f ,0.0f ,0.0f };
}
void Make_cube_Left(Object& obj, float size) {
	obj.vertex.emplace_back(glm::vec3{ 0.0f,size,size });
	obj.vertex.emplace_back(glm::vec3{ 0.0f,size ,-size });
	obj.vertex.emplace_back(glm::vec3{ 0.0f,-size ,-size });
	obj.vertex.emplace_back(glm::vec3{ 0.0f,-size ,size });
	for (unsigned int i = 1; i < obj.vertex.size() - 1; i++)
	{
		obj.indexlist.emplace_back(Index{ 0,i,i + 1 });
	}
	obj.color.emplace_back(glm::vec3{ 1.0f,0.0f ,0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f,1.0f ,0.0f });
	obj.color.emplace_back(glm::vec3{ 1.0f,0.0f ,0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f,0.0f ,1.0f });
	obj.transform = { -size ,0.0f ,0.0f };
	obj.scaling = { 1.0f ,1.0f ,1.0f };
	obj.rotation = { 0.0f ,0.0f ,0.0f };
}
void Make_cube_Right(Object& obj, float size) {
	obj.vertex.emplace_back(glm::vec3{ 0.0f,size,size });
	obj.vertex.emplace_back(glm::vec3{ 0.0f,-size ,size });
	obj.vertex.emplace_back(glm::vec3{ 0.0f,-size ,-size });
	obj.vertex.emplace_back(glm::vec3{ 0.0f,size ,-size });
	for (unsigned int i = 1; i < obj.vertex.size() - 1; i++)
	{
		obj.indexlist.emplace_back(Index{ 0,i,i + 1 });
	}
	obj.color.emplace_back(glm::vec3{ 1.0f,0.0f ,0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f,1.0f ,0.0f });
	obj.color.emplace_back(glm::vec3{ 1.0f,0.0f ,0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f,0.0f ,1.0f });
	obj.transform = { size ,0.0f ,0.0f };
	obj.scaling = { 1.0f ,1.0f ,1.0f };
	obj.rotation = { 0.0f ,0.0f ,0.0f };
}
void Make_cube_front(Object& obj, float size) {
	obj.vertex.emplace_back(glm::vec3{ size,size,0.0f });
	obj.vertex.emplace_back(glm::vec3{ -size,size ,0.0f });
	obj.vertex.emplace_back(glm::vec3{ -size,-size ,0.0f });
	obj.vertex.emplace_back(glm::vec3{ size,-size ,0.0f });
	for (unsigned int i = 1; i < obj.vertex.size() - 1; i++)
	{
		obj.indexlist.emplace_back(Index{ 0,i,i + 1 });
	}
	obj.color.emplace_back(glm::vec3{ 1.0f,0.0f ,0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f,1.0f ,0.0f });
	obj.color.emplace_back(glm::vec3{ 1.0f,0.0f ,0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f,0.0f ,1.0f });
	obj.transform = { 0.0f ,0.0f ,size };
	obj.scaling = { 1.0f ,1.0f ,1.0f };
	obj.rotation = { 0.0f ,0.0f ,0.0f };
}
void Make_cube_back(Object& obj, float size) {
	obj.vertex.emplace_back(glm::vec3{ size,size,0.0f });
	obj.vertex.emplace_back(glm::vec3{ size,-size ,0.0f });
	obj.vertex.emplace_back(glm::vec3{ -size,-size ,0.0f });
	obj.vertex.emplace_back(glm::vec3{ -size,size ,0.0f });
	for (unsigned int i = 1; i < obj.vertex.size() - 1; i++)
	{
		obj.indexlist.emplace_back(Index{ 0,i,i + 1 });
	}
	obj.color.emplace_back(glm::vec3{ 1.0f,0.0f ,0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f,1.0f ,0.0f });
	obj.color.emplace_back(glm::vec3{ 1.0f,0.0f ,0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f,0.0f ,1.0f });
	obj.transform = { 0.0f ,0.0f ,-size };
	obj.scaling = { 1.0f ,1.0f ,1.0f };
	obj.rotation = { 0.0f ,0.0f ,0.0f };
}
//사각뿔 나눠 만들기
void Make_quad_pyra_bottom(Object& obj, float size) {
	obj.vertex.emplace_back(glm::vec3{ size,0.0f,size });
	obj.vertex.emplace_back(glm::vec3{ -size,0.0f,size });
	obj.vertex.emplace_back(glm::vec3{ -size,0.0f,-size });
	obj.vertex.emplace_back(glm::vec3{ size,0.0f,-size });
	for (unsigned int i = 1; i < obj.vertex.size() - 1; i++)
	{
		obj.indexlist.emplace_back(Index{ 0,i,i + 1 });
	}
	obj.color.emplace_back(glm::vec3{ 1.0f,0.0f ,0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f,1.0f ,0.0f });
	obj.color.emplace_back(glm::vec3{ 1.0f,0.0f ,0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f,0.0f ,1.0f });
	obj.transform = { 0.0f ,0.0f ,0.0f };
	obj.scaling = { 1.0f ,1.0f ,1.0f };
	obj.rotation = { 0.0f ,0.0f ,0.0f };
}
void Make_quad_pyra_Left(Object& obj, float size) {
	obj.vertex.emplace_back(glm::vec3{ 0.0f,2 * size,0.0f });
	obj.vertex.emplace_back(glm::vec3{ 0.0f,0.0f,-size });
	obj.vertex.emplace_back(glm::vec3{ 0.0f,0.0f,size });
	for (unsigned int i = 1; i < obj.vertex.size() - 1; i++)
	{
		obj.indexlist.emplace_back(Index{ 0,i,i + 1 });
	}
	obj.color.emplace_back(glm::vec3{ 1.0f,0.0f ,0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f,1.0f ,0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f,0.0f ,1.0f });
	obj.transform = { -size, 0.0f ,0.0f  };
	obj.scaling = { 1.0f ,1.0f ,1.0f };
	obj.rotation = { 0.0f ,0.0f ,-30.0f };
}
void Make_quad_pyra_Right(Object& obj, float size) {
	obj.vertex.emplace_back(glm::vec3{ 0.0f,2 * size,0.0f });
	obj.vertex.emplace_back(glm::vec3{ 0.0f,0.0f,size });
	obj.vertex.emplace_back(glm::vec3{ 0.0f,0.0f,-size });
	for (unsigned int i = 1; i < obj.vertex.size() - 1; i++)
	{
		obj.indexlist.emplace_back(Index{ 0,i,i + 1 });
	}
	obj.color.emplace_back(glm::vec3{ 1.0f,0.0f ,0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f,1.0f ,0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f,0.0f ,1.0f });
	obj.transform = { size, 0.0f ,0.0f };
	obj.scaling = { 1.0f ,1.0f ,1.0f };
	obj.rotation = { 0.0f ,0.0f ,30.0f };
}
void Make_quad_pyra_front(Object& obj, float size) {
	obj.vertex.emplace_back(glm::vec3{ 0.0f,2 * size,0.0f });
	obj.vertex.emplace_back(glm::vec3{ size,0.0f,0.0f });
	obj.vertex.emplace_back(glm::vec3{ -size,0.0f,0.0f });
	for (unsigned int i = 1; i < obj.vertex.size() - 1; i++)
	{
		obj.indexlist.emplace_back(Index{ 0,i,i + 1 });
	}
	obj.color.emplace_back(glm::vec3{ 1.0f,0.0f ,0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f,1.0f ,0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f,0.0f ,1.0f });
	obj.transform = { 0.0f, 0.0f ,size };
	obj.scaling = { 1.0f ,1.0f ,1.0f };
	obj.rotation = { -30.0f ,0.0f ,0.0f };
}
void Make_quad_pyra_back(Object& obj, float size) {
	obj.vertex.emplace_back(glm::vec3{ 0.0f,2 * size,0.0f });
	obj.vertex.emplace_back(glm::vec3{ -size,0.0f,0.0f });
	obj.vertex.emplace_back(glm::vec3{ size,0.0f,0.0f });
	for (unsigned int i = 1; i < obj.vertex.size() - 1; i++)
	{
		obj.indexlist.emplace_back(Index{ 0,i,i + 1 });
	}
	obj.color.emplace_back(glm::vec3{ 1.0f,0.0f ,0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f,1.0f ,0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f,0.0f ,1.0f });
	obj.transform = { 0.0f ,0.0f, -size };
	obj.scaling = { 1.0f ,1.0f ,1.0f };
	obj.rotation = { 30.0f ,0.0f ,0.0f };
}

GLvoid UpdateVBO(Object object) {
	// VAO 바인드
	glBindVertexArray(vao);

	// 큐브 위치 버퍼 바인드
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, object.vertex.size() * sizeof(glm::vec3), object.vertex.data(), GL_STREAM_DRAW);

	// 큐브 인덱스 버퍼 바인드
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// 전체 인덱스 배열의 크기 전달
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, object.indexlist.size() * sizeof(Index), object.indexlist.data(), GL_DYNAMIC_DRAW);

	// 위치 속성 설정
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(0);

	// 큐브 색상 버퍼 바인드
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, object.color.size() * sizeof(glm::vec3), object.color.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(1);
}

GLvoid UpdateVBO(Coordinate object) {
	// VAO 바인드
	glBindVertexArray(vao);

	// 좌표계 정점 버퍼 바인드
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, object.vertex.size() * sizeof(glm::vec3), object.vertex.data(), GL_DYNAMIC_DRAW);

	// 좌표계 인덱스 버퍼 바인드
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, object.indexlist.size() * sizeof(unsigned int), object.indexlist.data(), GL_DYNAMIC_DRAW);

	// 위치 속성 설정
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(0);

	// 좌표계 색상 버퍼 바인드
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, object.color.size() * sizeof(glm::vec3), object.color.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(1);
}

GLvoid Reshape(int w, int h)
{
	// 뷰포트 기본 WIDTH HEIGHT로 설정
	glViewport(0, 0, w, h);
}

char* GetBuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;

	// 파일 읽기 형식으로 열기
	fopen_s(&fptr, file, "rb");

	// 예외처리
	if (!fptr) return NULL;

	// 파일 buf로 변환
	fseek(fptr, 0, SEEK_END);
	length = ftell(fptr);
	buf = (char*)malloc(length + 1);
	fseek(fptr, 0, SEEK_SET);
	fread(buf, length, 1, fptr);

	// 파일 종료
	fclose(fptr);

	buf[length] = 0;
	return buf;
}

GLint CreateShader(const char* file, int type)
{
	GLchar* source = GetBuf(file);

	// 객체 생성
	GLint shader = glCreateShader(type);
	glShaderSource(shader, 1, (const GLchar**)&source, 0);
	glCompileShader(shader);

	// 컴파일 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

	if (!result)
	{
		glGetShaderInfoLog(shader, 512, NULL, errorLog);
		std::cerr << "ERROR: 컴파일 실패\n" << errorLog << std::endl;
		return 0;
	}

	return shader;
}

void CreateShaderProgram()
{
	vertexShader = CreateShader("vertex.vs", GL_VERTEX_SHADER);
	fragmentShader = CreateShader("fragment.fs", GL_FRAGMENT_SHADER);

	// 세이더 프로그램 생성
	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glLinkProgram(shaderProgramID);

	// 세이더 삭제
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// 세이더 프로그램 사용
	glUseProgram(shaderProgramID);
}

void read_obj_file(const char* filename, Object& model) {
	FILE* file;
	fopen_s(&file, filename, "r");
	if (!file) {
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}

	char line[MAX_LINE_LENGTH];
	std::vector<glm::vec3> temp_vertices;
	std::vector<Index> temp_indices;

	while (fgets(line, sizeof(line), file)) {
		read_newline(line); // 개행문자 제거

		// 정점(Vertex) 데이터 처리
		if (line[0] == 'v' && line[1] == ' ') {
			glm::vec3 vertex;
			sscanf_s(line + 2, "%f %f %f", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}

		// 면(Face) 데이터 처리
		else if (line[0] == 'f' && line[1] == ' ') {
			unsigned int v1, v2, v3;
			unsigned int vt1, vt2, vt3; // 텍스처 좌표 인덱스
			unsigned int vn1, vn2, vn3; // 법선 벡터 인덱스
			sscanf_s(line + 2, "%u/%u/%u %u/%u/%u %u/%u/%u",
				&v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3);

			// OBJ 인덱스는 1부터 시작하기 때문에 0부터 시작하도록 조정
			Index face = { v1 - 1, v2 - 1, v3 - 1 };
			temp_indices.push_back(face);
		}
	}

	fclose(file);

	// Object 구조체에 정점 및 인덱스 데이터를 전달
	model.vertex = temp_vertices; // 정점 데이터를 그대로 저장
	model.indexlist = temp_indices; // 인덱스 데이터를 그대로 저장

	// 색상은 임의로 설정 (추후 수정 가능)
	for (size_t i = 0; i < model.vertex.size(); i++) {
		model.color.push_back(glm::vec3(1.0f, 1.0f, 1.0f)); // 빨강색으로 설정
	}
	model.transform = { 0.0f,0.0f,0.0f };
	model.scaling = { 1.0f,1.0f,1.0f };
	model.rotation = { 0.0f,0.0f,0.0f };
	UpdateVBO(model);
}