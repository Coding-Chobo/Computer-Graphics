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
Object cube{}, tetra{}, corn{}, quad_pyra{}, temp{};
Object objfile{};
Coordinate Coordinate_system{}, spiral{};
Camera_Info camera{};

GLuint vao;
GLuint vbo[2];
GLuint EBO;

//�ǽ��� �ʿ��� ������
unsigned int shape{ 0 };
float theta = 0.0f;
float theta_2{};
float theta_3{};
float theta_5{};
float spiral_size{};
float radius = 0.0f;
glm::vec3 goal[4];
glm::vec3 goal2[4];
GLfloat orbit_angle_y{ 0.0f };  // ���� ����
bool mode_swab{ false };
bool mode_spin{ false };
unsigned int mode_animate{ 0 };
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

	//���� �Լ�
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeyboard);
	glutDisplayFunc(Render);
	glutIdleFunc(mainLoop); // ���� ����
	glutMainLoop(); //�̺�Ʈ ���� ����
}
//�׸��� �Լ�
GLvoid Render() {
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	// ��ǥ�� �׸���
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
	// �� location �ε��� ����
	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "trans");
	// ��ȯ ���� ������ �����մϴ�.
	glm::mat4 modelMatrix = glm::mat4(1.0f); // �� ��ȯ
	glm::mat4 viewMatrix = glm::mat4(1.0f);  // �� ��ȯ (ī�޶� ��ȯ)
	glm::mat4 projectionMatrix = glm::mat4(1.0f); // ���� ��ȯ (�������� ��ȯ)

	// �� ��ȯ
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	// ī�޶� ����: ī�޶� ��ġ, ī�޶� �ٶ󺸴� ��ġ, ���� �� ����
	viewMatrix = glm::lookAt(
		glm::vec3(camera.x, camera.y, camera.z), // ī�޶� ��ġ
		glm::vec3(camera.at_x, camera.at_y, camera.at_z), // ī�޶� �ٶ󺸴� ����
		glm::vec3(0.0f, 1.0f, 0.0f)  // ���� �� ����
	);

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

GLvoid Make_Matrix(float& orbit_angle, Object obj) {

	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "trans");
	// ��ȯ ���� ������ �����մϴ�.
	glm::mat4 modelMatrix = glm::mat4(1.0f);  // �� ��ȯ
	glm::mat4 viewMatrix = glm::mat4(1.0f);   // �� ��ȯ (ī�޶� ��ȯ)
	glm::mat4 projectionMatrix = glm::mat4(1.0f); // ���� ��ȯ (�������� ��ȯ)

	modelMatrix = glm::rotate(modelMatrix, glm::radians(30.f), glm::vec3(1.0f, 0.0f, 0.0f));  // X�� ȸ��
	modelMatrix = glm::rotate(modelMatrix, glm::radians(30.f), glm::vec3(0.0f, 1.0f, 0.0f));  // Y�� ȸ��

	// ���� ��ȯ: ��ü�� ������ �������� ȸ���ϰ� �մϴ�.
	//modelMatrix = glm::rotate(modelMatrix, glm::radians(orbit_angle), glm::vec3(0.0f, 1.0f, 0.0f));  // Y�� ����
	modelMatrix = glm::translate(modelMatrix, glm::vec3(obj.transform.x, obj.transform.y, obj.transform.z));  // ���� �Ÿ� ����
	if (mode_spin)
	{
		modelMatrix = glm::rotate(modelMatrix, glm::radians(orbit_angle), glm::vec3(0.0f, 1.0f, 0.0f));  // Y�� ����
	}


	// ���ڸ� ȸ��: ������ ��ġ���� ��ü�� ���ڸ����� ȸ����ŵ�ϴ�.
	modelMatrix = glm::rotate(modelMatrix, glm::radians(obj.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));  // X�� ȸ��
	modelMatrix = glm::rotate(modelMatrix, glm::radians(obj.rotation.x), glm::vec3(0.0f, 1.0f, 0.0f));  // Y�� ȸ��
	modelMatrix = glm::rotate(modelMatrix, glm::radians(obj.rotation.x), glm::vec3(0.0f, 0.0f, 1.0f));  // Z�� ȸ��

	// ũ�� ��ȯ
	modelMatrix = glm::scale(modelMatrix, glm::vec3(obj.scaling.x, obj.scaling.y, obj.scaling.z));


	// ī�޶� ����
	viewMatrix = glm::lookAt(
		glm::vec3(camera.x, camera.y, camera.z), // ī�޶� ��ġ
		glm::vec3(camera.at_x, camera.at_y, camera.at_z), // ī�޶� �ٶ󺸴� ����
		glm::vec3(0.0f, 1.0f, 0.0f)  // ���� �� ����
	);

	// ���� ����
	projectionMatrix = glm::perspective(
		glm::radians(45.0f), // �þ߰�
		(float)WIDTH / (float)HEIGHT, // ��Ⱦ��
		0.1f, 50.0f // Ŭ�� ��� (near, far)
	);

	// ���� ��ȯ ���(MVP)�� ����Ͽ� GPU�� ����
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
	case 1://����ü ���鸸 ȸ��
		theta += 2;
		
		for (int i = 0; i < 4; i++)
		{
			temp.vertex[i].x = cube.vertex[i].x * cos(glm::radians(theta));
			temp.vertex[i].y = cube.vertex[i].y * sin(glm::radians(theta));
		}

		break;
	case 3://����

		break;
	case 4://���Ʒ� �������
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
	glutPostRedisplay(); // �ٽ� �׸��� ��û
}

