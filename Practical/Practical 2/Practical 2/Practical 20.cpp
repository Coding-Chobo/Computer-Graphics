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
Object board{};
vector<Object> tank{};
Coordinate Coordinate_system{};
Camera_Info camera1{}, camera2{}, camera3{};

GLuint vao;
GLuint vbo[2];
GLuint EBO;

//실습에 필요한 변수들
unsigned int shape{ 0 };
bool mode_animation[10]{ false };
unsigned int camera_animation{ false };
int count{};
int viewID = 1;127
float size = 0.2f;
float dir_x = 0.0f;
float dir_z = 0.0f;
float orbit_size{ 0.5f };
float radius = 0.0f;
GLfloat orbit_angle{ 0.0f };  // 공전 각도

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
	init_camera();
	//리콜 함수
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeyboard);
	glutTimerFunc(50, Timer, 0);
	glutDisplayFunc(Render);
	glutIdleFunc(mainLoop); // 메인 루프
	glutMainLoop(); //이벤트 루프 진입
}
//그리는 함수
GLvoid Render() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	//---------------------------View 1------------------------
	glViewport(0, HEIGHT / 2, WIDTH / 2, HEIGHT / 2);
	viewID = 1;
	RenderScene();

	//---------------------------View 2------------------------
	glViewport(WIDTH / 2, HEIGHT / 2, WIDTH / 2, HEIGHT / 2);
	viewID = 2;
	RenderScene();

	//---------------------------View 3------------------------
	glViewport(WIDTH / 4, 0, WIDTH / 2, HEIGHT / 2);
	viewID = 3;
	RenderScene();


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

	// 카메라 설정: 카메라 위치, 카메라가 바라보는 위치, 월드 업 벡터
	if (viewID == 1)
	{
		viewMatrix = glm::lookAt(
			glm::vec3(camera1.x, camera1.y, camera1.z), // 카메라 위치
			glm::vec3(camera1.at_x, camera1.at_y, camera1.at_z), // 카메라가 바라보는 지점
			glm::vec3(0.0f, 1.0f, 0.0f)  // 월드 업 벡터
		);
	}
	else if (viewID == 2)
	{
		viewMatrix = glm::lookAt(
			glm::vec3(camera2.x, camera2.y, camera2.z), // 카메라 위치
			glm::vec3(camera2.at_x, camera2.at_y, camera2.at_z), // 카메라가 바라보는 지점
			glm::vec3(0.0f, 1.0f, 0.0f)  // 월드 업 벡터
		);
	}
	else if (viewID == 3)
	{
		viewMatrix = glm::lookAt(
			glm::vec3(camera3.x, camera3.y, camera3.z), // 카메라 위치
			glm::vec3(camera3.at_x, camera3.at_y, camera3.at_z), // 카메라가 바라보는 지점
			glm::vec3(0.0f, 1.0f, 0.0f)  // 월드 업 벡터
		);
	}

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

void Object::Draw_object() {
	Make_Matrix();
	UpdateVBO();
	glDrawElements(GL_TRIANGLES, 3 * this->indexlist.size(), GL_UNSIGNED_INT, 0);
}

