#define _CRT_SECURE_NO_WARNINGS
#include "CG_OPGL.h"

#define WIDTH 1200
#define HEIGHT 900
#define PI 3.1415926535897932384620588419716939
#define MAX_LINE_LENGTH 1024
GLchar* vertexSource, * fragmentSource; // �ҽ��ڵ� ���� ����
GLuint vertexShader, fragmentShader; //���̴� ��ü
GLuint shaderProgramID; // ���̴� ���α׷�

//�׷��� ������Ʈ ����
Object board{}, light{};
vector<Object> tank{};
FreeCamera camera{};

GLuint vao;
GLuint vbo[3];
GLuint EBO;
XYZ lightColor{ 1.0f,1.0f ,1.0f };
//�ǽ��� �ʿ��� ������
XYZ tps = { 0.0f, 0.15f,-0.1f };
//�ǽ��� �ʿ��� ������
unsigned int shape{ 0 };
bool mode_animation[10]{ false };
unsigned int camera_animation{ false };
int count{};
int light_color{};
bool mode_orbit{};
float size = 0.2f;
float dir_x = 0.0f;
float dir_z = 0.0f;
float orbit_size{ 0.5f };
float radius = 0.0f;
GLfloat orbit_angle{ 0.0f };  // ���� ����
//-----------------------------------------------------------------------
void main(int argc, char** argv)
{
	srand(time(NULL));
	// ������ ����
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("OpenGL Practical");

	// GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	glewInit();
	// ��������
	glEnable(GL_CULL_FACE);
	//glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	InitBuffer();
	CreateShaderProgram();
	//���� ���� �ʱ�ȭ
	init_figure();
	init_camera();
	//���� �Լ�
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeyboard);
	glutTimerFunc(50, Timer, 0);
	glutDisplayFunc(Render);
	glutIdleFunc(mainLoop); // ���� ����
	glutMainLoop(); //�̺�Ʈ ���� ����
}
//�׸��� �Լ�
GLvoid Render() {
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	//board.Draw_object();
	for (size_t i = 0; i < tank.size(); i++)
	{
		tank[i].Draw_object();
	}
	light.Draw_object_parent();
	board.Draw_object();
	//checkFrameBuffer();
	glutSwapBuffers();
}

void checkFrameBuffer() {
	int width = WIDTH;
	int height = HEIGHT;
	std::vector<unsigned char> pixels(3 * width * height);
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

	// ������ ���� �����͸� Ȯ��
	bool hasData = false;
	for (size_t i = 0; i < pixels.size(); ++i) {
		if (pixels[i] != 0) {
			hasData = true;
			break;
		}
	}

	if (hasData) {
		std::cout << "������ ���ۿ� �����Ͱ� �ֽ��ϴ�." << std::endl;
	}
	else {
		std::cout << "������ ���۰� ��� �ֽ��ϴ�." << std::endl;
	}
}
GLvoid InitBuffer()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(2, vbo);
	glGenBuffers(1, &EBO);
}