void init_figure() {
	//---------------------------obj����-----------------------------
	objfile.vertex.clear();
	objfile.color.clear();
	objfile.indexlist.clear();
	read_obj_file("2021184024.obj", objfile);
	//-----------------���� ��ȯ ��ǥ �Է�--------------------------
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

	// ���� �߰�
	obj.vertex.emplace_back(glm::vec3{ 0.0f, 0.0f, 0.0f });  // ����
	obj.vertex.emplace_back(glm::vec3{ 5.0f, 0.0f, 0.0f });  // X��
	obj.vertex.emplace_back(glm::vec3{ 0.0f, 0.0f, 0.0f });  // ����
	obj.vertex.emplace_back(glm::vec3{ 0.0f, 5.0f, 0.0f });  // Y��
	obj.vertex.emplace_back(glm::vec3{ 0.0f, 0.0f, 0.0f });  // ����
	obj.vertex.emplace_back(glm::vec3{ 0.0f, 0.0f, 5.0f });  // Z��

	// ���� �߰� (�� ���� ���� ����)
	obj.color.emplace_back(glm::vec3{ 1.0f, 0.0f, 0.0f });  // X��: ����
	obj.color.emplace_back(glm::vec3{ 1.0f, 0.0f, 0.0f });  // X��: ����
	obj.color.emplace_back(glm::vec3{ 0.0f, 0.0f, 1.0f });  // Y��: �Ķ�
	obj.color.emplace_back(glm::vec3{ 0.0f, 0.0f, 1.0f });  // Y��: �Ķ�
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 0.0f });  // Z��: �ʷ�
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

