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
vector<Object> robot{}, cube{}, block{}, ground{}, cylinder{},mini1,mini2,mini3;
Coordinate Coordinate_system{};
FreeCamera camera{};

GLuint vao;
GLuint vbo[2];
GLuint EBO;

//�ǽ��� �ʿ��� ������
int rcnt{};
bool mode_open{ false };
bool camera_animation{ false };
int count{};

float r = 1.0f;
int count_for_sec{};
float leg_size[7]{};
float mini_size[3][7]{};
float block_size{ 0.5f };
float cylinder_size = 0.5f;
float ground_size = 0.5f;
float gravity = 0.005f;
XYZ tps = { 0.0f, 0.15f,-0.1f };

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
	//glEnable(GL_CULL_FACE);

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
	//ť�� �׸���
	for (int i = 0; i < cube.size(); i++)
	{
		cube[i].Draw_object();
	}
	//ť�� �׸���
	for (int i = 0; i < ground.size(); i++)
	{
		ground[i].Draw_object();
	}
	//��ֹ� �׸���
	for (int i = 0; i < block.size(); i++)
	{
		block[i].Draw_object();
	}
	//�κ� �׸���
	for (int i = 0; i < cylinder.size(); i++)
	{
		cylinder[i].Draw_object();
	}
	//�κ� �׸���
	for (int i = 0; i < robot.size(); i++)
	{
		robot[i].Draw_object();
	}
	for (int i = 0; i < robot.size(); i++)
	{
		mini1[i].Draw_object();
	}
	for (int i = 0; i < robot.size(); i++)
	{
		mini2[i].Draw_object();
	}
	for (int i = 0; i < robot.size(); i++)
	{
		mini3[i].Draw_object();
	}

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

	// ť�� ��ġ ���� ���ε�
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, this->vertex.size() * sizeof(glm::vec3), this->vertex.data(), GL_STREAM_DRAW);

	// ť�� �ε��� ���� ���ε�
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// ��ü �ε��� �迭�� ũ�� ����
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indexlist.size() * sizeof(Index), this->indexlist.data(), GL_DYNAMIC_DRAW);

	// ��ġ �Ӽ� ����
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(0);

	// ť�� ���� ���� ���ε�
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, this->color.size() * sizeof(glm::vec3), this->color.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(1);
}

GLvoid UpdateVBO(Object object) {
	// VAO ���ε�
	glBindVertexArray(vao);

	// ť�� ��ġ ���� ���ε�
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, object.vertex.size() * sizeof(glm::vec3), object.vertex.data(), GL_DYNAMIC_DRAW);

	// ť�� �ε��� ���� ���ε�
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// ��ü �ε��� �迭�� ũ�� ����
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, object.indexlist.size() * sizeof(Index), object.indexlist.data(), GL_DYNAMIC_DRAW);

	// ��ġ �Ӽ� ����
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(0);

	// ť�� ���� ���� ���ε�
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, object.color.size() * sizeof(glm::vec3), object.color.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(1);
}
GLvoid UpdateVBO(Coordinate object) {
	// VAO ���ε�
	glBindVertexArray(vao);

	// ��ǥ�� ���� ���� ���ε�
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, object.vertex.size() * sizeof(glm::vec3), object.vertex.data(), GL_DYNAMIC_DRAW);

	// ��ǥ�� �ε��� ���� ���ε�
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, object.indexlist.size() * sizeof(unsigned int), object.indexlist.data(), GL_DYNAMIC_DRAW);

	// ��ġ �Ӽ� ����
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(0);

	// ��ǥ�� ���� ���� ���ε�
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, object.color.size() * sizeof(glm::vec3), object.color.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(1);
}

