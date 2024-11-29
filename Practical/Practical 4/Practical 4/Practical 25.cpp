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
Object center_sphere{}, shpere_xz{}, shpere_xz_up{}, shpere_xz_down{};
Object shpere_xz2{}, shpere_xz_up2{}, shpere_xz_down2{}, light{};
Coordinate Coordinate_system{}, path{}, path1{}, path2{}, path3{};
FreeCamera camera{};

GLuint vao;
GLuint vbo[3];
GLuint EBO;
XYZ lightColor{ 1.0f,1.0f ,1.0f };
//실습에 필요한 변수들
XYZ tps = { 0.0f, 0.15f,-0.1f };
int shape{};
int light_color{};
bool mode_animation{}, mode_orbit{};
float theta1 = 0.0f;
float theta2 = 0.0f;
float theta3 = 0.0f;
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
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	InitBuffer();
	CreateShaderProgram();
	//도형 정보 초기화
	init_figure();
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
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);


	//// 좌표계 그리기
	//init_Matrix();

	//UpdateVBO(path1);
	//glDrawElements(GL_LINE_LOOP, path1.indexlist.size(), GL_UNSIGNED_INT, 0);
	//UpdateVBO(path2);
	//glDrawElements(GL_LINE_LOOP, path2.indexlist.size(), GL_UNSIGNED_INT, 0);
	//UpdateVBO(path3);
	//glDrawElements(GL_LINE_LOOP, path3.indexlist.size(), GL_UNSIGNED_INT, 0);
	//
	//UpdateVBO(path);
	//Orbit_Path(path, shpere_xz);
	//glDrawElements(GL_LINE_LOOP, path.indexlist.size(), GL_UNSIGNED_INT, 0);
	//Orbit_Path(path, shpere_xz_up);
	//glDrawElements(GL_LINE_LOOP, path.indexlist.size(), GL_UNSIGNED_INT, 0);
	//Orbit_Path(path, shpere_xz_down);
	//glDrawElements(GL_LINE_LOOP, path.indexlist.size(), GL_UNSIGNED_INT, 0);
	center_sphere.Draw_object();
	
	shpere_xz.Draw_object_parent();
	//shpere_xz_up.Draw_object_parent();
	//shpere_xz_down.Draw_object_parent();
	shpere_xz2.Draw_object_parent();
	//shpere_xz_up2.Draw_object_parent();
	//shpere_xz_down2.Draw_object_parent();

	light.Draw_object_parent();

	//checkFrameBuffer();
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
	for (int i = 0; i < facecnt; i++)
	{
		glDrawArrays(GL_TRIANGLES, i * 3, 3);
	}
}
void Object::Draw_object_parent() {
	Make_Matrix_parent();
	UpdateVBO();
	for (int i = 0; i < facecnt; i++)
	{
		glDrawArrays(GL_TRIANGLES, i * 3, 3);
	}
}
void Object::Draw_object_element() {
	Make_Matrix();
	UpdateVBO();
	glDrawElements(GL_TRIANGLES, 3 * this->indexlist.size(), GL_UNSIGNED_INT, 0);
}
void Object::Draw_object_parent_element() {
	Make_Matrix_parent();
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
		glm::vec3(camera.x, camera.y, camera.z), // 카메라 위치
		glm::vec3(camera.at_x, camera.at_y, camera.at_z), // 카메라가 바라보는 지점
		glm::vec3(0.0f, 1.0f, 0.0f)  // 월드 업 벡터
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
//오브젝트별 변환 행렬
void Object::Make_Matrix_parent() {
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "model");
	unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "view");
	unsigned int projectLocation = glGetUniformLocation(shaderProgramID, "project");
	// 변환 관련 변수를 정의합니다.
	glm::mat4 modelMatrix = glm::mat4(1.0f);  // 모델 변환
	glm::mat4 viewMatrix = glm::mat4(1.0f);   // 뷰 변환 (카메라 변환)
	glm::mat4 projectionMatrix = glm::mat4(1.0f); // 투영 변환 (프로젝션 변환)
	// 변환 관련 변수를 정의합니다.

	modelMatrix = glm::translate(modelMatrix, glm::vec3(this->parent_transform->x, this->parent_transform->y, this->parent_transform->z));

	//상속관계
	modelMatrix = glm::rotate(modelMatrix, glm::radians(this->orbit_theta), glm::vec3(0.0f, 1.0f, 0.0f));  // Y축 회전

	modelMatrix = glm::translate(modelMatrix, glm::vec3(this->transform.x, this->transform.y, this->transform.z));

	// 제자리 회전: 공전된 위치에서 객체를 제자리에서 회전시킵니다.
	modelMatrix = glm::rotate(modelMatrix, glm::radians(this->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));  // Y축 회전
	modelMatrix = glm::rotate(modelMatrix, glm::radians(this->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));  // X축 회전
	modelMatrix = glm::rotate(modelMatrix, glm::radians(this->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));  // Z축 회전
	// 크기 변환
	modelMatrix = glm::scale(modelMatrix, glm::vec3(this->scaling.x, this->scaling.y, this->scaling.z));



	viewMatrix = glm::lookAt(
		glm::vec3(camera.x, camera.y, camera.z), // 카메라 위치
		glm::vec3(camera.at_x, camera.at_y, camera.at_z), // 카메라가 바라보는 지점
		glm::vec3(0.0f, 1.0f, 0.0f)  // 월드 업 벡터
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

	// 정점 버퍼
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, this->vertex.size() * sizeof(glm::vec3), this->vertex.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);

	// 노멀 버퍼
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, this->normal.size() * sizeof(glm::vec3), this->normal.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(1);

	// 텍스쳐 버퍼
	//glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	//glBufferData(GL_ARRAY_BUFFER, this->texture.size() * sizeof(glm::vec2), this->texture.data(), GL_STREAM_DRAW);
	//glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
	//glEnableVertexAttribArray(3);

	// 인덱스 버퍼
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indexlist.size() * sizeof(Index), this->indexlist.data(), GL_STATIC_DRAW);


	glUseProgram(shaderProgramID);
	unsigned int lightPosLocation = glGetUniformLocation(shaderProgramID, "lightPos");
	glUniform3f(lightPosLocation, light.transform.x, light.transform.y, light.transform.z);

	unsigned int lightColorLocation = glGetUniformLocation(shaderProgramID, "lightColor");
	glUniform3f(lightColorLocation, this->color[0].r, this->color[0].g, this->color[0].b);

	unsigned int objColorLocation = glGetUniformLocation(shaderProgramID, "objectColor");
	glUniform3f(objColorLocation, lightColor.x, lightColor.y, lightColor.z);

	unsigned int viewPosLocation = glGetUniformLocation(shaderProgramID, "viewPos");
	glUniform3f(viewPosLocation, camera.x, camera.y, camera.z);
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
void Orbit_Path(Coordinate path, Object planet) {
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "trans");
	// 변환 관련 변수를 정의합니다.
	glm::mat4 modelMatrix = glm::mat4(1.0f);  // 모델 변환
	glm::mat4 viewMatrix = glm::mat4(1.0f);   // 뷰 변환 (카메라 변환)
	glm::mat4 projectionMatrix = glm::mat4(1.0f); // 투영 변환 (프로젝션 변환)

	modelMatrix = glm::rotate(modelMatrix, glm::radians(30.f), glm::vec3(1.0f, 0.0f, 0.0f));  // X축 회전
	modelMatrix = glm::rotate(modelMatrix, glm::radians(30.f), glm::vec3(0.0f, 1.0f, 0.0f));  // Y축 회전

	// 공전 변환: 객체가 원점을 기준으로 회전하게 합니다.
	//modelMatrix = glm::rotate(modelMatrix, glm::radians(orbit_angle), glm::vec3(0.0f, 1.0f, 0.0f));  // Y축 공전

	modelMatrix = glm::translate(modelMatrix, glm::vec3(planet.transform.x, planet.transform.y, planet.transform.z));  // 이동

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
		0.1f, 100.0f // 클립 평면 (near, far)
	);

	// 최종 변환 행렬(MVP)을 계산하여 GPU로 전달
	glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
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
	//theta1 += 0.05f;
	//theta2 += 0.1f;
	//theta3 += 0.2f;
	shpere_xz.transform = { cos(theta1),0.0f,sin(theta1) };
	shpere_xz_up.transform = { cos(theta2),cos(theta2),sin(theta2) };
	shpere_xz_down.transform = { cos(theta3),-cos(theta3),sin(theta3) };
	
	if (mode_orbit)
	{
		float vx = light.transform.x;
		float vz = light.transform.z;
		light.transform.x = vx * cos(glm::radians(-2.0f)) - vz * sin(glm::radians(-2.0f));
		light.transform.z = vx * sin(glm::radians(-2.0f)) + vz * cos(glm::radians(-2.0f));
	}
	glutTimerFunc(50, Timer, 0);
	glutPostRedisplay(); // 다시 그리기 요청
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
	case 'y':
		mode_animation = (mode_animation + 1) % 2;
		break;
	case 'r':
		mode_orbit = !mode_orbit;
		break;
	case 'z':
		glm::vec3 direction = glm::normalize(glm::vec3{ light.parent_transform->x - light.transform.x,
											0.0f,light.parent_transform->z - light.transform.z });
		light.transform.x += direction.x / 10;
		light.transform.z += direction.z / 10;
		break;
	case 'Z':
		glm::vec3 direction2 = glm::normalize(glm::vec3{ light.parent_transform->x - light.transform.x,
											0.0f,light.parent_transform->z - light.transform.z });
		light.transform.x -= direction2.x / 10;
		light.transform.z -= direction2.z / 10;
		break;
	case 'x':
		camera.x -= 0.1f;
		break;
	case 'X':
		camera.x += 0.1f;
		break;

	case 'c':
		light_color = (light_color + 1) % 3;
		switch (light_color)
		{
		case 0:
			lightColor = { 1.0f,1.0f ,1.0f };
			break;
		case 1:
			lightColor = { 0.5f,0.5f ,1.0f };
			break;
		case 2:
			lightColor = { 1.0f,0.5f ,0.5f };
			break;
		default:
			break;
		}
		break;
	case '4': // 초기화
		init_figure();
		break;
	case 'q':
		exit(0);
		break;
	}
}

