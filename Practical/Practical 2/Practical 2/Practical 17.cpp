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
Object cube{}, tetra{}, corn{}, quad_pyra{}, temp{};
Object objfile{};
Coordinate Coordinate_system{}, spiral{};
Camera_Info camera{};

GLuint vao;
GLuint vbo[2];
GLuint EBO;

//실습에 필요한 변수들
unsigned int shape{ 0 };
float theta = 0.0f;
float theta_2{};
float theta_3{};
float theta_5{};
float spiral_size{};
float radius = 0.0f;
glm::vec3 goal[4];
glm::vec3 goal2[4];
GLfloat orbit_angle_y{ 0.0f };  // 공전 각도
bool mode_swab{ false };
bool mode_spin{ false };
unsigned int mode_animate{ 0 };
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
	init_figure();

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
	//
	if (mode_animate == 1)
	{
		Make_Matrix(orbit_angle_y, cube);
		UpdateVBO(cube);
		glDrawElements(GL_TRIANGLES, 3 * cube.indexlist.size(), GL_UNSIGNED_INT, 0);

		UpdateVBO(temp);
		glDrawElements(GL_TRIANGLES, 3 * temp.indexlist.size(), GL_UNSIGNED_INT, 0);

	}
	else if (mode_animate == 2)
	{
		Make_Matrix(orbit_angle_y, cube);
		UpdateVBO(cube);
		glDrawElements(GL_TRIANGLES, 3 * cube.indexlist.size(), GL_UNSIGNED_INT, 0);
	}
	else if (mode_animate == 3)
	{
		Make_Matrix(orbit_angle_y, cube);
		UpdateVBO(cube);
		glDrawElements(GL_TRIANGLES, 3 * cube.indexlist.size(), GL_UNSIGNED_INT, 0);

	}
	else if (mode_animate == 4)
	{
		Make_Matrix(orbit_angle_y, cube);
		UpdateVBO(cube);
		glDrawElements(GL_TRIANGLES, 3 * cube.indexlist.size(), GL_UNSIGNED_INT, 0);
	}
	else if (mode_animate == 5)
	{

	}
	else if (mode_animate == 5)
	{

	}
	if (mode_swab)
	{
		Make_Matrix(orbit_angle_y, quad_pyra);
		UpdateVBO(quad_pyra);
		glDrawElements(GL_TRIANGLES, 3 * quad_pyra.indexlist.size(), GL_UNSIGNED_INT, 0);

	}
	else
	{

	}


		//Make_Matrix(orbit_angle_y, corn);
		//UpdateVBO(corn);
		//glDrawElements(GL_TRIANGLES, 3 * corn.indexlist.size(), GL_UNSIGNED_INT, 0);
		//Make_Matrix(orbit_angle_y, objfile);
		//UpdateVBO(objfile);
		//glDrawElements(GL_TRIANGLES, 3 * objfile.indexlist.size(), GL_UNSIGNED_INT, 0);

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