//������Ʈ�� �׸��� �Լ�
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
//������Ʈ�� ��ȯ ���
void Object::Make_Matrix() {
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "model");
	unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "view");
	unsigned int projectLocation = glGetUniformLocation(shaderProgramID, "project");
	// ��ȯ ���� ������ �����մϴ�.
	glm::mat4 modelMatrix = glm::mat4(1.0f);  // �� ��ȯ
	glm::mat4 viewMatrix = glm::mat4(1.0f);   // �� ��ȯ (ī�޶� ��ȯ)
	glm::mat4 projectionMatrix = glm::mat4(1.0f); // ���� ��ȯ (�������� ��ȯ)
	// ��ȯ ���� ������ �����մϴ�.

	modelMatrix = glm::translate(modelMatrix, glm::vec3(this->transform.x, this->transform.y, this->transform.z));

	// ���ڸ� ȸ��: ������ ��ġ���� ��ü�� ���ڸ����� ȸ����ŵ�ϴ�.
	modelMatrix = glm::rotate(modelMatrix, glm::radians(this->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));  // Y�� ȸ��
	modelMatrix = glm::rotate(modelMatrix, glm::radians(this->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));  // X�� ȸ��
	modelMatrix = glm::rotate(modelMatrix, glm::radians(this->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));  // Z�� ȸ��
	// ũ�� ��ȯ
	modelMatrix = glm::scale(modelMatrix, glm::vec3(this->scaling.x, this->scaling.y, this->scaling.z));

	viewMatrix = glm::lookAt(
		glm::vec3(camera.x, camera.y, camera.z), // ī�޶� ��ġ
		glm::vec3(camera.at_x, camera.at_y, camera.at_z), // ī�޶� �ٶ󺸴� ����
		glm::vec3(0.0f, 1.0f, 0.0f)  // ���� �� ����
	);
	// ���� ����
	projectionMatrix = glm::perspective(
		glm::radians(90.0f), // �þ߰�
		(float)WIDTH / (float)HEIGHT, // ��Ⱦ��
		0.1f, 100.0f // Ŭ�� ��� (near, far)
	);
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(projectLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
}
//������Ʈ�� ��ȯ ��ģ���ӣ�
void Object::Make_Matrix_parent() {
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "model");
	unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "view");
	unsigned int projectLocation = glGetUniformLocation(shaderProgramID, "project");
	// ��ȯ ���� ������ �����մϴ�.
	glm::mat4 modelMatrix = glm::mat4(1.0f);  // �� ��ȯ
	glm::mat4 viewMatrix = glm::mat4(1.0f);   // �� ��ȯ (ī�޶� ��ȯ)
	glm::mat4 projectionMatrix = glm::mat4(1.0f); // ���� ��ȯ (�������� ��ȯ)
	// ��ȯ ���� ������ �����մϴ�.

	modelMatrix = glm::translate(modelMatrix, glm::vec3(this->parent_transform->x, this->parent_transform->y, this->parent_transform->z));

	//��Ӱ���
	modelMatrix = glm::rotate(modelMatrix, glm::radians(this->orbit_theta), glm::vec3(0.0f, 1.0f, 0.0f));  // Y�� ȸ��

	modelMatrix = glm::translate(modelMatrix, glm::vec3(this->transform.x, this->transform.y, this->transform.z));

	// ���ڸ� ȸ��: ������ ��ġ���� ��ü�� ���ڸ����� ȸ����ŵ�ϴ�.
	modelMatrix = glm::rotate(modelMatrix, glm::radians(this->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));  // Y�� ȸ��
	modelMatrix = glm::rotate(modelMatrix, glm::radians(this->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));  // X�� ȸ��
	modelMatrix = glm::rotate(modelMatrix, glm::radians(this->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));  // Z�� ȸ��
	// ũ�� ��ȯ
	modelMatrix = glm::scale(modelMatrix, glm::vec3(this->scaling.x, this->scaling.y, this->scaling.z));



	viewMatrix = glm::lookAt(
		glm::vec3(camera.x, camera.y, camera.z), // ī�޶� ��ġ
		glm::vec3(camera.at_x, camera.at_y, camera.at_z), // ī�޶� �ٶ󺸴� ����
		glm::vec3(0.0f, 1.0f, 0.0f)  // ���� �� ����
	);
	// ���� ����
	projectionMatrix = glm::perspective(
		glm::radians(90.0f), // �þ߰�
		(float)WIDTH / (float)HEIGHT, // ��Ⱦ��
		0.1f, 100.0f // Ŭ�� ��� (near, far)
	);
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(projectLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
}
// BoundingBox ������Ʈ �Լ�
void Object::UpdateBoundingBox() {
	if (this->vertex.empty()) return;

	// �ʱⰪ ����: ù ��° ������ ��ǥ�� �������� �ʱ�ȭ
	float minX = this->vertex[0].x, minY = this->vertex[0].y, minZ = this->vertex[0].z;
	float maxX = this->vertex[0].x, maxY = this->vertex[0].y, maxZ = this->vertex[0].z;

	// �� ������ ��ȸ�ϸ� �ּ�/�ִ밪�� ã��
	for (const auto& v : this->vertex) {
		minX = std::min(minX, v.x);
		minY = std::min(minY, v.y);
		minZ = std::min(minZ, v.z);
		maxX = std::max(maxX, v.x);
		maxY = std::max(maxY, v.y);
		maxZ = std::max(maxZ, v.z);
	}

	// BoundingBox ����, transform ���� �߰��Ͽ� ���� ��ǥ�� ����
	this->hitbox.left_front_top = { minX + this->transform.x, maxY + this->transform.y, maxZ + this->transform.z };
	this->hitbox.left_front_bottom = { minX + this->transform.x, minY + this->transform.y, maxZ + this->transform.z };
	this->hitbox.right_front_top = { maxX + this->transform.x, maxY + this->transform.y, maxZ + this->transform.z };
	this->hitbox.right_front_bottom = { maxX + this->transform.x, minY + this->transform.y, maxZ + this->transform.z };

	this->hitbox.left_back_top = { minX + this->transform.x, maxY + this->transform.y, minZ + this->transform.z };
	this->hitbox.left_back_bottom = { minX + this->transform.x, minY + this->transform.y, minZ + this->transform.z };
	this->hitbox.right_back_top = { maxX + this->transform.x, maxY + this->transform.y, minZ + this->transform.z };
	this->hitbox.right_back_bottom = { maxX + this->transform.x, minY + this->transform.y, minZ + this->transform.z };
}
//������Ʈ�� ���� ����
void Object::UpdateVBO() {
	// VAO ���ε�
	glBindVertexArray(vao);

	// ���� ����
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, this->vertex.size() * sizeof(glm::vec3), this->vertex.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);

	// ��� ����
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, this->normal.size() * sizeof(glm::vec3), this->normal.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(1);

	// �ؽ��� ����
	//glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	//glBufferData(GL_ARRAY_BUFFER, this->texture.size() * sizeof(glm::vec2), this->texture.data(), GL_STREAM_DRAW);
	//glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
	//glEnableVertexAttribArray(3);

	//// �ε��� ����
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indexlist.size() * sizeof(Index), this->indexlist.data(), GL_STATIC_DRAW);


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