void mainLoop() {


	glutPostRedisplay(); // 다시 그리기 요청
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
	//---------------------------cube만들기-----------------------------
	read_obj_file("cube.obj", light);
	AddColors(light, 1.0f, 1.0f, 1.0f);
	//---------------------------sphere-----------------------------
	read_obj_file("sphere.obj", center_sphere);
	read_obj_file("sphere.obj", shpere_xz);
	read_obj_file("sphere.obj", shpere_xz_up);
	read_obj_file("sphere.obj", shpere_xz_down);
	read_obj_file("sphere.obj", shpere_xz2);
	read_obj_file("sphere.obj", shpere_xz_up2);
	read_obj_file("sphere.obj", shpere_xz_down2);
	shpere_xz.parent_transform = &center_sphere.transform;
	shpere_xz_up.parent_transform = &center_sphere.transform;
	shpere_xz_down.parent_transform = &center_sphere.transform;
	shpere_xz2.parent_transform = &shpere_xz.transform;
	shpere_xz_up2.parent_transform = &shpere_xz_up.transform;
	shpere_xz_down2.parent_transform = &shpere_xz_down.transform;
	
	//-----------------------------색상입히기----------------------------
	AddColors(shpere_xz, 0.0f, 1.0f, 1.0f);
	AddColors(shpere_xz_up, 0.0f, 1.0f, 1.0f);
	AddColors(shpere_xz_down, 0.0f, 1.0f, 1.0f);
	AddColors(shpere_xz2, 1.0f, 0.5f, 0.5f);
	AddColors(shpere_xz_up2, 1.0f, 0.5f, 0.5f);
	AddColors(shpere_xz_down2, 1.0f, 0.5f, 0.5f);
	//-------------------------변환정보 입력-------------------------
	center_sphere.scaling = { 10.0f,10.0f ,10.0f };
	shpere_xz.scaling = shpere_xz_up.scaling = shpere_xz_down.scaling = { 15.0f,15.0f,15.0f };
	shpere_xz2.scaling = shpere_xz_up2.scaling = shpere_xz_down2.scaling = { 10.0f,10.0f,10.0f };
	shpere_xz2.transform = shpere_xz_up2.transform = shpere_xz_down2.transform = { 0.5f,0.0f,0.0f };

	light.transform = { 1.0f,0.1f,1.0f };
	light.scaling = { 0.2f,0.2f,0.2f };
	light.parent_transform = &center_sphere.transform;
}
void init_camera() {
	//3인칭 카메라
	camera.x = 0.0f, camera.y = 1.0f, camera.z = 4.0f;
	camera.at_x = 0.0f, camera.at_y = 0.0f, camera.at_z = 0.0f;
	camera.forward = glm::vec3{ 0.0f,0.0f,1.0f };
	camera.up = glm::vec3{ 0.0f,1.0f,0.0f };
	camera.right = glm::vec3{ 1.0f,0.0f,0.0f };
}