void Timer(int value) {
	for (int i = 0; i < rcnt; i++)
	{
		robot[i].last_transform.insert(robot[i].last_transform.begin(), robot[i].transform);
		robot[i].last_rotation.insert(robot[i].last_rotation.begin(), robot[i].rotation);
		if (robot[i].last_transform.size() >= 4)
		{
			mini1[i].transform = robot[i].last_transform[3];
			mini1[i].rotation = robot[i].last_rotation[3];
			mini1[i].transform.y -= mini_size[2][i];
		}
		if (robot[i].last_transform.size() >= 8)
		{
			mini2[i].transform = robot[i].last_transform[7];
			mini2[i].rotation = robot[i].last_rotation[7];
			mini2[i].transform.y -= mini_size[1][i];
		}
		if (robot[i].last_transform.size() >= 12)
		{
			mini3[i].transform = robot[i].last_transform[11];
			mini3[i].rotation = robot[i].last_rotation[11];
			mini3[i].transform.y -= mini_size[0][i];
		}
		if (robot[i].last_transform.size() > 12)
		{
			robot[i].last_transform.pop_back();
			robot[i].last_rotation.pop_back();
		}
	}



	//�̵� �浹�˻�
	if (robot[0].dir_x != 0.0f || robot[0].dir_z != 0.0f) {
		bool turn_x{ false }, turn_z{ false };
		RobotMove(1);
		if (!is_inMap())
		{
			turn_z = true;
		}
		RobotMove(2);
		RobotMove(3);
		if (!is_inMap())
		{
			turn_x = true;
		}
		RobotMove(4);
		if (turn_x)
		{
			robot[0].dir_x *= -1;
		}
		if (turn_z)
		{
			robot[0].dir_z *= -1;
		}
		turn_x = turn_z = false;
		//��ֹ��� xz��� �˻�
		if (!robot[0].is_jump)
		{
			// ��ֹ��� z�� �̵� �˻�
			RobotMove(1);
			for (size_t i = 0; i < block.size(); i++)
			{
				if (is_crash(robot[5], block[i])) // �浹�� �߻��ߴ��� Ȯ��
				{
					float robotBottomY = robot[5].vertex[0].y;
					for (const auto& v : robot[5].vertex) {
						robotBottomY = std::min(robotBottomY, v.y);
					}
					robotBottomY += robot[5].transform.y;

					if (robotBottomY < block_size / 20)
					{
						turn_z = true;
					}
				}
			}
			RobotMove(2);

			// ��ֹ��� x�� �̵� �˻�
			RobotMove(3);
			for (size_t i = 0; i < block.size(); i++)
			{
				if (is_crash(robot[5], block[i])) // �浹�� �߻��ߴ��� Ȯ��
				{
					float robotBottomY = robot[5].vertex[0].y;
					for (const auto& v : robot[5].vertex) {
						robotBottomY = std::min(robotBottomY, v.y);
					}
					robotBottomY += robot[5].transform.y;

					if (robotBottomY < block_size / 20)
					{
						turn_x = true;
					}
				}
			}
			RobotMove(4);
			if (turn_x)
			{
				robot[0].dir_x *= -1;
			}
			if (turn_z)
			{
				robot[0].dir_z *= -1;
			}
		}
		
		turn_x = turn_z = false;
		//����հ� �˻�
		RobotMove(1);
		for (int i = 0; i < cylinder.size(); i++)
		{
			if (is_crash(robot[5], cylinder[i])) // �浹�� �߻��ߴ��� Ȯ��
			{
				float robotBottomY = robot[5].vertex[0].y;
				for (const auto& v : robot[5].vertex) {
					robotBottomY = std::min(robotBottomY, v.y);
				}
				robotBottomY += robot[5].transform.y;
				if (robotBottomY < block_size / 20)
				{
					turn_z = true;
				}
			}
		}
		RobotMove(2);
		RobotMove(3);
		for (int i = 0; i < cylinder.size(); i++)
		{
			if (is_crash(robot[5], cylinder[i])) // �浹�� �߻��ߴ��� Ȯ��
			{
				float robotBottomY = robot[5].vertex[0].y;
				for (const auto& v : robot[5].vertex) {
					robotBottomY = std::min(robotBottomY, v.y);
				}
				robotBottomY += robot[5].transform.y;
				if (robotBottomY < block_size / 20)
				{
					turn_x = true;
				}
			}
		}
		RobotMove(4);
		if (turn_x)
		{
			robot[0].dir_x *= -1;
		}
		if (turn_z)
		{
			robot[0].dir_z *= -1;
		}
	}
	// �̵� ����
	RobotMove(1);
	RobotMove(3);
	//ĳ���� ȸ��
	float angle = (180.0f / PI) * atan2(robot[0].dir_x, robot[0].dir_z);
	for (size_t i = 0; i < rcnt; i++)
	{
		robot[i].rotation.y = angle;
	}
	//�̵� �ִϸ��̼� ���� 
	if (robot[0].dir_x != 0.0f || robot[0].dir_z != 0.0f)
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
	if (r != 0.0f)
	{
		robot[2].rotation.x -= 4 * (robot[0].speed / 0.02f) * r;
		robot[3].rotation.x += 4 * (robot[0].speed / 0.02f) * r;
		robot[4].rotation.x += 4 * (robot[0].speed / 0.02f) * r;
		robot[5].rotation.x -= 4 * (robot[0].speed / 0.02f) * r;
	}

	//���� ����
	if (robot[0].is_jump)
	{
		if (robot[0].flight_time < 50)
		{
			robot[0].flight_time++;
		}
		for (int i = 0; i < rcnt; i++)
		{
			robot[i].transform.y += robot[0].speed;
		}
	}

	on_land();
	on_block();

	//�� ���� �ִϸ��̼�
	if (mode_open)
	{
		for (size_t i = 0; i < 4; i++)
		{
			if (cube[0].vertex[0].x > -2.0f)
			{
				cube[0].vertex[i].x -= 0.05f;
				cube[1].vertex[i].x += 0.05f;
			}
		}
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
	case 'w':
		if (r == 0.0f)
		{
			r = 1.0f;
		}
		robot[0].dir_z = -1.0f;
		break;
	case 's':
		if (r == 0.0f)
		{
			r = 1.0f;
		}
		robot[0].dir_z = 1.0f;
		break;
	case 'a':
		if (r == 0.0f)
		{
			r = -1.0f;
		}
		robot[0].dir_x = -1.0f;
		break;
	case 'd':
		if (r == 0.0f)
		{
			r = -1.0f;
		}
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
	case 'z':
		camera.z -= 0.1f;
		break;
	case 'Z':
		camera.z += 0.1f;
		break;
	case 'x':
		camera.x -= 0.1f;
		break;
	case 'X':
		camera.x += 0.1f;
		break;
	case 'y':case 'Y':
		camera_animation = !camera_animation;
		break;
	case 'v':
		camera.y -= 0.1f;
		break;
	case 'V':
		camera.y += 0.1f;
		break;
	case 'o':case 'O':
		mode_open = true;
		break;
	case 32: // ����
		robot[0].is_jump = true;
		break;
	case '4': // �ʱ�ȭ
		init_figure();
		mode_open = false;
		camera_animation = 0;
		break;
	case 'c': case 'C':
		init_figure();
		break;
	case 'q':
		exit(0);
		break;
	}
}