void Timer(int value) {
	if (mode_animation[0])// ũ���� x�� �� �̵�
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
	else if (mode_animation[1])// ũ���� x�� �¿� �̵�
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
	if (mode_animation[2]) // �߾� ��ü,�� �� y �� ȸ�� (���� ����)
	{
		for (int i = 3; i < tank.size(); i++)
		{
			tank[i].rotation.y += 1.0f;
		}
	}
	else if (mode_animation[3]) // �߾� ��ü,�� �� y �� ȸ�� (���� ����)
	{
		for (int i = 3; i < tank.size(); i++)
		{
			tank[i].rotation.y -= 1.0f;
		}
	}
	if (mode_animation[4])// ���� y �� ȸ�� (���� ����) ���� �ݴ����
	{
		tank[1].rotation.y += 1.0f;
		tank[2].rotation.y -= 1.0f;
	}
	else if (mode_animation[5])// ���� y �� ȸ�� (���� ����) ���� �ݴ����
	{
		tank[1].rotation.y -= 1.0f;
		tank[2].rotation.y += 1.0f;
	}

	if (mode_animation[6]) //���� 2�� ��ü
	{
		if (count < 50)
		{
			for (size_t i = 0; i < tank[1].vertex.size(); i++)
			{
				tank[1].vertex[i].x += 0.0035f;
				tank[2].vertex[i].x -= 0.0035f;
			}
			count++;
		}
	}
	else if (mode_animation[7]) //���� 2�� �и�
	{
		if (count > 0)
		{
			for (size_t i = 0; i < tank[1].vertex.size(); i++)
			{
				tank[1].vertex[i].x -= 0.0035f;
				tank[2].vertex[i].x += 0.0035f;
			}
			count--;
		}
	}
	if (mode_animation[8]) // ���� ���� z�࿡ ���� �������� ���� �ݴ�������� ȸ�� �ִ� 90���� 
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
	if (mode_animation[9]) // ���� ���� z�࿡ ���� �������� ���� �ݴ�������� ȸ�� �ִ� 90���� 
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
		float cx = camera.x;
		float cz = camera.z;
		camera.x = cx * cos(glm::radians(2.0f)) - cz * sin(glm::radians(2.0f));
		camera.z = cx * sin(glm::radians(2.0f)) + cz * cos(glm::radians(2.0f));
	}
	else if (camera_animation == 2)
	{
		float cx = camera.x;
		float cz = camera.z;
		camera.x = cx * cos(glm::radians(-2.0f)) - cz * sin(glm::radians(-2.0f));
		camera.z = cx * sin(glm::radians(-2.0f)) + cz * cos(glm::radians(-2.0f));
	}
	if (mode_orbit)
	{
		float vx = light.transform.x;
		float vz = light.transform.z;
		light.transform.x = vx * cos(glm::radians(-2.0f)) - vz * sin(glm::radians(-2.0f));
		light.transform.z = vx * sin(glm::radians(-2.0f)) + vz * cos(glm::radians(-2.0f));
	}
	glutTimerFunc(50, Timer, 0);
	glutPostRedisplay(); // �ٽ� �׸��� ��û
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
		mode_orbit = !mode_orbit;
		break;
	case '1':
		lightColor.x = (int)(lightColor.x + 1.0f) % 2;
		lightColor.y = (int)(lightColor.y + 1.0f) % 2;
		lightColor.z = (int)(lightColor.z + 1.0f) % 2;
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
		camera.z -= 0.1f;
		break;
	case 's':
		camera.z += 0.1f;
		break;
	case 'a':
		camera.x -= 0.1f;
		break;
	case 'd':
		camera.x += 0.1f;
		break;
	case 'x':
		camera.y -= 0.1f;
		break;
	case 32:
		camera.y += 0.1f;
		break;
	case '4': // �ʱ�ȭ
		for (size_t i = 0; i < 10; i++)
		{
			mode_animation[i] = false;
		}


		break;
	case 'q':
		exit(0);
		break;
	}
}