void Update_camera(FreeCamera& camera, float angle_xz, float angle_y) {
	float vx = camera.forward.x;
	float vz = camera.forward.z;
	camera.forward.x = vx * cos(glm::radians(angle_xz)) - vz * sin(glm::radians(angle_xz));
	camera.forward.z = vx * sin(glm::radians(angle_xz)) + vz * cos(glm::radians(angle_xz));
	camera.right.x = camera.forward.x * cos(glm::radians(90.0)) - camera.forward.z * sin(glm::radians(90.0));
	camera.right.z = camera.forward.x * sin(glm::radians(90.0)) + camera.forward.z * cos(glm::radians(90.0));
}
void Update_camera(FixedCamera& camera, float angle_xz, float angle_y) {
	float vx = camera.forward.x;
	float vz = camera.forward.z;
	camera.forward.x = vx * cos(glm::radians(angle_xz)) - vz * sin(glm::radians(angle_xz));
	camera.forward.z = vx * sin(glm::radians(angle_xz)) + vz * cos(glm::radians(angle_xz));
	camera.right.x = camera.forward.x * cos(glm::radians(90.0)) - camera.forward.z * sin(glm::radians(90.0));
	camera.right.z = camera.forward.x * sin(glm::radians(90.0)) + camera.forward.z * cos(glm::radians(90.0));
}

