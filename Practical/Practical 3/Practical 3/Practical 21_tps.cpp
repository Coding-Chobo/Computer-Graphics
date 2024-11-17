#define _CRT_SECURE_NO_WARNINGS
#include "CG_OPGL.h"

#define WIDTH 1200
#define HEIGHT 900
#define PI 3.1415926535897932384620588419716939
#define MAX_LINE_LENGTH 1024
GLchar* vertexSource, * fragmentSource; // 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //세이더 객체
GLuint shaderProgramID; // 세이더 프로그램

//그려질 오브젝트 선언
vector<Object> robot{}, cube{}, block{};
Coordinate Coordinate_system{};
FixedCamera camera{};

GLuint vao;
GLuint vbo[2];
GLuint EBO;

//실습에 필요한 변수들
unsigned int shape{ 0 };
bool mode_open{ false };
bool camera_animation{ false };
int rcnt{};
float CenterScreen_x = WIDTH / 2;
float CenterScreen_y = HEIGHT / 2;
float r = 1.0f;
float gravity = 0.005f;
float block_size{ 0.07 };
XYZ tps = { 0.0f, 0.15f,0.1f };
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
	glutSetCursor(GLUT_CURSOR_CROSSHAIR);

	InitBuffer();
	CreateShaderProgram();
	
	//도형 정보 초기화
	init_figure();
	init_camera();
	init_figure();
	//리콜 함수
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutPassiveMotionFunc(passiveMotion);
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyUp);
	glutSpecialFunc(SpecialKeyboard);
	glutTimerFunc(50, Timer, 0);
	glutDisplayFunc(Render);
	glutIdleFunc(mainLoop); // 메인 루프
	glutMainLoop(); //이벤트 루프 진입
}
//그리는 함수
GLvoid Render() {
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	//큐브 그리기
	for (int i = 0; i < cube.size(); i++)
	{
		cube[i].Draw_object();
	}
	//장애물 그리기
	for (int i = 0; i < block.size(); i++)
	{
		block[i].Draw_object();
	}
	//로봇 그리기
	for (int i = 0; i < rcnt; i++)
	{
		robot[i].Draw_object();
	}
	glutSwapBuffers();
}

void checkFrameBuffer() {
	int width = WIDTH;
	int height = HEIGHT;
	std::vector<unsigned char> pixels(3 * width * height);
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

	// 프레임 버퍼 데이터를 확인
	bool hasData = false;
	for (size_t i = 0; i < pixels.size(); ++i) {
		if (pixels[i] != 0) {
			hasData = true;
			break;
		}
	}

	if (hasData) {
		std::cout << "프레임 버퍼에 데이터가 있습니다." << std::endl;
	}
	else {
		std::cout << "프레임 버퍼가 비어 있습니다." << std::endl;
	}
}
GLvoid InitBuffer()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(2, vbo);
	glGenBuffers(1, &EBO);
}