void mainLoop() {


	glutPostRedisplay(); // �ٽ� �׸��� ��û
}
bool is_crash(Object& objA, Object& objB) {
	// AABB ������Ʈ
	objA.UpdateBoundingBox();
	objB.UpdateBoundingBox();

	// �浹 �˻�
	// X�� ��ħ �˻�
	bool xOverlap = objA.hitbox.right_back_bottom.x >= objB.hitbox.left_back_bottom.x &&
		objA.hitbox.left_back_bottom.x <= objB.hitbox.right_back_bottom.x;

	// Y�� ��ħ �˻�
	bool yOverlap = objA.hitbox.left_front_top.y >= objB.hitbox.left_front_bottom.y &&
		objA.hitbox.left_front_bottom.y <= objB.hitbox.left_front_top.y;

	// Z�� ��ħ �˻�
	bool zOverlap = objA.hitbox.left_front_top.z >= objB.hitbox.left_back_top.z &&
		objA.hitbox.left_back_top.z <= objB.hitbox.left_front_top.z;

	// ��� �࿡�� ��ġ�� �浹 �߻�
	return xOverlap && yOverlap && zOverlap;
}

void init_figure() {
	//---------------------------cube�����-----------------------------
	tank.clear();
	read_obj_file("cube.obj", board);
	AddColors(board, 0.5f, 0.5f, 1.0f);
	read_obj_file("cube.obj", light);
	AddColors(light, 1.0f, 1.0f, 1.0f);
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
		for (size_t j = 0; j < tank[i].vertex.size(); j++)
		{
			tank[i].vertex[j].y += 0.3f;
		}
	}
	tank[4].transform.y = tank[4].vertex[0].y;
	tank[5].transform.y = tank[5].vertex[0].y;
	for (size_t j = 0; j < tank[5].vertex.size(); j++)
	{
		tank[4].vertex[j].y -= tank[4].transform.y;
		tank[5].vertex[j].y -= tank[5].transform.y;
	}

	//-----------------------------����������----------------------------
	AddColors(tank[0], 0.8f, 0.0f, 0.0f);

	AddColors(tank[1], 0.8f, 0.8f, 0.0f);
	AddColors(tank[2], 0.8f, 0.8f, 0.0f);

	AddColors(tank[3], 0.8f, 0.8f, 0.8f);

	AddColors(tank[4], 0.0f, 0.8f, 0.8f);
	AddColors(tank[5], 0.0f, 0.8f, 0.8f);
	//-------------------------��ȯ���� �Է�-------------------------
	board.scaling = { 10.0f,0.1f,10.0f };
	light.transform = { 1.0f,1.0f,1.0f };
	light.scaling = { 0.2f,0.2f,0.2f };
	light.parent_transform = &board.transform;
}
void init_camera() {
	//3��Ī ī�޶�
	camera.x = 0.0f, camera.y = 1.0f, camera.z = 2.0f;
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

	// ���� �߰�
	obj.vertex.emplace_back(glm::vec3{ -size, size, -size });
	obj.vertex.emplace_back(glm::vec3{ -size, size, size });
	obj.vertex.emplace_back(glm::vec3{ size, size, size });
	obj.vertex.emplace_back(glm::vec3{ size, size,  -size });

	obj.vertex.emplace_back(glm::vec3{ -size,  -size,  -size });
	obj.vertex.emplace_back(glm::vec3{ -size,  -size,  +size });
	obj.vertex.emplace_back(glm::vec3{ +size,  -size,  +size });
	obj.vertex.emplace_back(glm::vec3{ +size,  -size,  -size });

	// ���� �߰�
	AddColors(obj, 0.0f, 1.0f, 1.0f);

	//�ε�������Ʈ �߰�
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

	// ���� �߰�
	AddColors(obj, 1.0f, 0.5f, 0.5f);

	// �ε��� ����Ʈ �߰�
	obj.indexlist.emplace_back(Index{ 0, 1, 2 });
	obj.indexlist.emplace_back(Index{ 0, 2, 3 });
	obj.indexlist.emplace_back(Index{ 0, 3, 1 });
	obj.indexlist.emplace_back(Index{ 1, 2, 3 });
}
void Make_Board(Object& obj, float size) {
	obj.vertex.emplace_back(glm::vec3{ size,0.0f,size });
	obj.vertex.emplace_back(glm::vec3{ size,0.0f,-size });
	obj.vertex.emplace_back(glm::vec3{ -size,0.0f,-size });
	obj.vertex.emplace_back(glm::vec3{ size,0.0f,size });
	obj.vertex.emplace_back(glm::vec3{ -size,0.0f,-size });
	obj.vertex.emplace_back(glm::vec3{ -size,0.0f,size });
	obj.facecnt = 2;
	AddColors(obj, 0.8f, 0.8f, 1.0f);
	std::cout << obj.vertex.size() << "���� ������ ����\n";
	for (unsigned int i = 0; i < obj.vertex.size(); i++)
	{
		obj.normal.emplace_back(glm::vec3{ 0.0f, 1.0f,0.0f });
	}
}