void Make_Cube(float size, Object& obj) {
	obj.vertex.clear();
	obj.color.clear();
	obj.indexlist.clear();

	// 정점 추가
	obj.vertex.emplace_back(glm::vec3{ -size, size, -size });
	obj.vertex.emplace_back(glm::vec3{ -size, size, size });
	obj.vertex.emplace_back(glm::vec3{ size, size, size });
	obj.vertex.emplace_back(glm::vec3{ size, size,  -size });

	obj.vertex.emplace_back(glm::vec3{ -size,  -size,  -size });
	obj.vertex.emplace_back(glm::vec3{ -size,  -size,  +size });
	obj.vertex.emplace_back(glm::vec3{ +size,  -size,  +size });
	obj.vertex.emplace_back(glm::vec3{ +size,  -size,  -size });

	// 색상 추가
	AddColors(obj, 0.0f, 1.0f, 1.0f);

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

	obj.normal.emplace_back(glm::vec3{ 0.0f,1.0f,0.0f });
	obj.normal.emplace_back(glm::vec3{ 0.0f,1.0f,0.0f });
	obj.normal.emplace_back(glm::vec3{ 1.0f,0.0f,0.0f });
	obj.normal.emplace_back(glm::vec3{ 1.0f,0.0f,0.0f });
	obj.normal.emplace_back(glm::vec3{ 1.0f,0.0f,0.0f });
}
void Make_Tetra(float size, Object& obj) {
	float a = 0.85 * size;
	obj.vertex.emplace_back(glm::vec3{ 0.0f, size, 0.0f });
	obj.vertex.emplace_back(glm::vec3{ 0.0f, -size / 2, a });
	obj.vertex.emplace_back(glm::vec3{ 0.85 * a, -size / 2, -a / 2 });
	obj.vertex.emplace_back(glm::vec3{ -0.85 * a, -size / 2, -a / 2 });

	// 색상 추가
	AddColors(obj, 1.0f, 0.5f, 0.5f);

	// 인덱스 리스트 추가
	obj.indexlist.emplace_back(Index{ 0, 1, 2 });
	obj.indexlist.emplace_back(Index{ 0, 2, 3 });
	obj.indexlist.emplace_back(Index{ 0, 3, 1 });
	obj.indexlist.emplace_back(Index{ 1, 2, 3 });
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

// OBJ 파일 읽기 함수
void read_obj_file(const char* filename, Object& model) {
	FILE* file;
	fopen_s(&file, filename, "r");
	if (!file) {
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}

	char line[MAX_LINE_LENGTH];
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec3> temp_vertices2;
	std::vector<glm::vec3> temp_normals;
	std::vector<glm::vec3> temp_normals2;
	std::vector<glm::vec2> temp_textures;
	std::vector<Index> temp_indices;

	while (fgets(line, sizeof(line), file)) {
		read_newline(line); // 개행문자 제거

		// 정점(Vertex) 데이터 처리
		if (line[0] == 'v' && line[1] == ' ') {
			glm::vec3 vertex;
			sscanf_s(line + 2, "%f %f %f", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		// 정점(Vertex) 데이터 처리
		if (line[0] == 'v' && line[1] == 'n') {
			glm::vec3 normal;
			sscanf_s(line + 2, "%f %f %f", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		// 정점(Vertex) 데이터 처리
		if (line[0] == 'v' && line[1] == 't') {
			glm::vec2 texture;
			sscanf_s(line + 2, "%f %f", &texture.x, &texture.y);
			temp_textures.push_back(texture);
		}
		// 면(Face) 데이터 처리
		else if (line[0] == 'f' && line[1] == ' ') {
			unsigned int v1, v2, v3;
			unsigned int vt1, vt2, vt3; // 텍스처 좌표 인덱스
			unsigned int vn1, vn2, vn3; // 법선 벡터 인덱스
			sscanf_s(line + 2, "%u/%u/%u %u/%u/%u %u/%u/%u",
				&v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3);

			// OBJ 인덱스는 1부터 시작하기 때문에 0부터 시작하도록 조정
			Index face = { v1 - 1, v2 - 1,v3 - 1 };
			temp_indices.push_back(face);
			temp_vertices2.push_back(glm::vec3{ temp_vertices[v1 - 1] });
			temp_vertices2.push_back(glm::vec3{ temp_vertices[v2 - 1] });
			temp_vertices2.push_back(glm::vec3{ temp_vertices[v3 - 1] });
			temp_normals2.push_back(glm::vec3{ temp_normals[vn1 - 1] });
			temp_normals2.push_back(glm::vec3{ temp_normals[vn2 - 1] });
			temp_normals2.push_back(glm::vec3{ temp_normals[vn3 - 1] });
			model.facecnt++;
		}
	}

	fclose(file);

	// Object 구조체에 정점 및 인덱스 데이터를 전달
	model.vertex = temp_vertices2; // 정점 데이터를 그대로 저장
	model.normal = temp_normals2; // 인덱스 데이터를 그대로 저장
	model.texture = temp_textures; // 인덱스 데이터를 그대로 저장
	model.indexlist = temp_indices; // 인덱스 데이터를 그대로 저장

	// 색상은 임의로 설정 (추후 수정 가능)
	AddColors(model, 0.5f, 0.5f, 1.0f);

	model.transform = { 0.0f,0.0f,0.0f };
	model.scaling = { 1.0f,1.0f,1.0f };
	model.rotation = { 0.0f,0.0f,0.0f };
}