GLvoid Make_Matrix(float& orbit_angle, Object obj) {

	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "trans");
	// 변환 관련 변수를 정의합니다.
	glm::mat4 modelMatrix = glm::mat4(1.0f);  // 모델 변환
	glm::mat4 viewMatrix = glm::mat4(1.0f);   // 뷰 변환 (카메라 변환)
	glm::mat4 projectionMatrix = glm::mat4(1.0f); // 투영 변환 (프로젝션 변환)

	modelMatrix = glm::rotate(modelMatrix, glm::radians(30.f), glm::vec3(1.0f, 0.0f, 0.0f));  // X축 회전
	modelMatrix = glm::rotate(modelMatrix, glm::radians(30.f), glm::vec3(0.0f, 1.0f, 0.0f));  // Y축 회전

	// 공전 변환: 객체가 원점을 기준으로 회전하게 합니다.
	//modelMatrix = glm::rotate(modelMatrix, glm::radians(orbit_angle), glm::vec3(0.0f, 1.0f, 0.0f));  // Y축 공전
	modelMatrix = glm::translate(modelMatrix, glm::vec3(obj.transform.x, obj.transform.y, obj.transform.z));  // 공전 거리 적용
	if (mode_spin)
	{
		modelMatrix = glm::rotate(modelMatrix, glm::radians(orbit_angle), glm::vec3(0.0f, 1.0f, 0.0f));  // Y축 자전
	}


	// 제자리 회전: 공전된 위치에서 객체를 제자리에서 회전시킵니다.
	modelMatrix = glm::rotate(modelMatrix, glm::radians(obj.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));  // X축 회전
	modelMatrix = glm::rotate(modelMatrix, glm::radians(obj.rotation.x), glm::vec3(0.0f, 1.0f, 0.0f));  // Y축 회전
	modelMatrix = glm::rotate(modelMatrix, glm::radians(obj.rotation.x), glm::vec3(0.0f, 0.0f, 1.0f));  // Z축 회전

	// 크기 변환
	modelMatrix = glm::scale(modelMatrix, glm::vec3(obj.scaling.x, obj.scaling.y, obj.scaling.z));


	// 카메라 설정
	viewMatrix = glm::lookAt(
		glm::vec3(camera.x, camera.y, camera.z), // 카메라 위치
		glm::vec3(camera.at_x, camera.at_y, camera.at_z), // 카메라가 바라보는 지점
		glm::vec3(0.0f, 1.0f, 0.0f)  // 월드 업 벡터
	);

	// 투영 설정
	projectionMatrix = glm::perspective(
		glm::radians(45.0f), // 시야각
		(float)WIDTH / (float)HEIGHT, // 종횡비
		0.1f, 50.0f // 클립 평면 (near, far)
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
		if (mode_animate != 1)
		{
			for (int i = 0; i < 4; i++)
			{
				temp.vertex.emplace_back(cube.vertex.at(i).x, cube.vertex.at(i).y, cube.vertex.at(i).z);
				temp.color.emplace_back(cube.color.at(i).x, cube.color.at(i).y, cube.color.at(i).z);
			}
			temp.indexlist.emplace_back(Index{ cube.indexlist.at(0).v1, cube.indexlist.at(0).v2, cube.indexlist.at(0).v3 });
			temp.indexlist.emplace_back(Index{ cube.indexlist.at(1).v1, cube.indexlist.at(1).v2, cube.indexlist.at(1).v3 });
			mode_animate = 1;
			theta = 0.0f;
		}

		break;
	case '2':

		if (mode_animate != 2)
		{
			theta_2 = 0.0f;
			cube.transform = { 1.0f,1.0f,1.0f };
			objfile.transform = { 1.0f,1.0f,1.0f };
			corn.transform = { -1.0f,-1.0f,-1.0f };
			tetra.transform = { -1.0f,-1.0f,-1.0f };

			mode_animate = 2;
		}
		break;
	case '3':
		if (mode_animate != 3)
		{
			orbit_angle_y = 0.0f;
			cube.transform = { 1.0f,1.0f,1.0f };
			objfile.transform = { 1.0f,1.0f,1.0f };
			corn.transform = { -1.0f,-1.0f,-1.0f };
			tetra.transform = { -1.0f,-1.0f,-1.0f };
			mode_animate = 3;
		}
		break;
	case '4':
		if (mode_animate != 4)
		{
			theta_3 = 0.0f;
			cube.transform = { 1.0f,0.0f,1.0f };
			objfile.transform = { 1.0f,0.0f,1.0f };
			corn.transform = { -1.0f,0.0f,-1.0f };
			tetra.transform = { -1.0f,0.0f,-1.0f };

			mode_animate = 4;
		}
		break;
	case '5':
		if (mode_animate != 5)
		{
			theta_5 = 0.0f;
			orbit_angle_y = 0.0f;
			cube.transform = { 1.0f,0.0f,1.0f };
			objfile.transform = { 1.0f,0.0f,1.0f };
			corn.transform = { -1.0f,0.0f,-1.0f };
			tetra.transform = { -1.0f,0.0f,-1.0f };
			mode_animate = 5;
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
	case 's':

		break;
	default:
		break;
	}
}