void AddColors(Object& fig, float r, float g, float b) {
	fig.color.clear();
	// ���� ������
	for (int i = 0; i < fig.vertex.size(); i++) {
		fig.color.emplace_back(glm::vec3{ r,g,b });
	}
}
void AddColors_Indexlist(Object& fig, float r, float g, float b) {
	fig.color.clear();
	fig.indexlist.clear();
	// �ε��� �߰� (�ﰢ���� �̷�� ������� �߰�)
	for (unsigned int i = 1; i < fig.vertex.size() - 1; i++) {
		fig.indexlist.emplace_back(Index{ 0, i, i + 1 });
	}
	// ���� ������
	for (int i = 0; i < fig.vertex.size(); i++) {
		fig.color.emplace_back(glm::vec3{ r,g,b });
	}
}

GLvoid Reshape(int w, int h)
{
	// ����Ʈ �⺻ WIDTH HEIGHT�� ����
	glViewport(0, 0, w, h);
}

char* GetBuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;

	// ���� �б� �������� ����
	fopen_s(&fptr, file, "rb");

	// ����ó��
	if (!fptr) return NULL;

	// ���� buf�� ��ȯ
	fseek(fptr, 0, SEEK_END);
	length = ftell(fptr);
	buf = (char*)malloc(length + 1);
	fseek(fptr, 0, SEEK_SET);
	fread(buf, length, 1, fptr);

	// ���� ����
	fclose(fptr);

	buf[length] = 0;
	return buf;
}