//오브젝트별 변환 행렬
void Object::Make_Matrix() {
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "trans");
	// 변환 관련 변수를 정의합니다.
	glm::mat4 modelMatrix = glm::mat4(1.0f);  // 모델 변환
	glm::mat4 viewMatrix = glm::mat4(1.0f);   // 뷰 변환 (카메라 변환)
	glm::mat4 projectionMatrix = glm::mat4(1.0f); // 투영 변환 (프로젝션 변환)
	// 변환 관련 변수를 정의합니다.

	modelMatrix = glm::translate(modelMatrix, glm::vec3(this->transform.x, this->transform.y, this->transform.z));

	// 제자리 회전: 공전된 위치에서 객체를 제자리에서 회전시킵니다.
	modelMatrix = glm::rotate(modelMatrix, glm::radians(this->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));  // Y축 회전
	modelMatrix = glm::rotate(modelMatrix, glm::radians(this->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));  // X축 회전
	modelMatrix = glm::rotate(modelMatrix, glm::radians(this->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));  // Z축 회전
	// 크기 변환
	modelMatrix = glm::scale(modelMatrix, glm::vec3(this->scaling.x, this->scaling.y, this->scaling.z));
	if (viewID == 1)
	{
		viewMatrix = glm::lookAt(
			glm::vec3(camera1.x, camera1.y, camera1.z), // 카메라 위치
			glm::vec3(camera1.at_x, camera1.at_y, camera1.at_z), // 카메라가 바라보는 지점
			glm::vec3(0.0f, 1.0f, 0.0f)  // 월드 업 벡터
		);
	}
	else if (viewID == 2)
	{
		viewMatrix = glm::lookAt(
			glm::vec3(camera2.x, camera2.y, camera2.z), // 카메라 위치
			glm::vec3(camera2.at_x, camera2.at_y, camera2.at_z), // 카메라가 바라보는 지점
			glm::vec3(0.0f, 1.0f, 0.0f)  // 월드 업 벡터
		);
	}
	else if (viewID == 3)
	{
		viewMatrix = glm::lookAt(
			glm::vec3(camera3.x, camera3.y, camera3.z), // 카메라 위치
			glm::vec3(camera3.at_x, camera3.at_y, camera3.at_z), // 카메라가 바라보는 지점
			glm::vec3(0.0f, 1.0f, 0.0f)  // 월드 업 벡터
		);
	}
	// 투영 설정
	projectionMatrix = glm::perspective(
		glm::radians(90.0f), // 시야각
		(float)WIDTH / (float)HEIGHT, // 종횡비
		0.1f, 100.0f // 클립 평면 (near, far)
	);

	// 최종 변환 행렬(MVP)을 계산하여 GPU로 전달
	glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
}

void RenderScene() {
	// 좌표계 그리기
	init_Matrix();
	Draw_Coordinate(Coordinate_system);
	// 보드 그리기
	board.Draw_object();
	for (int i = 0; i < tank.size(); i++) {
		tank[i].Draw_object();
	}
}