//오브젝트별 그리기 함수
void Object::Draw_object() {
	Make_Matrix();
	UpdateVBO();
	glDrawElements(GL_TRIANGLES, 3 * this->indexlist.size(), GL_UNSIGNED_INT, 0);
}
//오브젝트별 변환 행렬
void Object::Make_Matrix() {
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "model");
	unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "view");
	unsigned int projectLocation = glGetUniformLocation(shaderProgramID, "project");
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

	viewMatrix = glm::lookAt(
		glm::vec3(*camera.x - 0.3 * camera.forward.x, *camera.y + 0.1f, *camera.z - 0.3 * camera.forward.z), // 카메라 위치
		glm::vec3(*camera.x, *camera.y - 0.1f , *camera.z) + camera.forward, // 카메라가 바라보는 지점
		glm::vec3(camera.up.x, camera.up.y, camera.up.z)  // 월드 업 벡터
	);

	// 투영 설정
	projectionMatrix = glm::perspective(
		glm::radians(90.0f), // 시야각
		(float)WIDTH / (float)HEIGHT, // 종횡비
		0.1f, 100.0f // 클립 평면 (near, far)
	);
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(projectLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
}
// BoundingBox 업데이트 함수
void Object::UpdateBoundingBox() {
	if (this->vertex.empty()) return;

	// 초기값 설정: 첫 번째 정점의 좌표를 기준으로 초기화
	float minX = this->vertex[0].x, minY = this->vertex[0].y, minZ = this->vertex[0].z;
	float maxX = this->vertex[0].x, maxY = this->vertex[0].y, maxZ = this->vertex[0].z;

	// 각 정점을 순회하며 최소/최대값을 찾음
	for (const auto& v : this->vertex) {
		minX = std::min(minX, v.x);
		minY = std::min(minY, v.y);
		minZ = std::min(minZ, v.z);
		maxX = std::max(maxX, v.x);
		maxY = std::max(maxY, v.y);
		maxZ = std::max(maxZ, v.z);
	}

	// BoundingBox 설정, transform 값을 추가하여 월드 좌표계 적용
	this->hitbox.left_front_top = { minX + this->transform.x, maxY + this->transform.y, maxZ + this->transform.z };
	this->hitbox.left_front_bottom = { minX + this->transform.x, minY + this->transform.y, maxZ + this->transform.z };
	this->hitbox.right_front_top = { maxX + this->transform.x, maxY + this->transform.y, maxZ + this->transform.z };
	this->hitbox.right_front_bottom = { maxX + this->transform.x, minY + this->transform.y, maxZ + this->transform.z };

	this->hitbox.left_back_top = { minX + this->transform.x, maxY + this->transform.y, minZ + this->transform.z };
	this->hitbox.left_back_bottom = { minX + this->transform.x, minY + this->transform.y, minZ + this->transform.z };
	this->hitbox.right_back_top = { maxX + this->transform.x, maxY + this->transform.y, minZ + this->transform.z };
	this->hitbox.right_back_bottom = { maxX + this->transform.x, minY + this->transform.y, minZ + this->transform.z };
}
//오브젝트별 버퍼 갱신
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
void FixedCamera::rotation_camera_vector_xz(float angle) {
	float vx = camera.forward.x;
	float vz = camera.forward.z;
	camera.forward.x = vx * cos(glm::radians(angle)) - vz * sin(glm::radians(angle));
	camera.forward.z = vx * sin(glm::radians(angle)) + vz * cos(glm::radians(angle));
	camera.right.x = camera.forward.x * cos(glm::radians(90.0)) - camera.forward.z * sin(glm::radians(90.0));
	camera.right.z = camera.forward.x * sin(glm::radians(90.0)) + camera.forward.z * cos(glm::radians(90.0));
}
void FreeCamera::rotation_camera_vector_xz(float angle) {
	float vx = camera.forward.x;
	float vz = camera.forward.z;
	camera.forward.x = vx * cos(glm::radians(angle)) - vz * sin(glm::radians(angle));
	camera.forward.z = vx * sin(glm::radians(angle)) + vz * cos(glm::radians(angle));
	camera.right.x = camera.forward.x * cos(glm::radians(90.0)) - camera.forward.z * sin(glm::radians(90.0));
	camera.right.z = camera.forward.x * sin(glm::radians(90.0)) + camera.forward.z * cos(glm::radians(90.0));
}
GLvoid UpdateVBO(Object object) {
	// VAO 바인드
	glBindVertexArray(vao);

	// 큐브 위치 버퍼 바인드
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, object.vertex.size() * sizeof(glm::vec3), object.vertex.data(), GL_DYNAMIC_DRAW);

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