GLint CreateShader(const char* file, int type)
{
	GLchar* source = GetBuf(file);

	// ��ü ����
	GLint shader = glCreateShader(type);
	glShaderSource(shader, 1, (const GLchar**)&source, 0);
	glCompileShader(shader);

	// ������ ���� üũ
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

	if (!result)
	{
		glGetShaderInfoLog(shader, 512, NULL, errorLog);
		std::cerr << "ERROR: ������ ����\n" << errorLog << std::endl;
		return 0;
	}

	return shader;
}

void CreateShaderProgram()
{
	vertexShader = CreateShader("vertex.vs", GL_VERTEX_SHADER);
	fragmentShader = CreateShader("fragment.fs", GL_FRAGMENT_SHADER);

	// ���̴� ���α׷� ����
	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glLinkProgram(shaderProgramID);

	// ���̴� ����
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// ���̴� ���α׷� ���
	glUseProgram(shaderProgramID);
}

// OBJ ���� �б� �Լ�
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
		read_newline(line); // ���๮�� ����

		// ����(Vertex) ������ ó��
		if (line[0] == 'v' && line[1] == ' ') {
			glm::vec3 vertex;
			sscanf_s(line + 2, "%f %f %f", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		// ����(Vertex) ������ ó��
		if (line[0] == 'v' && line[1] == 'n') {
			glm::vec3 normal;
			sscanf_s(line + 2, "%f %f %f", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		// ����(Vertex) ������ ó��
		if (line[0] == 'v' && line[1] == 't') {
			glm::vec2 texture;
			sscanf_s(line + 2, "%f %f", &texture.x, &texture.y);
			temp_textures.push_back(texture);
		}
		// ��(Face) ������ ó��
		else if (line[0] == 'f' && line[1] == ' ') {
			unsigned int v1, v2, v3;
			unsigned int vt1, vt2, vt3; // �ؽ�ó ��ǥ �ε���
			unsigned int vn1, vn2, vn3; // ���� ���� �ε���
			sscanf_s(line + 2, "%u/%u/%u %u/%u/%u %u/%u/%u",
				&v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3);

			// OBJ �ε����� 1���� �����ϱ� ������ 0���� �����ϵ��� ����
			//Index face = { v1 - 1, v2 - 1,v3 - 1 };
			//temp_indices.push_back(face);
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

	// Object ����ü�� ���� �� �ε��� �����͸� ����
	model.vertex = temp_vertices2; // ���� �����͸� �״�� ����
	model.normal = temp_normals2; // �ε��� �����͸� �״�� ����
	model.texture = temp_textures; // �ε��� �����͸� �״�� ����
	model.indexlist = temp_indices; // �ε��� �����͸� �״�� ����

	// ������ ���Ƿ� ���� (���� ���� ����)
	AddColors(model, 0.5f, 0.5f, 1.0f);

	model.transform = { 0.0f,0.0f,0.0f };
	model.scaling = { 1.0f,1.0f,1.0f };
	model.rotation = { 0.0f,0.0f,0.0f };
}