void Make_Cube(float x, float y, float z, float size, Object& obj) {
	obj.vertex.clear();
	obj.color.clear();
	obj.indexlist.clear();
	obj.transform = { 0.0f,0.0f,0.0f };
	obj.scaling = { 1.0f,1.0f,1.0f };
	obj.rotation = { 0.0f,0.0f,0.0f };
	// ���� �߰�
	obj.vertex.emplace_back(glm::vec3{ x - size, y + size, z - size });
	obj.vertex.emplace_back(glm::vec3{ x - size, y + size, z + size });
	obj.vertex.emplace_back(glm::vec3{ x + size, y + size, z + size });
	obj.vertex.emplace_back(glm::vec3{ x + size, y + size, z - size });

	obj.vertex.emplace_back(glm::vec3{ x - size, y - size, z - size });
	obj.vertex.emplace_back(glm::vec3{ x - size, y - size, z + size });
	obj.vertex.emplace_back(glm::vec3{ x + size, y - size, z + size });
	obj.vertex.emplace_back(glm::vec3{ x + size, y - size, z - size });

	// ���� �߰�
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 1.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f, 0.0f, 1.0f });
	obj.color.emplace_back(glm::vec3{ 1.0f, 0.0f, 1.0f });

	obj.color.emplace_back(glm::vec3{ 1.0f, 0.0f, 0.0f });
	obj.color.emplace_back(glm::vec3{ 1.0f, 1.0f, 0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 1.0f });

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

	// ���� �߰�
	obj.color.emplace_back(glm::vec3{ 1.0f, 1.0f, 0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 1.0f });
	obj.color.emplace_back(glm::vec3{ 1.0f, 0.0f, 1.0f });
	obj.color.emplace_back(glm::vec3{ 0.5f, 0.5f, 0.5f });

	// �ε��� ����Ʈ �߰�
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

	// ���� �߰�
	obj.color.emplace_back(glm::vec3{ 1.0f, 1.0f, 0.0f });
	obj.color.emplace_back(glm::vec3{ 0.0f, 1.0f, 1.0f });
	obj.color.emplace_back(glm::vec3{ 1.0f, 0.0f, 1.0f });
	obj.color.emplace_back(glm::vec3{ 0.5f, 0.5f, 0.5f });

	// �ε��� ����Ʈ �߰�
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
	//��ü�� ���庯ȯ �Ű������� �ʱ�ȭ	
	obj.transform = { 0.0f,0.0f,0.0f };
	obj.scaling = { 1.0f,1.0f,1.0f };
	obj.rotation = { 0.0f,0.0f,0.0f };
	const int cnt = 20;
	float theta_corn[cnt];
	for (int i = 0; i < cnt; i++)
	{
		theta_corn[i] = i * (360 / cnt);
	}

	//�����߰�
	for (int i = 0; i < cnt; i++)
	{
		obj.vertex.emplace_back(glm::vec3{ x + size * cos(glm::radians(theta_corn[i])), y, z + size * sin(glm::radians(theta_corn[i])) });
	}
	obj.vertex.emplace_back(glm::vec3{ x, y + 2 * size, z });
	obj.vertex.emplace_back(glm::vec3{ x, y, z });

	// ���� �߰�
	for (int i = 0; i < obj.vertex.size(); i++)
	{
		float r = static_cast<float>(rand()) / RAND_MAX;
		float g = static_cast<float>(rand()) / RAND_MAX;
		float b = static_cast<float>(rand()) / RAND_MAX;
		obj.color.emplace_back(glm::vec3{ r,g,b });
	}
	// �ε��� ����Ʈ �߰�
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

	//�����߰�
	for (int i = 0; i < 5 * cnt; i++)
	{
		size += 0.02f;
		obj.vertex.emplace_back(glm::vec3{ x + size * cos(glm::radians(theta_sprial[i % cnt])), y, z + size * sin(glm::radians(theta_sprial[i % cnt])) });
	}

	// ���� �߰�
	for (int i = 0; i < obj.vertex.size(); i++)
	{
		float r = static_cast<float>(rand()) / RAND_MAX;
		float g = static_cast<float>(rand()) / RAND_MAX;
		float b = static_cast<float>(rand()) / RAND_MAX;
		obj.color.emplace_back(glm::vec3{ r,g,b });
	}
	// �ε��� ����Ʈ �߰�

	for (unsigned int i = 0; i < obj.vertex.size(); i++)
	{
		obj.indexlist.emplace_back(i);
	}
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
		model.color.push_back(glm::vec3(1.0f, 1.0f, 1.0f)); // ���������� ����
	}
	model.transform = { 0.0f,0.0f,0.0f };
	model.scaling = { 1.0f,1.0f,1.0f };
	model.rotation = { 0.0f,0.0f,0.0f };
	UpdateVBO(model);
}