void Timer(int value) {

	// 장애물과 검사
	for (size_t i = 0; i < block.size(); i++)
	{
		if (is_crash(robot[5], block[i])) // 충돌이 발생했는지 확인
		{
			float robotBottomY = robot[5].transform.y + robot[5].vertex[0].y;

			bool isLandingOnTop =
				// 로봇의 하단이 장애물 상단에 가깝게 있을 때 (착지할 때 약간의 여유를 줌)
				robotBottomY >= block_size - 0.01f &&
				robot[5].transform.x >= block[i].transform.x - 2 * block_size &&
				robot[5].transform.x <= block[i].transform.x + 2 * block_size &&
				robot[5].transform.z >= block[i].transform.z - 2 * block_size &&
				robot[5].transform.z <= block[i].transform.z + 2 * block_size;

			if (robot[0].is_jump && isLandingOnTop)
			{
				// 착지 처리
				robot[0].is_jump = false;
				robot[0].flight_time = 0;

				// 착지 위치 조정
				float gap = robotBottomY - block_size + 0.01f;
				for (int i = 0; i < rcnt; i++)
				{
					robot[i].transform.y -= gap;
				}
			}
			else if (!isLandingOnTop)
			{
				robot[0].dir_z = 0.0f;
			}
		}
	}
	//이동 구현
	for (size_t i = 0; i < rcnt; i++)
	{
		robot[i].transform.x += robot[0].dir_z * robot[0].speed * camera.forward.x;
		robot[i].transform.z += robot[0].dir_z * robot[0].speed * camera.forward.z;
	}

	// 장애물과 검사
	for (size_t i = 0; i < block.size(); i++)
	{
		if (is_crash(robot[5], block[i])) // 충돌이 발생했는지 확인
		{
			float robotBottomY = robot[5].transform.y + robot[5].vertex[0].y;

			bool isLandingOnTop =
				// 로봇의 하단이 장애물 상단에 가깝게 있을 때 (착지할 때 약간의 여유를 줌)
				robotBottomY >= block_size - 0.01f &&
				robot[5].transform.x >= block[i].transform.x - 2 * block_size &&
				robot[5].transform.x <= block[i].transform.x + 2 * block_size &&
				robot[5].transform.z >= block[i].transform.z - 2 * block_size &&
				robot[5].transform.z <= block[i].transform.z + 2 * block_size;

			if (robot[0].is_jump && isLandingOnTop)
			{
				// 착지 처리
				robot[0].is_jump = false;
				robot[0].flight_time = 0;

				// 착지 위치 조정
				float gap = robotBottomY - block_size + 0.01f;
				for (int i = 0; i < rcnt; i++)
				{
					robot[i].transform.y -= gap;
				}
			}
			else if (!isLandingOnTop)
			{
				robot[0].dir_x = 0.0f;
			}
		}
	}
	for (size_t i = 0; i < rcnt; i++)
	{
		robot[i].transform.x += robot[0].dir_x * robot[0].speed * camera.right.x;
		robot[i].transform.z += robot[0].dir_x * robot[0].speed * camera.right.z;
	}

	//중력 구현
	if (robot[0].flight_time != 0)
	{

		for (int i = 0; i < rcnt; i++)
		{
			robot[i].transform.y -= gravity * robot[0].flight_time;
		}
		if (is_crash(robot[5], cube[4])) {
			float y_gap = 0.0f - robot[5].transform.y;
			if (y_gap < 0.0f)
			{
				y_gap *= -1;
			}
			for (int i = 0; i < rcnt; i++)
			{
				robot[i].transform.y += y_gap;
			}
			robot[0].is_jump = false;
			robot[0].flight_time = 0;
		}
	}

	//점프 구현
	if (robot[0].is_jump)
	{
		for (int i = 0; i < rcnt; i++)
		{
			robot[i].transform.y += 0.05f;
		}
		robot[0].flight_time++;
	}

	//이동 애니메이션 구현 
	if (robot[0].is_move)
	{
		if (robot[4].rotation.x > 50)
		{
			r = -1.0f;
		}
		else if (robot[4].rotation.x < -50)
		{
			r = 1.0f;
		}
	}
	else
	{
		if (robot[4].rotation.x > -1.0 && robot[4].rotation.x < 1.0)
		{
			r = 0.0f;
		}
		else
		{
			if (robot[4].rotation.x > 50)
			{
				r = -1.0f;
			}
			else if (robot[4].rotation.x < -50)
			{
				r = 1.0f;
			}
		}
	}
	robot[2].rotation.x -= 4 * (robot[0].speed / 0.02f) * r;
	robot[3].rotation.x += 4 * (robot[0].speed / 0.02f) * r;
	robot[4].rotation.x += 4 * (robot[0].speed / 0.02f) * r;
	robot[5].rotation.x -= 4 * (robot[0].speed / 0.02f) * r;
	//문 열림 애니메이션
	if (mode_open)
	{
		for (size_t i = 0; i < 4; i++)
		{
			if (cube[0].vertex[0].x > -1.0f)
			{
				cube[0].vertex[i].x -= 0.05f;
				cube[1].vertex[i].x += 0.05f;
			}
		}
	}

	glutTimerFunc(50, Timer, 0);
}
void Mouse(int button, int state, int x, int y) {

	glutPostRedisplay(); // 다시 그리기 요청
}
// 마우스 이동 콜백 함수
void passiveMotion(int x, int y) {
	int deltaX = ((x - CenterScreen_x) / CenterScreen_x) * 90;
	int deltaY = ((CenterScreen_y - y) / CenterScreen_y) * 90;

	// 카메라 각도를 업데이트
	Update_camera(deltaX * robot[0].sensitivity, 0.0f * robot[0].sensitivity);

	//캐릭터 회전
	float angle = (180.0f / PI) * atan2(camera.forward.x, camera.forward.z);
	for (size_t i = 0; i < rcnt; i++)
	{
		robot[i].rotation.y = angle;
	}

	// 마우스를 다시 중앙으로 설정
	glutWarpPointer(CenterScreen_x, CenterScreen_y);
	glutPostRedisplay(); // 다시 그리기 요청
}
void Motion(int x, int y) {
	int deltaX = ((x - CenterScreen_x) / CenterScreen_x) * 90;
	int deltaY = ((CenterScreen_y - y) / CenterScreen_y) * 90;

	// 카메라 각도를 업데이트
	Update_camera(deltaX * robot[0].sensitivity, 0.0f * robot[0].sensitivity);

	//캐릭터 회전
	float angle = (180.0f / PI) * atan2(camera.forward.x, camera.forward.z);
	for (size_t i = 0; i < rcnt; i++)
	{
		robot[i].rotation.y = angle;
	}

	// 마우스를 다시 중앙으로 설정
	glutWarpPointer(CenterScreen_x, CenterScreen_y);
	glutPostRedisplay(); // 다시 그리기 요청
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
	case 'w':
		if (!robot[0].is_move)
		{
			r = 1.0f;
		}
		robot[0].is_move = true;
		robot[0].dir_z = 1.0f;
		break;
	case 's':
		if (!robot[0].is_move)
		{
			r = 1.0f;
		}
		robot[0].is_move = true;
		robot[0].dir_z = -1.0f;
		break;
	case 'a':
		if (!robot[0].is_move)
		{
			r = 1.0f;
		}
		robot[0].is_move = true;
		robot[0].dir_x = -1.0f;

		break;
	case 'd':
		if (!robot[0].is_move)
		{
			r = 1.0f;
		}
		robot[0].is_move = true;
		robot[0].dir_x = 1.0f;
		break;
	case '+':
		if (robot[0].speed < 0.1f)
		{
			robot[0].speed += 0.01f;
		}
		break;
	case '-':
		if (robot[0].speed > 0.02f)
		{
			robot[0].speed -= 0.01f;
		}
		break;
	case 'o':case 'O':
		mode_open = true;
		break;
	case 32: // 점프
		robot[0].is_jump = true;
		break;
	case '4': // 초기화
		init_figure();
		mode_open = false;
		break;
	case 'q':
		exit(0);
		break;
	}
}
GLvoid KeyUp(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'w':case 's':
		if (robot[0].dir_x ==0)
		{
			robot[0].is_move = false;
			if (robot[4].rotation.x <40 && robot[4].rotation.x > -40)
			{
				r *= -1;
			}
		}
		robot[0].dir_z = 0.0f;
		break;
	case 'a':case 'd':
		if (robot[0].dir_z == 0)
		{
			robot[0].is_move = false;
			if (robot[4].rotation.x <40 && robot[4].rotation.x > -40)
			{
				r *= -1;
			}
		}
		robot[0].dir_x = 0.0f;
		break;
	default:
		break;
	}
}
void mainLoop() {

}