void mainLoop() {
	float rotate_gap = PI / 20;
	orbit_angle_y += rotate_gap;
	switch (mode_animate)
	{
	case 1://육면체 윗면만 회전
		theta += 2;
		
		for (int i = 0; i < 4; i++)
		{
			temp.vertex[i].x = cube.vertex[i].x * cos(glm::radians(theta));
			temp.vertex[i].y = cube.vertex[i].y * sin(glm::radians(theta));
		}

		break;
	case 3://공전

		break;
	case 4://위아래 지그재그
		theta_3 += 2;

		break;
	case 5:


		cube.scaling = { sin(glm::radians(theta_3)) + 1.0f,sin(glm::radians(theta_3)) + 1.0f,sin(glm::radians(theta_3)) + 1.0f };
		objfile.scaling = { sin(glm::radians(theta_3)) + 1.0f,sin(glm::radians(theta_3)) + 1.0f,sin(glm::radians(theta_3)) + 1.0f };
		tetra.scaling = { -sin(glm::radians(theta_3)) + 1.0f ,-sin(glm::radians(theta_3)) + 1.0f,-sin(glm::radians(theta_3)) + 1.0f };
		corn.scaling = { -sin(glm::radians(theta_3)) + 1.0f ,-sin(glm::radians(theta_3)) + 1.0f,-sin(glm::radians(theta_3)) + 1.0f };

		break;
	default:
		break;
	}
	glutPostRedisplay(); // 다시 그리기 요청
}

void init_figure() {
	//---------------------------obj파일-----------------------------
	objfile.vertex.clear();
	objfile.color.clear();
	objfile.indexlist.clear();
	read_obj_file("2021184024.obj", objfile);
	//-----------------월드 변환 좌표 입력--------------------------
	camera.x = 0.0f, camera.y = 2.0f, camera.z = 6.0f;
	camera.at_x = 0.0f, camera.at_y = 0.0f, camera.at_z = 0.0f;

	Make_Corn(0.0f, 0.0f, 0.0f, 0.3f, corn);
	Make_Tetra(0.0f, 0.0f, 0.0f, 0.3f, tetra);
	Make_Cube(0.0f, 0.0f, 0.0f, 0.3f, cube);
	Make_QuadPyra(0.0f, 0.0f, 0.0f, 0.3f, quad_pyra);
	Make_Spiral(0.0f, 0.0f, 0.0f, spiral);
}

void Draw_Coordinate(Coordinate obj) {
	obj.vertex.clear();
	obj.color.clear();
	obj.indexlist.clear();

	// 정점 추가
	obj.vertex.emplace_back(glm::vec3{ 0.0f, 0.0f, 0.0f });  // 원점
	obj.vertex.emplace_back(glm::vec3{ 5.0f, 0.0f, 0.0f });  // X축
	obj.vertex.emplace_back(glm::vec3{ 0.0f, 0.0f, 0.0f });  // 원점
	obj.vertex.emplace_back(glm::vec3{ 0.0f, 5.0f, 0.0f });  // Y축
	obj.vertex.emplace_back(glm::vec3{ 0.0f, 0.0f, 0.0f });  // 원점
	obj.vertex.emplace_back(glm::vec3{ 0.0f, 0.0f, 5.0f });  // Z축

	// 색상 추가 (각 축의 색상 설정)
	obj.color.emplace_back(glm::vec3{ 1.0f, 0.0f, 0.0f });  // X축: 빨강
	obj.color.emplace_back(glm::vec3{ 1.0f, 0.0f, 0.0f });  // X축: 빨강
	obj.color.emplace_back(glm::vec3{ 0.0f, 0.0f, 1.0f });  // Y축: 파랑
	obj.color.emplace_back(glm::vec3{ 0.0f, 0.0f, 1.0f });  // Y축: 파랑
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 0.0f });  // Z축: 초록
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

