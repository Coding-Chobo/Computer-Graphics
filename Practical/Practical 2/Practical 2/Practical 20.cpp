#define _CRT_SECURE_NO_WARNINGS
#include "3DGraphics.h"

#define WIDTH 1200
#define HEIGHT 900
#define PI 3.1415926535897932384620588419716939
#define MAX_LINE_LENGTH 1024
GLchar* vertexSource, * fragmentSource; // �ҽ��ڵ� ���� ����
GLuint vertexShader, fragmentShader; //���̴� ��ü
GLuint shaderProgramID; // ���̴� ���α׷�

//�׷��� ������Ʈ ����
Object board{};
vector<Object> tank{};
Coordinate Coordinate_system{};
Camera_Info camera1{}, camera2{}, camera3{};

GLuint vao;
GLuint vbo[2];
GLuint EBO;

//�ǽ��� �ʿ��� ������
unsigned int shape{ 0 };
bool mode_animation[10]{ false };
unsigned int camera_animation{ false };
int count{};
int viewID = 1;
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
	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	InitBuffer();
	CreateShaderProgram();
	//���� ���� �ʱ�ȭ

	init_figure(size);
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
	// �� location �ε��� ����
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "trans");
	// ��ȯ ���� ������ �����մϴ�.
	glm::mat4 modelMatrix = glm::mat4(1.0f); // �� ��ȯ
	glm::mat4 viewMatrix = glm::mat4(1.0f);  // �� ��ȯ (ī�޶� ��ȯ)
	glm::mat4 projectionMatrix = glm::mat4(1.0f); // ���� ��ȯ (�������� ��ȯ)

	// �� ��ȯ
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));

	// ī�޶� ����: ī�޶� ��ġ, ī�޶� �ٶ󺸴� ��ġ, ���� �� ����
	if (viewID == 1)
	{
		viewMatrix = glm::lookAt(
			glm::vec3(camera1.x, camera1.y, camera1.z), // ī�޶� ��ġ
			glm::vec3(camera1.at_x, camera1.at_y, camera1.at_z), // ī�޶� �ٶ󺸴� ����
			glm::vec3(0.0f, 1.0f, 0.0f)  // ���� �� ����
		);
	}
	else if (viewID == 2)
	{
		viewMatrix = glm::lookAt(
			glm::vec3(camera2.x, camera2.y, camera2.z), // ī�޶� ��ġ
			glm::vec3(camera2.at_x, camera2.at_y, camera2.at_z), // ī�޶� �ٶ󺸴� ����
			glm::vec3(0.0f, 1.0f, 0.0f)  // ���� �� ����
		);
	}
	else if (viewID == 3)
	{
		viewMatrix = glm::lookAt(
			glm::vec3(camera3.x, camera3.y, camera3.z), // ī�޶� ��ġ
			glm::vec3(camera3.at_x, camera3.at_y, camera3.at_z), // ī�޶� �ٶ󺸴� ����
			glm::vec3(0.0f, 1.0f, 0.0f)  // ���� �� ����
		);
	}

	// ���� ����: FOV, ��Ⱦ��, near, far Ŭ�� �Ÿ�
	projectionMatrix = glm::perspective(
		glm::radians(45.0f), // �þ߰�
		(float)WIDTH / (float)HEIGHT, // ��Ⱦ��
		0.1f, 50.0f // Ŭ�� ��� (near, far)
	);


	glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix; // MVP ��ȯ


	// �� ��ȯ ����
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

}

void Object::Draw_object() {
	Make_Matrix();
	UpdateVBO();
	glDrawElements(GL_TRIANGLES, 3 * this->indexlist.size(), GL_UNSIGNED_INT, 0);
}

//������Ʈ�� ��ȯ ���
void Object::Make_Matrix() {
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "trans");
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
	if (viewID == 1)
	{
		viewMatrix = glm::lookAt(
			glm::vec3(camera1.x, camera1.y, camera1.z), // ī�޶� ��ġ
			glm::vec3(camera1.at_x, camera1.at_y, camera1.at_z), // ī�޶� �ٶ󺸴� ����
			glm::vec3(0.0f, 1.0f, 0.0f)  // ���� �� ����
		);
	}
	else if (viewID == 2)
	{
		viewMatrix = glm::lookAt(
			glm::vec3(camera2.x, camera2.y, camera2.z), // ī�޶� ��ġ
			glm::vec3(camera2.at_x, camera2.at_y, camera2.at_z), // ī�޶� �ٶ󺸴� ����
			glm::vec3(0.0f, 1.0f, 0.0f)  // ���� �� ����
		);
	}
	else if (viewID == 3)
	{
		viewMatrix = glm::lookAt(
			glm::vec3(camera3.x, camera3.y, camera3.z), // ī�޶� ��ġ
			glm::vec3(camera3.at_x, camera3.at_y, camera3.at_z), // ī�޶� �ٶ󺸴� ����
			glm::vec3(0.0f, 1.0f, 0.0f)  // ���� �� ����
		);
	}
	// ���� ����
	projectionMatrix = glm::perspective(
		glm::radians(90.0f), // �þ߰�
		(float)WIDTH / (float)HEIGHT, // ��Ⱦ��
		0.1f, 100.0f // Ŭ�� ��� (near, far)
	);

	// ���� ��ȯ ���(MVP)�� ����Ͽ� GPU�� ����
	glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
}

void RenderScene() {
	// ��ǥ�� �׸���
	init_Matrix();
	Draw_Coordinate(Coordinate_system);
	// ���� �׸���
	board.Draw_object();
	for (int i = 0; i < tank.size(); i++) {
		tank[i].Draw_object();
	}
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
			for (size_t i = 0; i < 8; i++)
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
			for (size_t i = 0; i < 8; i++)
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
	case 'v': // ������ a
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

	glutPostRedisplay(); // �ٽ� �׸��� ��û
}

void init_figure(float size) {
	//---------------------------obj����-----------------------------
	tank.clear();
	Make_Board(board, 2.5f);
	//-----------------���� ��ȯ ��ǥ �Է�--------------------------

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

	//-----------------------------����������----------------------------
	AddColors(tank[0], 0.8f, 0.0f, 0.0f);

	AddColors(tank[1], 0.8f, 0.8f, 0.0f);
	AddColors(tank[2], 0.8f, 0.8f, 0.0f);

	AddColors(tank[3], 0.8f, 0.8f, 0.8f);

	AddColors(tank[4], 0.0f, 0.8f, 0.8f);
	AddColors(tank[5], 0.0f, 0.8f, 0.8f);
	//-------------------------��ȯ���� �Է�--------------------------


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
	UpdateVBO(obj);

	// �ε����� ������ ��ǥ���� ������ 3���̹Ƿ� �� 6��
	glDrawElements(GL_LINES, obj.indexlist.size(), GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

GLvoid UpdateVBO(Object object) {
	// VAO ���ε�
	glBindVertexArray(vao);

	// ť�� ��ġ ���� ���ε�
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, object.vertex.size() * sizeof(glm::vec3), object.vertex.data(), GL_STREAM_DRAW);

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
	UpdateVBO(model);
}