void mainLoop() {


	glutPostRedisplay(); // �ٽ� �׸��� ��û
}

bool is_inMap() {
	if (robot[5].transform.x > 2.0f || robot[5].transform.x < -2.0f || robot[5].transform.z < -2.0f || robot[5].transform.z > 2.0f)
	{
		return false;
	}
	return true;
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
void on_land() {
	// �κ��� �ϴܺ� �� ���ϱ�
	float robotBottomY = robot[5].vertex[0].y;
	for (const auto& v : robot[5].vertex) {
		robotBottomY = std::min(robotBottomY, v.y);
	}
	robotBottomY += robot[5].transform.y; // �κ� �ϴܺ��� ���� ��ǥ
	Apply_Gravity(1);
	for (int i = 0; i < ground.size(); i++)
	{
		if (is_crash(ground[i], robot[5]) && robotBottomY < block_size / 20) {
			for (int j = 0; j < rcnt; j++)
			{
				robot[j].transform.y = leg_size[j];
			}
			robot[0].is_jump = false;
			robot[0].flight_time = 0;
		}
	}
}
void on_block() {
	// �κ��� �ϴܺ� �� ���ϱ�
	float robotBottomY = robot[5].vertex[0].y;
	for (const auto& v : robot[5].vertex) {
		robotBottomY = std::min(robotBottomY, v.y);
	}
	robotBottomY += robot[5].transform.y; // �κ� �ϴܺ��� ���� ��ǥ

	// ���� ���� �ִ��� Ȯ��
	for (int i = 0; i < block.size(); i++)
	{
		float blockTop{ block[i].vertex[0].y };
		for (const auto& v : block[i].vertex) {
			blockTop = std::max(blockTop, v.y);
		}
		blockTop += block[i].transform.y;
		if (is_crash(block[i], robot[5])) {
			if (blockTop > 0.0f)
			{
				block[i].transform.y -= 0.001f;
			}
			for (int j = 0; j < rcnt; j++)
			{
				robot[j].transform.y = leg_size[j] + blockTop;
			}
			robot[0].is_jump = false;
			robot[0].flight_time = 0;
		}
		if (blockTop <= 0.001f)
		{
			block.erase(block.begin() + i);
			i--;
		}
	}
}
void RobotMove(int dir) {
	switch (dir)
	{
	case 1:
		for (int i = 0; i < rcnt; i++)
		{
			robot[i].transform.x += robot[0].dir_z * robot[0].speed * camera.forward.x;
			robot[i].transform.z += robot[0].dir_z * robot[0].speed * camera.forward.z;
		}
		break;
	case 2:
		for (int i = 0; i < rcnt; i++)
		{
			robot[i].transform.x -= robot[0].dir_z * robot[0].speed * camera.forward.x;
			robot[i].transform.z -= robot[0].dir_z * robot[0].speed * camera.forward.z;
		}
		break;
	case 3:
		for (int i = 0; i < rcnt; i++)
		{
			robot[i].transform.x += robot[0].dir_x * robot[0].speed * camera.right.x;
			robot[i].transform.z += robot[0].dir_x * robot[0].speed * camera.right.z;
		}
		break;
	case 4:
		for (int i = 0; i < rcnt; i++)
		{
			robot[i].transform.x -= robot[0].dir_x * robot[0].speed * camera.right.x;
			robot[i].transform.z -= robot[0].dir_x * robot[0].speed * camera.right.z;
		}
		break;
	default:
		break;
	}
}
void Apply_Gravity(int dir) {
	if (dir == 1)
	{
		if (robot[5].transform.y > leg_size[5] + block_size * 2 / 3)
		{
			for (int i = 0; i < rcnt; i++)
			{
				robot[i].transform.y -= gravity * (robot[0].flight_time + 2);
			}
		}
		else
		{
			for (int i = 0; i < rcnt; i++)
			{
				robot[i].transform.y -= gravity * (robot[0].flight_time + 1);
			}
		}
	}
	else if (dir == -1)
	{
		for (int i = 0; i < rcnt; i++)
		{
			robot[i].transform.y += gravity * robot[0].flight_time;
		}
	}
}
void init_figure() {
	//---------------------------cube�����-----------------------------
	cube.clear();
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
	Make_cube_Left(c, cube_size);
	cube.emplace_back(c);
	Make_cube_Right(c, cube_size);
	cube.emplace_back(c);
	//---------------------------obj ����-----------------------------
	robot.clear();
	Object objfile{};
	read_obj_file("robot_body.obj", objfile);
	robot.emplace_back(objfile);
	read_obj_file("robot_head.obj", objfile);
	robot.emplace_back(objfile);
	//�� obj
	read_obj_file("robot_left_arm.obj", objfile);
	robot.emplace_back(objfile);
	read_obj_file("robot_right_arm.obj", objfile);
	robot.emplace_back(objfile);
	//�ٸ� obj
	read_obj_file("robot_left_leg.obj", objfile);
	robot.emplace_back(objfile);
	read_obj_file("robot_right_leg.obj", objfile);
	robot.emplace_back(objfile);
	//�κ� ��
	read_obj_file("robot_face.obj", objfile);
	robot.emplace_back(objfile);
	rcnt = robot.size();
	//-----------------------------����������----------------------------
	AddColors(robot[0], 0.8f, 0.0f, 0.0f);
	AddColors(robot[1], 0.8f, 0.8f, 0.0f);

	AddColors(robot[2], 0.8f, 0.8f, 0.8f);
	AddColors(robot[3], 0.8f, 0.8f, 0.8f);

	AddColors(robot[4], 0.2f, 0.2f, 0.2f);
	AddColors(robot[5], 0.2f, 0.2f, 0.2f);
	AddColors(robot[6], 0.0f, 0.0f, 0.0f);
	//-----------------------------mini�κ� ����---------------------------
	mini1 = robot;
	mini2 = robot;
	mini3 = robot;
	for (int i = 0; i < rcnt; i++)
	{
		size_t vcnt = robot[i].vertex.size();
		for (size_t j = 0; j < vcnt; j++)
		{
			mini1[i].vertex[j] *= 0.7f;
			mini2[i].vertex[j] *= 0.5f;
			mini3[i].vertex[j] *= 0.3f;
		}
	}
	//-----------------------------�κ� ���庯ȯ ���� ����---------------------------
	for (size_t i = 0; i < rcnt; i++)
	{
		robot[i].transform.y = robot[i].vertex[2].y;
		mini1[i].transform.y = mini1[i].vertex[2].y;
		mini2[i].transform.y = mini2[i].vertex[2].y;
		mini3[i].transform.y = mini3[i].vertex[2].y;
		size_t vcnt = robot[i].vertex.size();
		for (size_t j = 0; j < vcnt; j++)
		{
			robot[i].vertex[j].y -= robot[i].transform.y;
			mini1[i].vertex[j].y -= mini1[i].transform.y;
			mini2[i].vertex[j].y -= mini2[i].transform.y;
			mini3[i].vertex[j].y -= mini3[i].transform.y;
		}
		leg_size[i] = robot[i].transform.y;
		mini_size[0][i] = mini1[i].transform.y;
		mini_size[1][i] = mini2[i].transform.y;
		mini_size[2][i] = mini3[i].transform.y;
	}
	robot[0].speed = 0.04f;

	//-----------------------------�� ����---------------------------
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			Make_cube_bottom(c, ground_size);
			c.transform = { -1.5f + 1.0f * j,ground_size,-1.5f + 1.0f * i };
			AddColors(c, 0.5f, 0.5f, 1.0f * ((j+i) % 2));
			ground.emplace_back(c);
		}
	}
	//-----------------------------����� ����---------------------------
	cylinder.clear();
	read_obj_file("cylinder.obj", objfile);
	objfile.transform = { 1.5f,0.0f,1.5f };
	cylinder.emplace_back(objfile);
	objfile.transform = { 1.5f,0.0f,-1.5f };
	cylinder.emplace_back(objfile);
	objfile.transform = { -1.5f,0.0f,-1.5f };
	cylinder.emplace_back(objfile);
	objfile.transform = { -1.5f,0.0f,1.5f };
	cylinder.emplace_back(objfile);
	for (int i = 0; i < cylinder.size(); i++)
	{
		int cvcnt = cylinder[i].vertex.size();
		for (int j = 0; j < cvcnt; j++)
		{
			cylinder[i].vertex[j].x /= 2;
			cylinder[i].vertex[j].z /= 2;
		}
	}
	//-----------------------------��ֹ� ����---------------------------
	block.clear();
	for (size_t i = 0; i < 3; i++)
	{
		Make_Block(c, block_size);
		c.transform = { (rand() % 4)  - 1.5f,0.0f,(rand() % 4) * 1.0f - 1.5f };
		block.emplace_back(c);
	}

	//-------------------------��ȯ���� �Է�-------------------------
}
void init_camera() {
	//3��Ī ī�޶�
	camera.x = 0.0f, camera.y = 1.5f, camera.z = 2.5f;
	camera.at_x = 0.0f, camera.at_y = 0.5f, camera.at_z = 0.0f;
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
	obj.vertex.emplace_back(glm::vec3{ -size,-size ,size });
	obj.vertex.emplace_back(glm::vec3{ -size,-size ,-size });
	obj.vertex.emplace_back(glm::vec3{ -size,size ,-size });
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
	obj.vertex.emplace_back(glm::vec3{ size,-size ,-size });
	obj.vertex.emplace_back(glm::vec3{ -size,-size ,-size });
	obj.vertex.emplace_back(glm::vec3{ -size,size ,-size });
	AddColors_Indexlist(obj, 0.0f, 1.0f, 1.0f);
	obj.transform = { 0.0f ,size ,0.0f };
	obj.scaling = { 1.0f ,1.0f ,1.0f };
	obj.rotation = { 0.0f ,0.0f ,0.0f };
}