void Timer(int value) {
	if (mode_animation[0])// 크레인 x축 좌 이동
	{
		for (int i = 0; i < tank.size(); i++)
		{
			if (tank[i].transform.x > 2.2f && dir_x > 0.0)
			{
				dir_x = -1.0f;
			}
			else if (tank[i].transform.x < -2.2f && dir_x < 0.0)
			{
				dir_x = 1.0f;
			}
			tank[i].transform.x += dir_x * 0.05f;
		}
	}
	else if (mode_animation[1])// 크레인 x축 좌우 이동
	{
		for (int i = 0; i < tank.size(); i++)
		{
			if (tank[i].transform.x > 2.2f && dir_x > 0.0)
			{
				dir_x = -1.0f;
			}
			else if (tank[i].transform.x < -2.2f && dir_x < 0.0)
			{
				dir_x = 1.0f;
			}
			tank[i].transform.x += dir_x * 0.05f;
		}
	}
	if (mode_animation[2]) // 중앙 몸체,위 팔 y 축 회전 (양의 방향)
	{
		for (int i = 3; i < tank.size(); i++)
		{
			tank[i].rotation.y += 1.0f;
		}
	}
	else if (mode_animation[3]) // 중앙 몸체,위 팔 y 축 회전 (음의 방향)
	{
		for (int i = 3; i < tank.size(); i++)
		{
			tank[i].rotation.y -= 1.0f;
		}
	}
	if (mode_animation[4])// 포신 y 축 회전 (양의 방향) 서로 반대방향
	{
		tank[1].rotation.y += 1.0f;
		tank[2].rotation.y -= 1.0f;
	}
	else if (mode_animation[5])// 포신 y 축 회전 (음의 방향) 서로 반대방향
	{
		tank[1].rotation.y -= 1.0f;
		tank[2].rotation.y += 1.0f;
	}

	if (mode_animation[6]) //포신 2개 합체
	{
		if (count < 50)
		{
			for (size_t i = 0; i < 8; i++)
			{
				tank[1].vertex[i].x += 0.0035f;
				tank[2].vertex[i].x -= 0.0035f;
			}
			count++;
		}
	}
	else if (mode_animation[7]) //포신 2개 분리
	{
		if (count > 0)
		{
			for (size_t i = 0; i < 8; i++)
			{
				tank[1].vertex[i].x -= 0.0035f;
				tank[2].vertex[i].x += 0.0035f;
			}
			count--;
		}
	}
	if (mode_animation[8]) // 위의 팔이 z축에 대해 음양으로 서로 반대방향으로 회전 최대 90도씩 
	{
		if (tank[4].rotation.x > 70)
		{
			dir_z = -1.0f;
		}
		else if (tank[4].rotation.x < -70)
		{
			dir_z = 1.0f;
		}
		tank[4].rotation.x += 2 * dir_z;
		tank[5].rotation.x -= 2 * dir_z;
	}
	if (mode_animation[9]) // 위의 팔이 z축에 대해 양음으로 서로 반대방향으로 회전 최대 90도씩 
	{
		if (tank[4].rotation.x > 90)
		{
			dir_z = -1.0f;
		}
		else if (tank[4].rotation.x < -90)
		{
			dir_z = 1.0f;
		}
		tank[4].rotation.x += 2 * dir_z;
		tank[5].rotation.x -= 2 * dir_z;
	}
	if (camera_animation == 1)
	{
		float cx = camera1.x;
		float cz = camera1.z;
		camera1.x = cx * cos(glm::radians(2.0f)) - cz * sin(glm::radians(2.0f));
		camera1.z = cx * sin(glm::radians(2.0f)) + cz * cos(glm::radians(2.0f));
	}
	else if (camera_animation == 2)
	{
		float cx = camera1.x;
		float cz = camera1.z;
		camera1.x = cx * cos(glm::radians(-2.0f)) - cz * sin(glm::radians(-2.0f));
		camera1.z = cx * sin(glm::radians(-2.0f)) + cz * cos(glm::radians(-2.0f));
	}
	glutTimerFunc(50, Timer, 0);
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
	case 'b':
		dir_x = 1.0f;
		mode_animation[0] = !mode_animation[0];
		mode_animation[1] = false;
		break;
	case 'B':
		dir_x = -1.0f;
		mode_animation[1] = !mode_animation[1];
		mode_animation[0] = false;
		break;
	case 'm':
		mode_animation[2] = !mode_animation[2];
		mode_animation[3] = false;
		break;
	case 'M':
		mode_animation[3] = !mode_animation[3];
		mode_animation[2] = false;
		break;
	case 'f':
		mode_animation[4] = !mode_animation[4];
		mode_animation[5] = false;
		break;
	case 'F':
		mode_animation[5] = !mode_animation[5];
		mode_animation[4] = false;
		break;
	case 'e':
		mode_animation[6] = !mode_animation[6];
		mode_animation[7] = false;
		break;
	case 'E':
		mode_animation[7] = !mode_animation[7];
		mode_animation[6] = false;
		break;
	case 't':
		dir_z = 1.0f;
		mode_animation[8] = !mode_animation[8];
		mode_animation[9] = false;
		break;
	case 'T':
		dir_z = -1.0f;
		mode_animation[9] = !mode_animation[9];
		mode_animation[8] = false;
		break;
	case 'w':
		camera1.z -= 0.1f;
		break;
	case 's':
		camera1.z += 0.1f;
		break;
	case 'a':
		camera1.x -= 0.1f;
		break;
	case 'd':
		camera1.x += 0.1f;
		break;
	case 'x':
		camera1.y -= 0.1f;
		break;
	case 32:
		camera1.y += 0.1f;
		break;
	case 'r':
		camera1.at_x = camera1.at_y = camera1.at_z = 0.0f;
		camera_animation = 1;
		break;
	case 'R':
		camera1.at_x = camera1.at_y = camera1.at_z = 0.0f;
		camera_animation = 2;
		break;
	case 'v': // 기존의 a
		camera1.at_x = camera1.at_y = camera1.at_z = 0.0f;
		camera_animation = 1;
		break;
	case 'V':
		camera1.at_x = camera1.at_y = camera1.at_z = 0.0f;
		camera_animation = 0;
		break;
	case '4':
		for (int i = 0; i < 10; i++)
		{
			mode_animation[i] = false;
		}
		camera_animation = 0;
		break;
	case 'c': case 'C':
		init_figure(0.0f);
		break;
	case 'q':
		exit(0);
		break;
	}
}

void mainLoop() {

	glutPostRedisplay(); // 다시 그리기 요청
}