bool is_crash(Object& objA, Object& objB) {
	// AABB 업데이트
	objA.UpdateBoundingBox();
	objB.UpdateBoundingBox();

	// 충돌 검사
	// X축 겹침 검사
	bool xOverlap = objA.hitbox.right_back_bottom.x >= objB.hitbox.left_back_bottom.x &&
		objA.hitbox.left_back_bottom.x <= objB.hitbox.right_back_bottom.x;

	// Y축 겹침 검사
	bool yOverlap = objA.hitbox.left_front_top.y >= objB.hitbox.left_front_bottom.y &&
		objA.hitbox.left_front_bottom.y <= objB.hitbox.left_front_top.y;

	// Z축 겹침 검사
	bool zOverlap = objA.hitbox.left_front_top.z >= objB.hitbox.left_back_top.z &&
		objA.hitbox.left_back_top.z <= objB.hitbox.left_front_top.z;

	// 모든 축에서 겹치면 충돌 발생
	return xOverlap && yOverlap && zOverlap;
}

void init_figure() {
	cube.clear();
	robot.clear();
	//---------------------------cube만들기-----------------------------
	Object c{};
	float cube_size = 2.0f;
	Make_cube_front_left(c, cube_size);
	cube.emplace_back(c);
	Make_cube_front_right(c, cube_size);
	cube.emplace_back(c);
	Make_cube_back(c, cube_size);
	cube.emplace_back(c);
	Make_cube_top(c, cube_size);
	cube.emplace_back(c);
	Make_cube_bottom(c, cube_size);
	cube.emplace_back(c);
	Make_cube_Left(c, cube_size);
	cube.emplace_back(c);
	Make_cube_Right(c, cube_size);
	cube.emplace_back(c);
	//---------------------------obj 파일-----------------------------
	Object objfile{};
	read_obj_file("robot_body.obj", objfile);
	robot.emplace_back(objfile);
	read_obj_file("robot_head.obj", objfile);
	robot.emplace_back(objfile);
	//팔 obj
	read_obj_file("robot_left_arm.obj", objfile);
	robot.emplace_back(objfile);
	read_obj_file("robot_right_arm.obj", objfile);
	robot.emplace_back(objfile);
	//다리 obj
	read_obj_file("robot_left_leg.obj", objfile);
	robot.emplace_back(objfile);
	read_obj_file("robot_right_leg.obj", objfile);
	robot.emplace_back(objfile);
	//로봇 코
	read_obj_file("robot_face.obj", objfile);
	robot.emplace_back(objfile);
	rcnt = robot.size();
	for (size_t i = 0; i < rcnt; i++)
	{
		robot[i].transform.y = robot[i].vertex[2].y - 0.02f;
		size_t vcnt = robot[i].vertex.size();
		for (size_t j = 0; j < vcnt; j++)
		{
			robot[i].vertex[j].y -= robot[i].transform.y;
		}
	}
	robot[0].speed = 0.05f;

	//-----------------------------장애물 생성---------------------------
	block.clear();
	for (size_t i = 0; i < 3; i++)
	{
		Make_Block(c, block_size);
		int x{}, z{};
		x = 2 * (rand() % 2) - 1;
		z = 2 * (rand() % 2) - 1;
		c.transform = {
			x * (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (0.8f * cube_size) + (0.1f * cube_size),
			block_size,
			z * (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (0.8f * cube_size) + (0.1f * cube_size)
		};

		block.emplace_back(c);
	}
	//-----------------------------색상입히기----------------------------
	AddColors(robot[0], 0.8f, 0.0f, 0.0f);
	AddColors(robot[1], 0.8f, 0.8f, 0.0f);

	AddColors(robot[2], 0.8f, 0.8f, 0.8f);
	AddColors(robot[3], 0.8f, 0.8f, 0.8f);

	AddColors(robot[4], 0.2f, 0.2f, 0.2f);
	AddColors(robot[5], 0.2f, 0.2f, 0.2f);
	AddColors(robot[6], 0.0f, 0.0f, 0.0f);
	//-------------------------변환정보 입력-------------------------
}
void init_camera(){
	//3인칭 카메라
	camera.x = &robot[1].transform.x;
	camera.y = &robot[1].transform.y;
	camera.z = &robot[1].transform.z;
	camera.forward = glm::vec3{ 0.0f,0.0f,1.0f };
	camera.up = glm::vec3{ 0.0f,1.0f,0.0f };
	camera.right = glm::vec3{ 1.0f,0.0f,0.0f };
} 

void Update_camera(float angle_xz, float angle_y) {
	float vx = camera.forward.x;
	float vz = camera.forward.z;
	camera.forward.x = vx * cos(glm::radians(angle_xz)) - vz * sin(glm::radians(angle_xz));
	camera.forward.z = vx * sin(glm::radians(angle_xz)) + vz * cos(glm::radians(angle_xz));
	camera.right.x = camera.forward.x * cos(glm::radians(90.0)) - camera.forward.z * sin(glm::radians(90.0));
	camera.right.z = camera.forward.x * sin(glm::radians(90.0)) + camera.forward.z * cos(glm::radians(90.0));
	for (size_t i = 0; i < rcnt; i++)
	{
		robot[i].rotation.y = angle_xz;
	}
}

void Make_cube_top(Object& obj, float size) {
	obj.vertex.clear();
	obj.vertex.emplace_back(glm::vec3{ size,size ,size });
	obj.vertex.emplace_back(glm::vec3{ size,size ,-size });
	obj.vertex.emplace_back(glm::vec3{ -size,size ,-size });
	obj.vertex.emplace_back(glm::vec3{ -size,size ,size });
	AddColors_Indexlist(obj, 0.0f, 0.0f, 1.0f);
	obj.transform = { 0.0f ,size ,0.0f };
	obj.scaling = { 1.0f ,1.0f ,1.0f };
	obj.rotation = { 0.0f ,0.0f ,0.0f };
}
void Make_cube_bottom(Object& obj, float size) {
	obj.vertex.clear();
	obj.vertex.emplace_back(glm::vec3{ size,-size ,size });
	obj.vertex.emplace_back(glm::vec3{ -size,-size ,size });
	obj.vertex.emplace_back(glm::vec3{ -size,-size ,-size });
	obj.vertex.emplace_back(glm::vec3{ size,-size ,-size });
	AddColors_Indexlist(obj, 1.0f, 0.0f, 0.0f);
	obj.transform = { 0.0f ,size ,0.0f };
	obj.scaling = { 1.0f ,1.0f ,1.0f };
	obj.rotation = { 0.0f ,0.0f ,0.0f };
}
void Make_cube_Left(Object& obj, float size) {
	obj.vertex.clear();
	obj.vertex.emplace_back(glm::vec3{ -size,size,size });
	obj.vertex.emplace_back(glm::vec3{ -size,size ,-size });
	obj.vertex.emplace_back(glm::vec3{ -size,-size ,-size });
	obj.vertex.emplace_back(glm::vec3{ -size,-size ,size });
	AddColors_Indexlist(obj, 1.0f, 1.0f, 0.0f);
	obj.transform = { 0.0f ,size ,0.0f };
	obj.scaling = { 1.0f ,1.0f ,1.0f };
	obj.rotation = { 0.0f ,0.0f ,0.0f };
}
void Make_cube_Right(Object& obj, float size) {
	obj.vertex.clear();
	obj.vertex.emplace_back(glm::vec3{ size,size,size });
	obj.vertex.emplace_back(glm::vec3{ size,-size ,size });
	obj.vertex.emplace_back(glm::vec3{ size,-size ,-size });
	obj.vertex.emplace_back(glm::vec3{ size,size ,-size });
	AddColors_Indexlist(obj, 1.0f, 0.0f, 1.0f);
	obj.transform = { 0.0f ,size ,0.0f };
	obj.scaling = { 1.0f ,1.0f ,1.0f };
	obj.rotation = { 0.0f ,0.0f ,0.0f };
}
void Make_cube_front_left(Object& obj, float size) {
	obj.vertex.clear();
	obj.vertex.emplace_back(glm::vec3{ 0.0f,size,size });
	obj.vertex.emplace_back(glm::vec3{ -size,size ,size });
	obj.vertex.emplace_back(glm::vec3{ -size,-size ,size });
	obj.vertex.emplace_back(glm::vec3{ 0.0f,-size ,size });
	AddColors_Indexlist(obj, 1.0f, 1.0f, 1.0f);
	obj.transform = { 0.0f ,size ,0.0f };
	obj.scaling = { 1.0f ,1.0f ,1.0f };
	obj.rotation = { 0.0f ,0.0f ,0.0f };
}
void Make_cube_front_right(Object& obj, float size) {
	obj.vertex.clear();
	obj.vertex.emplace_back(glm::vec3{ size,size,size });
	obj.vertex.emplace_back(glm::vec3{ 0.0f,size ,size });
	obj.vertex.emplace_back(glm::vec3{ 0.0f,-size ,size });
	obj.vertex.emplace_back(glm::vec3{ size,-size ,size });
	AddColors_Indexlist(obj, 1.0f, 1.0f, 1.0f);
	obj.transform = { 0.0f ,size ,0.0f };
	obj.scaling = { 1.0f ,1.0f ,1.0f };
	obj.rotation = { 0.0f ,0.0f ,0.0f };
}
void Make_cube_back(Object& obj, float size) {
	obj.vertex.clear();
	obj.vertex.emplace_back(glm::vec3{ size,size,-size });
	obj.vertex.emplace_back(glm::vec3{ -size,size ,-size });
	obj.vertex.emplace_back(glm::vec3{ -size,-size ,-size });
	obj.vertex.emplace_back(glm::vec3{ size,-size ,-size });
	AddColors_Indexlist(obj, 0.0f, 1.0f, 1.0f);
	obj.transform = { 0.0f ,size ,0.0f };
	obj.scaling = { 1.0f ,1.0f ,1.0f };
	obj.rotation = { 0.0f ,0.0f ,0.0f };
}

void Make_Block(Object& obj, float size) {
	obj.vertex.clear();
	obj.color.clear();
	obj.indexlist.clear();

	// 정점 추가
	obj.vertex.emplace_back(glm::vec3{ -size * 2, size / 2, -size * 2});
	obj.vertex.emplace_back(glm::vec3{ -size * 2, size / 2, size * 2 });
	obj.vertex.emplace_back(glm::vec3{ size * 2, size / 2, size * 2 });
	obj.vertex.emplace_back(glm::vec3{ size * 2, size / 2, -size * 2 });

	obj.vertex.emplace_back(glm::vec3{ -size * 2, -size / 2, -size * 2 });
	obj.vertex.emplace_back(glm::vec3{ -size * 2, -size / 2, size * 2 });
	obj.vertex.emplace_back(glm::vec3{ size * 2, -size / 2, size * 2 });
	obj.vertex.emplace_back(glm::vec3{ size * 2, -size / 2, -size * 2 });

	// 색상 추가
	int gray = rand() % 11;
	AddColors(obj, gray * 0.1f, gray * 0.1f, gray * 0.1f);

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

	glutPostRedisplay();
}

void AddColors(Object& fig, float r, float g, float b) {
	fig.color.clear();
	// 색상 입히기
	for (int i = 0; i < fig.vertex.size(); i++) {
		fig.color.emplace_back(glm::vec3{ r,g,b });
	}
}
void AddColors_Indexlist(Object& fig, float r, float g, float b) {
	fig.color.clear();
	fig.indexlist.clear();
	// 인덱스 추가 (삼각형을 이루는 방식으로 추가)
	for (unsigned int i = 1; i < fig.vertex.size() - 1; i++) {
		fig.indexlist.emplace_back(Index{ 0, i, i + 1 });
	}
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
	//UpdateVBO(obj);

	// 인덱스의 개수는 좌표계의 라인이 3개이므로 총 6개
	glDrawElements(GL_LINES, obj.indexlist.size(), GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
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
}