void Make_Block(Object& obj, float size) {
	obj.vertex.clear();
	obj.color.clear();
	obj.indexlist.clear();

	// ���� �߰�
	obj.vertex.emplace_back(glm::vec3{ -size , size/10, -size  });
	obj.vertex.emplace_back(glm::vec3{ -size , size/10, size  });
	obj.vertex.emplace_back(glm::vec3{ size , size/10, size  });
	obj.vertex.emplace_back(glm::vec3{ size , size/10, -size  });

	obj.vertex.emplace_back(glm::vec3{ -size , 0, -size  });
	obj.vertex.emplace_back(glm::vec3{ -size , 0, size  });
	obj.vertex.emplace_back(glm::vec3{ size , 0, size  });
	obj.vertex.emplace_back(glm::vec3{ size , 0, -size  });

	// ���� �߰�
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

void Make_Ground(Object& obj, float size) {
	obj.vertex.clear();
	float angle{};
	for (int i = 0; i < 8; i++)
	{
		obj.vertex.emplace_back(glm::vec3{ size * cos(glm::radians(45.0 * i)),0.0,size * sin(glm::radians(45.0 * i)) });
	}
	for (int i = 0; i < 8; i++)
	{
		obj.vertex.emplace_back(glm::vec3{ size * cos(glm::radians(45.0 * i)),3 * size,size * sin(glm::radians(45.0 * i)) });
	}
	AddColors_Indexlist(obj, 1.0f, 0.6f, 1.0f);
	obj.transform = { 0.0f ,size ,0.0f };
	obj.scaling = { 1.0f ,1.0f ,1.0f };
	obj.rotation = { 0.0f ,0.0f ,0.0f };
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

void Draw_Coordinate(Coordinate obj) {
	obj.vertex.clear();
	obj.color.clear();
	obj.indexlist.clear();

	// ���� �߰�
	obj.vertex.emplace_back(glm::vec3{ -5.0f, 0.0f, 0.0f });  // X��
	obj.vertex.emplace_back(glm::vec3{ 5.0f, 0.0f, 0.0f });  // X��
	obj.vertex.emplace_back(glm::vec3{ 0.0f, -5.0f, 0.0f });  // Y��
	obj.vertex.emplace_back(glm::vec3{ 0.0f, 5.0f, 0.0f });  // Y��
	obj.vertex.emplace_back(glm::vec3{ 0.0f, 0.0f, -5.0f });  // Z��
	obj.vertex.emplace_back(glm::vec3{ 0.0f, 0.0f, 5.0f });  // Z��

	// ���� �߰� (�� ���� ���� ����)
	obj.color.emplace_back(glm::vec3{ 0.0f, 0.0f, 0.0f });  // X��: ����
	obj.color.emplace_back(glm::vec3{ 1.0f, 0.0f, 0.0f });  // X��: ����
	obj.color.emplace_back(glm::vec3{ 0.0f, 0.0f, 0.0f });  // Y��: �Ķ�
	obj.color.emplace_back(glm::vec3{ 0.0f, 0.0f, 1.0f });  // Y��: �Ķ�
	obj.color.emplace_back(glm::vec3{ 0.0f, 0.0f, 0.0f });  // Z��: �ʷ�
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 0.0f });  // Z��: �ʷ�


	// �ε��� �߰�
	obj.indexlist.emplace_back(0);  // X��
	obj.indexlist.emplace_back(1);

	obj.indexlist.emplace_back(2);  // Y��
	obj.indexlist.emplace_back(3);

	obj.indexlist.emplace_back(4);  // Z��
	obj.indexlist.emplace_back(5);

	// VBO ������Ʈ
	//UpdateVBO(obj);

	// �ε����� ������ ��ǥ���� ������ 3���̹Ƿ� �� 6��
	glDrawElements(GL_LINES, obj.indexlist.size(), GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
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
		read_newline(line); // ���๮�� ����

		// ����(Vertex) ������ ó��
		if (line[0] == 'v' && line[1] == ' ') {
			glm::vec3 vertex;
			sscanf_s(line + 2, "%f %f %f", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}

		// ��(Face) ������ ó��
		else if (line[0] == 'f' && line[1] == ' ') {
			unsigned int v1, v2, v3;
			unsigned int vt1, vt2, vt3; // �ؽ�ó ��ǥ �ε���
			unsigned int vn1, vn2, vn3; // ���� ���� �ε���
			sscanf_s(line + 2, "%u/%u/%u %u/%u/%u %u/%u/%u",
				&v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3);

			// OBJ �ε����� 1���� �����ϱ� ������ 0���� �����ϵ��� ����
			Index face = { v1 - 1, v2 - 1, v3 - 1 };
			temp_indices.push_back(face);
		}
	}

	fclose(file);

	// Object ����ü�� ���� �� �ε��� �����͸� ����
	model.vertex = temp_vertices; // ���� �����͸� �״�� ����
	model.indexlist = temp_indices; // �ε��� �����͸� �״�� ����

	// ������ ���Ƿ� ���� (���� ���� ����)
	for (size_t i = 0; i < model.vertex.size(); i++) {
		model.color.push_back(glm::vec3(1.0f, 1.0f, 1.0f)); // ������� ����
	}
	model.transform = { 0.0f,0.0f,0.0f };
	model.scaling = { 1.0f,1.0f,1.0f };
	model.rotation = { 0.0f,0.0f,0.0f };
}