void Make_Cube(float x, float y, float z, float size, Object& obj) {
	obj.vertex.clear();
	obj.color.clear();
	obj.indexlist.clear();
	obj.transform = { 0.0f,0.0f,0.0f };
	obj.scaling = { 1.0f,1.0f,1.0f };
	obj.rotation = { 0.0f,0.0f,0.0f };
	// 정점 추가
	obj.vertex.emplace_back(glm::vec3{ x - size, y + size, z - size });
	obj.vertex.emplace_back(glm::vec3{ x - size, y + size, z + size });
	obj.vertex.emplace_back(glm::vec3{ x + size, y + size, z + size });
	obj.vertex.emplace_back(glm::vec3{ x + size, y + size, z - size });

	obj.vertex.emplace_back(glm::vec3{ x - size, y - size, z - size });
	obj.vertex.emplace_back(glm::vec3{ x - size, y - size, z + size });
	obj.vertex.emplace_back(glm::vec3{ x + size, y - size, z + size });
	obj.vertex.emplace_back(glm::vec3{ x + size, y - size, z - size });

	// 색상 추가
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 1.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f, 0.0f, 1.0f });
	obj.color.emplace_back(glm::vec3{ 1.0f, 0.0f, 1.0f });

	obj.color.emplace_back(glm::vec3{ 1.0f, 0.0f, 0.0f });
	obj.color.emplace_back(glm::vec3{ 1.0f, 1.0f, 0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 1.0f });

	//인덱스리스트 추가
	obj.indexlist.emplace_back(Index{ 0, 1, 2 });
	obj.indexlist.emplace_back(Index{ 0, 2, 3 });
	obj.indexlist.emplace_back(Index{ 1, 5, 6 });
	obj.indexlist.emplace_back(Index{ 1, 6, 2 });
	obj.indexlist.emplace_back(Index{ 2, 6, 7 });
	obj.indexlist.emplace_back(Index{ 2, 7, 3 });
	obj.indexlist.emplace_back(Index{ 0, 4, 5 });
	obj.indexlist.emplace_back(Index{ 0, 5, 1 });
	obj.indexlist.emplace_back(Index{ 5, 4, 6 });
	obj.indexlist.emplace_back(Index{ 4, 7, 6 });
	obj.indexlist.emplace_back(Index{ 0, 7, 4 });
	obj.indexlist.emplace_back(Index{ 0, 3, 7 });
}

void Make_Tetra(float x, float y, float z, float size, Object& obj) {
	obj.transform = { 0.0f,0.0f,0.0f };
	obj.scaling = { 1.0f,1.0f,1.0f };
	obj.rotation = { 0.0f,0.0f,0.0f };
	float a = 0.85 * size;
	obj.vertex.emplace_back(glm::vec3{ x, y + size, z });
	obj.vertex.emplace_back(glm::vec3{ x, y - size / 2, z + a });
	obj.vertex.emplace_back(glm::vec3{ x + 0.85 * a, y - size / 2, z - a / 2 });
	obj.vertex.emplace_back(glm::vec3{ x - 0.85 * a, y - size / 2, z - a / 2 });

	// 색상 추가
	obj.color.emplace_back(glm::vec3{ 1.0f, 1.0f, 0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 1.0f });
	obj.color.emplace_back(glm::vec3{ 1.0f, 0.0f, 1.0f });
	obj.color.emplace_back(glm::vec3{ 0.5f, 0.5f, 0.5f });

	// 인덱스 리스트 추가
	obj.indexlist.emplace_back(Index{ 0, 1, 2 });
	obj.indexlist.emplace_back(Index{ 0, 2, 3 });
	obj.indexlist.emplace_back(Index{ 0, 3, 1 });
	obj.indexlist.emplace_back(Index{ 1, 2, 3 });
}