void init_figure(float size) {
	//---------------------------obj파일-----------------------------
	tank.clear();
	Make_Board(board, 2.5f);
	//-----------------월드 변환 좌표 입력--------------------------

	//---------------------------sphere-----------------------------
	Object objfile{};
	read_obj_file("tank_body.obj", objfile);
	tank.emplace_back(objfile);
	read_obj_file("tank_stick_xl.obj", objfile);
	tank.emplace_back(objfile);
	read_obj_file("tank_stick_xr.obj", objfile);
	tank.emplace_back(objfile);

	read_obj_file("tank_top.obj", objfile);
	tank.emplace_back(objfile);
	read_obj_file("tank_stick_yl.obj", objfile);
	tank.emplace_back(objfile);
	read_obj_file("tank_stick_yr.obj", objfile);
	tank.emplace_back(objfile);
	for (int i = 0; i < tank.size(); i++)
	{
		for (size_t j = 0; j < 8; j++)
		{
			tank[i].vertex[j].y += 0.3f;
		}
	}
	tank[4].transform.y = tank[4].vertex[0].y;
	tank[5].transform.y = tank[5].vertex[0].y;
	for (size_t j = 0; j < 8; j++)
	{
		tank[4].vertex[j].y -= tank[4].transform.y;
		tank[5].vertex[j].y -= tank[5].transform.y;
	}

	//-----------------------------색상입히기----------------------------
	AddColors(tank[0], 0.8f, 0.0f, 0.0f);

	AddColors(tank[1], 0.8f, 0.8f, 0.0f);
	AddColors(tank[2], 0.8f, 0.8f, 0.0f);

	AddColors(tank[3], 0.8f, 0.8f, 0.8f);

	AddColors(tank[4], 0.0f, 0.8f, 0.8f);
	AddColors(tank[5], 0.0f, 0.8f, 0.8f);
	//-------------------------변환정보 입력--------------------------


}

void init_camera() {
	//camera 1
	camera1.x = 0.0f, camera1.y = 2.0f, camera1.z = 4.0f;
	camera1.at_x = 0.0f, camera1.at_y = 0.0f, camera1.at_z = 0.0f;
	//camera 2 - top
	camera2.x = 0.0f, camera2.y = 4.0f, camera2.z = 0.01f;
	camera2.at_x = 0.0f, camera2.at_y = 0.0f, camera2.at_z = 0.0f;
	//camera 3 - front
	camera3.x = 0.0f, camera3.y = 0.0f, camera3.z = 4.0f;
	camera3.at_x = 0.0f, camera3.at_y = 0.0f, camera3.at_z = 0.0f;
}

void Make_Board(Object& obj, float size) {
	obj.vertex.emplace_back(glm::vec3{ size,0.0f,size });
	obj.vertex.emplace_back(glm::vec3{ size,0.0f,-size });
	obj.vertex.emplace_back(glm::vec3{ -size,0.0f,-size });
	obj.vertex.emplace_back(glm::vec3{ -size,0.0f,size });
	AddColors(obj, 0.8f, 0.8f, 1.0f);
	for (unsigned int i = 1; i < obj.vertex.size() - 1; i++)
	{
		obj.indexlist.emplace_back(Index{ 0, i, i + 1 });
	}
}

void AddColors(Object& fig, float r, float g, float b) {
	fig.color.clear();
	// 색상 입히기
	for (int i = 0; i < fig.vertex.size(); i++) {
		fig.color.emplace_back(glm::vec3{ r,g,b });
	}
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

void Object::UpdateVBO() {
	// VAO 바인드
	glBindVertexArray(vao);

	// 큐브 위치 버퍼 바인드
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, this->vertex.size() * sizeof(glm::vec3), this->vertex.data(), GL_STREAM_DRAW);

	// 큐브 인덱스 버퍼 바인드
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// 전체 인덱스 배열의 크기 전달
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indexlist.size() * sizeof(Index), this->indexlist.data(), GL_DYNAMIC_DRAW);

	// 위치 속성 설정
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(0);

	// 큐브 색상 버퍼 바인드
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, this->color.size() * sizeof(glm::vec3), this->color.data(), GL_DYNAMIC_DRAW);
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
		model.color.push_back(glm::vec3(1.0f, 1.0f, 1.0f)); // 흰색으로 설정
	}
	model.transform = { 0.0f,0.0f,0.0f };
	model.scaling = { 1.0f,1.0f,1.0f };
	model.rotation = { 0.0f,0.0f,0.0f };
	UpdateVBO(model);
}