void Make_QuadPyra(float x, float y, float z, float size, Object& obj) {
	obj.transform = { 0.0f,0.0f,0.0f };
	obj.scaling = { 1.0f,1.0f,1.0f };
	obj.rotation = { 0.0f,0.0f,0.0f };
	
	obj.vertex.emplace_back(glm::vec3{ x, y + size, z });
	obj.vertex.emplace_back(glm::vec3{ x - size, y - size / 2, z + size });
	obj.vertex.emplace_back(glm::vec3{ x + size, y - size / 2, z + size });
	obj.vertex.emplace_back(glm::vec3{ x + size, y - size / 2, z - size });
	obj.vertex.emplace_back(glm::vec3{ x - size, y - size / 2, z - size });

	// 색상 추가
	obj.color.emplace_back(glm::vec3{ 1.0f, 1.0f, 0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 1.0f });
	obj.color.emplace_back(glm::vec3{ 1.0f, 0.0f, 1.0f });
	obj.color.emplace_back(glm::vec3{ 0.5f, 0.5f, 0.5f });

	// 인덱스 리스트 추가
	obj.indexlist.emplace_back(Index{ 0, 1, 2 });
	obj.indexlist.emplace_back(Index{ 0, 2, 3 });
	obj.indexlist.emplace_back(Index{ 0, 3, 4 });
	obj.indexlist.emplace_back(Index{ 0, 4, 1 });
	obj.indexlist.emplace_back(Index{ 3, 2, 1 });
	obj.indexlist.emplace_back(Index{ 1, 4, 3 });
}

void Make_Corn(float x, float y, float z, float size, Object& obj) {
	obj.vertex.clear();
	obj.color.clear();
	obj.indexlist.clear();
	//객체의 월드변환 매개변수값 초기화	
	obj.transform = { 0.0f,0.0f,0.0f };
	obj.scaling = { 1.0f,1.0f,1.0f };
	obj.rotation = { 0.0f,0.0f,0.0f };
	const int cnt = 20;
	float theta_corn[cnt];
	for (int i = 0; i < cnt; i++)
	{
		theta_corn[i] = i * (360 / cnt);
	}

	//정점추가
	for (int i = 0; i < cnt; i++)
	{
		obj.vertex.emplace_back(glm::vec3{ x + size * cos(glm::radians(theta_corn[i])), y, z + size * sin(glm::radians(theta_corn[i])) });
	}
	obj.vertex.emplace_back(glm::vec3{ x, y + 2 * size, z });
	obj.vertex.emplace_back(glm::vec3{ x, y, z });

	// 색상 추가
	for (int i = 0; i < obj.vertex.size(); i++)
	{
		float r = static_cast<float>(rand()) / RAND_MAX;
		float g = static_cast<float>(rand()) / RAND_MAX;
		float b = static_cast<float>(rand()) / RAND_MAX;
		obj.color.emplace_back(glm::vec3{ r,g,b });
	}
	// 인덱스 리스트 추가
	for (unsigned int i = 0; i < cnt; i++)
	{
		obj.indexlist.emplace_back(Index{ 20, i ,(i + 1) % cnt });
	}
	for (unsigned int i = 0; i < cnt; i++)
	{
		obj.indexlist.emplace_back(Index{ 21, i ,(i + 1) % cnt });
	}
}

void Make_Spiral(float x, float y, float z, Coordinate& obj) {
	obj.vertex.clear();
	obj.color.clear();
	obj.indexlist.clear();
	float size{};
	const int cnt = 36;
	float theta_sprial[cnt];
	for (int i = 0; i < cnt; i++)
	{
		theta_sprial[i] = i * (360 / cnt);
	}

	//정점추가
	for (int i = 0; i < 5 * cnt; i++)
	{
		size += 0.02f;
		obj.vertex.emplace_back(glm::vec3{ x + size * cos(glm::radians(theta_sprial[i % cnt])), y, z + size * sin(glm::radians(theta_sprial[i % cnt])) });
	}

	// 색상 추가
	for (int i = 0; i < obj.vertex.size(); i++)
	{
		float r = static_cast<float>(rand()) / RAND_MAX;
		float g = static_cast<float>(rand()) / RAND_MAX;
		float b = static_cast<float>(rand()) / RAND_MAX;
		obj.color.emplace_back(glm::vec3{ r,g,b });
	}
	// 인덱스 리스트 추가

	for (unsigned int i = 0; i < obj.vertex.size(); i++)
	{
		obj.indexlist.emplace_back(i);
	}
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